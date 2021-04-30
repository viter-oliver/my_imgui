#include "msg_host_n.h"
#include <fstream>
#include <cstring>
#include <chrono>
#include <stdio.h>
//#include "RmtBusReceiver.h"
namespace auto_future_utilities
{ 
msg_host_n::msg_host_n()
	: _be_running(false)
{
	//load_protocol_from_config(protocol_path);
	//msg_host_n::key_unit::compare_type = msg_host_n::key_unit::en_by_id;
	
}


msg_host_n::~msg_host_n()
{
}


void msg_host_n::load_protocol_from_config(const char* config_path)
{
	ifstream fin;
	fin.open(config_path);
	if (fin.is_open())
	{
		Reader reader;
		Value root;
		if (reader.parse(fin,root,false))
		{
			Value& cmd_list = root["mcu_cmd_list"];
			int isz = cmd_list.size();
			for (int ix = 0; ix < isz;ix++)
			{
				Value& cmd_unit = cmd_list[ix];
				Value& jname = cmd_unit["name"];
				Value& jid = cmd_unit["id"];
				Value& vsub_unit = cmd_unit["sub_units"];
				auto ndu = make_shared<data_unit>();
				int jsz = vsub_unit.size();
				for (int ii = 0; ii < jsz;ii++)
				{
					Value& jsbut = vsub_unit[ii];
					ndu->_vsub_unit.emplace_back();
					auto& subu = ndu->_vsub_unit.back();
					subu._name = jsbut["name"].asString();
					subu._seg_len = jsbut["seg_len"].asUInt();
				}
				_recieve_protocol[jid.asString()] = ndu;
				_rec_id_name[jname.asString()] = jid.asString();
			}
			Value& mpu_cmd_list = root["mpu_cmd_list"];
			isz = mpu_cmd_list.size();
			for (int ix = 0; ix < isz; ix++)
			{
				Value& cmd_unit = mpu_cmd_list[ix];
				Value& jname = cmd_unit["name"];
				Value& jid = cmd_unit["id"];
				Value& vsub_unit = cmd_unit["sub_units"];
				auto ndu = make_shared<data_unit>();
				int jsz = vsub_unit.size();
				for (int ii = 0; ii < jsz; ii++)
				{
					Value& jsbut = vsub_unit[ii];
					ndu->_vsub_unit.emplace_back();
					auto& subu = ndu->_vsub_unit.back();
					subu._name = jsbut["name"].asString();
					subu._seg_len = jsbut["seg_len"].asUInt();
				}
				_send_protocol[jid.asString()] = ndu;
				_sd_id_name[jname.asString()] = jid.asString();
			}
		}
		fin.close();
	}
	else
	{
		printf("invalid config_path:%s\n",config_path);
		return;
	}
}

bool msg_host_n::attach_monitor(const char* object_name, msg_handle fn_obj)
{
	const auto& ircl = _rec_id_name.find(object_name);
	if (ircl == _rec_id_name.end())
	{
		printf("???????fail to configure object:%s!\n", object_name);
		return false;
	}
	auto& idkey = ircl->second;
	const auto& ipt = _recieve_protocol.find(idkey);
	if (ipt==_recieve_protocol.end())
	{
	}
	else
	{
		//printf("!!!!!configure object:%s!\n",object_name);

		swap((*ipt->second)._handle, fn_obj);
		return true;
	}
}

bool msg_host_n::attach_monitor(const char* object_name, const char* seg_name, msg_handle fn_obj)
{
	const auto& ircl = _rec_id_name.find(object_name);
	if (ircl == _rec_id_name.end())
	{
		printf("???????fail to configure object:%s!\n", object_name);
		return false;
	}
	auto& idkey = ircl->second;
	const auto& ipt = _recieve_protocol.find(idkey);
	if (ipt==_recieve_protocol.end())
	{
		printf("???????fail to configure object:%s!\n",object_name);
		return false;
	}
	else
	{
		//printf("!!!!!configure object:%s!\n",object_name);

		auto& datau = *ipt->second;
		for (auto&sut:datau._vsub_unit)
		{
			if (sut._name==seg_name)
			{
				swap(sut._handle, fn_obj);
				return true;
			}
		}
		return false;
	}
}
bool msg_host_n::attach_monitor(const char* object_name, u8 seg_id, msg_handle fn_obj)
{
	const auto& ircl = _rec_id_name.find(object_name);
	if (ircl == _rec_id_name.end())
	{
		printf("???????fail to configure object:%s!\n", object_name);
		return false;
	}
	auto& idkey = ircl->second;
	const auto& ipt = _recieve_protocol.find(idkey);
	if (ipt == _recieve_protocol.end())
	{
		printf("???????fail to configure object:%s!\n", object_name);
		return false;
	}
	else
	{
		//printf("!!!!!configure object:%s!\n",object_name);

		auto& datau = *ipt->second;
		if (seg_id<datau._vsub_unit.size())
		{
			swap(datau._vsub_unit[seg_id]._handle, fn_obj);
			return true;
		}
		return false;
	}
}
void print_buff0(u8* pbuff, int len)
{
	static char num_tb[0x10] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};
	struct buff_data
	{
		u8 data_l : 4;
		u8 data_h : 4;
	};
	string pt_buff;
	while (len > 0)
	{
		buff_data* pdata = (buff_data*)pbuff++;
		len--;
		pt_buff += " x";
		pt_buff += num_tb[pdata->data_h];
		pt_buff += num_tb[pdata->data_l];
	}
	printf("%s\n", pt_buff.c_str());
}

void msg_host_n::pick_valid_data(u8* pbuff, int len)
{
//print_buff(pbuff,len);
    // print_buff0(pbuff,len);
	auto& cur_cmd = _cmd_queue[_front_id];
	if (len>static_buff_len)
	{
		cur_cmd._pdynamic_buff = new u8[len];
		memcpy(cur_cmd._pdynamic_buff, pbuff, len);
	}
	else
	{
		memcpy(cur_cmd._static_buff, pbuff, len);
	}
	cur_cmd._data_len = len;
	unsigned int front_next_id = _front_id + 1;
	front_next_id %= queque_len;
	
	if (front_next_id == _rear_id)
	{
		printf("too fast.slow down please!!!!\n");
	    this_thread::sleep_for(chrono::milliseconds(50));
	}
	_front_id = front_next_id;
	
}
static char num_tb[0x10] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};
 
void msg_host_n::execute_data_handle_cmd()
{
	
	for (; _rear_id != _front_id;++_rear_id,_rear_id=_rear_id %queque_len)
	{
		auto& bunt = _cmd_queue[_rear_id];
		u8* pdata = bunt._data_len > static_buff_len ? bunt._pdynamic_buff : bunt._static_buff;
		static string kid("0000");
		u8 cmd_len = 2;

		if (_get_cmd)
		{
			cmd_len = _get_cmd(pdata,kid);
		}
		else
		{
			struct cmd_data
			{
				u8 data0 : 4;
				u8 data1 : 4;
				u8 data2 : 4;
				u8 data3 : 4;
			};
			cmd_data* pcmd = (cmd_data*)pdata;		
			kid[0] = num_tb[pcmd->data3]; kid[1] = num_tb[pcmd->data2];
			kid[2] = num_tb[pcmd->data1]; kid[3] = num_tb[pcmd->data0];
		}
		
		pdata += cmd_len;
		const auto& idata_u = _recieve_protocol.find(kid);
		if (idata_u!=_recieve_protocol.end())
		{
			auto& data_u = *idata_u->second;
			if (data_u._handle)
			{
				data_u._handle(pdata, bunt._data_len-2);
			}
			else
			{
				for (auto&sun:data_u._vsub_unit)
				{
					if (sun._handle)
					{
						sun._handle(pdata, sun._seg_len);
					}
					pdata += sun._seg_len;
				}
			}
		}
		else
		{
			//printf("unkown command:%s datalen=%d\n",kid, bunt._data_len);
		}
		if (bunt._pdynamic_buff)
		{
			delete[] bunt._pdynamic_buff;
			bunt._pdynamic_buff = nullptr;
		}
	}
}
void msg_host_n::send_data(const char* object_name,u8* pbuff, int len)
{
   if (_send_cmd)
   {
	   const auto& icmd_id = _sd_id_name.find(object_name);
	   if (icmd_id!=_sd_id_name.end())
	   {
		   
		   auto& str_cmd_id=icmd_id->second;
		   string wcmd_id="0x"+str_cmd_id;
		   u16 cmd_id;
		   sscanf(wcmd_id.c_str(),"%x",&cmd_id);
		   int dlen = 2 + len;
		   u8* psdata = new u8[dlen];
		   memcpy(psdata, &cmd_id, 2);
		   memcpy(psdata + 2, pbuff, len);
		   _send_cmd(psdata, dlen+2);
		   delete psdata;
	   }
   }
}
}
