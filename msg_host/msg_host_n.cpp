#include "msg_host_n.h"
#include <fstream>
#include <cstring>
//#include "RmtBusReceiver.h"
namespace auto_future_utilities
{ 
 bool msg_host_n::key_unit::is_byid = true;
msg_host_n::msg_host_n()
	:_task_entry(0)
	, _be_running(false)
{
	//load_protocol_from_config(protocol_path);
	//msg_host_n::key_unit::compare_type = msg_host_n::key_unit::en_by_id;

	_root_protocol_receive_data_map._is_root = true;
}


msg_host_n::~msg_host_n()
{
}

void msg_host_n::load_protocol_from_json(Value& junit, data_unit& recv)
{
	//key_unit ku(junit["id"].asInt(), junit["name"].asString());
	//data_unit du;
	auto p_nchd = new data_unit();
	u8 uid = junit["id"].asInt();
	string iname = junit["name"].asString();
	auto rst=recv._child.insert(make_pair(key_unit(uid,iname), p_nchd));
	if (!rst.second)
	{
		printf("fail to insert:[%d,%s]\n",uid,iname.c_str());
	}
	Value vchild = junit["child"];
	if (!vchild.isNull())
	{
		int isize = vchild.size();
		for (int ix = 0; ix < isize; ix++)
		{
			Value vunit = vchild[ix];
			//data_unit vdu;
			//recv._child[vunit] = vdu;
			load_protocol_from_json(vunit,*p_nchd);
			//if (_task_entry)
			//{
			//	_task_entry->parse_protocol_from_json_unit(vunit);
			//}
		}
	}
	

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
			load_protocol_from_json(root, _root_protocol_receive_data_map);
		}
		fin.close();
	}
	else
	{
		printf("invalid config_path:%s\n",config_path);
		return;
	}
}

void msg_host_n::load_send_protocol_from_json(Value& junit,vector<u8>* pparent_id)
{
	try
	{
		u8 uid = junit["id"].asInt();
		string iname = junit["name"].asString();
		_protocol_send_data_map.insert(make_pair(iname, vector<u8>()));
		if (pparent_id)
		{
			_protocol_send_data_map[iname].assign(pparent_id->begin(), pparent_id->end());
		}
		_protocol_send_data_map[iname].push_back(uid);
		Value& jchild = junit["child"];
		if (jchild.isArray())
		{
			int isize = jchild.size();
			for (int ix = 0; ix < isize;ix++)
			{
				Value& jcd_unit = jchild[ix];
				load_send_protocol_from_json(jcd_unit, &_protocol_send_data_map[iname]);
			}
		}
	}
	catch (std::exception& e)
	{
		cout << "load send protocol error:" << e.what() << endl;
	}
	
}
void msg_host_n::load_send_protocol_from_config(const char* config_path)
{
	ifstream fin;
	fin.open(config_path);
	if (fin.is_open())
	{
		Reader reader;
		Value root;
		if (reader.parse(fin, root, false))
		{
			Value& jchilds = root["child"];
			int isize = jchilds.size();
			for (int ix = 0; ix < isize;ix++)
			{
				Value& junit = jchilds[ix];
				load_send_protocol_from_json(junit);
			}
		}
		fin.close();
	}
	else
	{
		printf("invalid send config_path:%s\n", config_path);
		return;
	}
}

msg_host_n::data_unit& msg_host_n::find_data_unit_by_key_unit(key_unit& ku, msg_host_n::data_unit& data_host)
{
	auto& chld = data_host._child;
	for (auto iit:chld)
	{
        auto& irtn = *iit.second;
		if (iit.first._name==ku._name)
		{
			return irtn;
		}
		else
		{
			auto& sbtar = find_data_unit_by_key_unit(ku, irtn);
			if (!sbtar._is_root)
				return sbtar;
		}
	}
	return _root_protocol_receive_data_map;
}

bool msg_host_n::attach_monitor(const char* object_name, msg_handle fn_obj)
{
	key_unit::is_byid = false;
	key_unit kut(0,object_name);
	auto& tar_data_unit = find_data_unit_by_key_unit(kut, _root_protocol_receive_data_map);
	if (tar_data_unit._is_root)
	{
		printf("???????fail to configure object:%s!\n",object_name);
		return false;
	}
	else
	{
		//printf("!!!!!configure object:%s!\n",object_name);
		if (_task_entry)
		{
			_task_entry->attach_handle(object_name, fn_obj);
		}
		swap(tar_data_unit._handle, fn_obj);
		_mvalid_data_buff[object_name] = &tar_data_unit;
		return true;
	}
}

msg_host_n::data_unit& msg_host_n::find_data_unit_by_id(u8* pin_buff, msg_host_n::data_unit& data_host, u8*& pout_buff, string& rtn_key_name)
{
	pout_buff = pin_buff;
	key_unit kut(*pin_buff, "");
	auto& chld = data_host._child;
	auto tar = chld.find(kut);
	if (tar!=chld.end())
	{
		rtn_key_name = tar->first._name;
		return find_data_unit_by_id(++pin_buff, *tar->second, pout_buff,rtn_key_name);
	}
	return data_host;
}

void msg_host_n::pick_valid_data(u8* pbuff, int len)
{
	//key_unit::is_byid = true;
	//pbuff--;
	//len++;
	//*pbuff = 0;//for find root
	//cruise set cmdid:060016
	u8* pvalue = pbuff;
	auto& root_child = _root_protocol_receive_data_map._child;
	auto& valid_child = root_child[key_unit(0,"")];
	string key_name;
	auto& target = find_data_unit_by_id(pbuff, *valid_child, pvalue,key_name);
	if (target._is_root)
	{
		printf("invalid head data:%x \n", *pbuff);
	}
	int vlen = len - (pvalue - pbuff);
	if (vlen>0)
	{
		target._data_buff.emplace(vdata_list(pvalue, pvalue + vlen));
	}
	else
	{
		printf("cmd:%s is invalid\n", key_name.c_str());
	}
	/*if (!_task_entry||!(_task_entry->handle_custom_data(key_name.c_str(),pvalue,vlen)))
	{
		if (target._handle)
		{
			target._handle(pvalue, vlen);
		}
		else
		{
			
			printf("fail to handle cmd:");
			for (int ix = 0; ix < len; ix++)
			{
				printf("%d ", pbuff[ix]);
			}
			printf("\n");
		}
	}*/
	
}
void msg_host_n::execute_data_handle_cmd()
{
	for (auto& dtunit : _mvalid_data_buff)
	{
		auto& keyname = dtunit.first;
		auto& msg_hdl = dtunit.second->_handle;
		auto& databf = dtunit.second->_data_buff;
		if (!_task_entry)
		{
			while (!databf.empty())
			{
				auto& datal = databf.front();
				if (datal.size()>0)
				{
					msg_hdl(&datal.at(0), datal.size());
				}
				databf.pop();
			}
		}
		else
		{
			while (!databf.empty())
			{
				auto& datal = databf.front();
				if (!(_task_entry->handle_custom_data(keyname.c_str(), &datal.at(0), datal.size())))
					msg_hdl(&datal.at(0), datal.size());
				databf.pop();
			}

		}

	}
}
void msg_host_n::send_data(const char* object_name,u8* pbuff, int len)
{
   if (_send_cmd)
   {
	   auto ivdata = _protocol_send_data_map.find(object_name);
	   if (ivdata!=_protocol_send_data_map.end())
	   {
		   int dlen = ivdata->second.size() + len;
		   u8* psdata = new u8[dlen];
		   memcpy(psdata, &*ivdata->second.begin(), ivdata->second.size());
		   memcpy(psdata + ivdata->second.size(), pbuff, len);
		   _send_cmd(psdata, dlen);
		   delete psdata;
	   }
   }
}
}
