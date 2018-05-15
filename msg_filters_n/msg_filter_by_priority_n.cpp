#include "msg_filter_by_priority_n.h"
#include "priority_filter_def.h"

//#include <algorithm> 
#include <fstream>

namespace auto_future_utilities
{
	using namespace std;
msg_filter_by_priority_n::msg_filter_by_priority_n()
{
}

msg_filter_by_priority_n::~msg_filter_by_priority_n()
{
}
/*
为什么不用Json::value 作为参数类型？
是为了通用性，不想把对协议数据的控制限制在json数据格式
*/
void msg_filter_by_priority_n::parse_protocol_from_json_unit(Json::Value& junit)
{
    using namespace Json;
	Value jgroup = junit["group"];
	if (jgroup.isNull())
	{
		return;
	}
	Value jtype = jgroup["type"];
	if (jtype.asString() != "by_priority")
	{
		if (_psuccessor)
		{
			return _psuccessor->parse_protocol_from_json_unit(junit);
		}
		return;
	}
	Value jname = junit["name"];
	Value jgid = jgroup["id"];
	Value jpriority = jgroup["priority"];
	_map_msg_name_grop_id[jname.asString()] = jgid.asInt();
	auto it_cm_ut = _map_group_id_v_comsume_unit.find(jgid.asInt());
	if (it_cm_ut != _map_group_id_v_comsume_unit.end())
	{
		auto vmut = it_cm_ut->second;
		auto it_msg_u = vmut.begin();
		for (; it_msg_u != vmut.end(); it_msg_u++)
		{
			if (it_msg_u->_priority > jpriority.asInt())
			{
				vmut.insert(it_msg_u, msg_consume_unit(jname.asString(),jpriority.asInt()));
				break;
			}
		}
		vmut.push_back(msg_consume_unit(jname.asString(), jpriority.asInt()));

	}
	else
	{
		std::vector<msg_consume_unit> vmsgut;
		vmsgut.push_back(msg_consume_unit(jname.asString(), jpriority.asInt()));
		_map_group_id_v_comsume_unit[jgid.asInt()] = vmsgut;
	}	
}

void msg_filter_by_priority_n::load_tactic_from_config(const char* config_path)
{
	using namespace Json;
	ifstream fin;
	fin.open(config_path);
	if (fin.is_open())
	{
		Reader reader;
		Value root;
		if (reader.parse(fin, root, false))
		{
			int isize = root.size();
			for (int i = 0; i < isize; i++)
			{
				Value ju = root[i];
				auto groupid = ju["groupid"].asUInt();
				Value members = ju["members"];
				
				int iisize = members.size();
				std::vector<msg_consume_unit> vmsgut;
				_map_group_id_v_comsume_unit[groupid] = vmsgut;
				for (int ii = 0; ii < iisize; ii++)
				{
					Value mb = members[ii];
					_map_group_id_v_comsume_unit[groupid].push_back(msg_consume_unit(mb.asString()));
					_map_msg_name_grop_id[mb.asString()] = groupid;
				}
			}
			printf("get %d msg objects\n", isize);
		}
		fin.close();
	}
	else
	{
		printf("invalid config_path:%s\n", config_path);
		return;
	}

}

bool msg_filter_by_priority_n::attach_handle(const char* pkey_name, msg_handle fn_obj)
{
	auto it_gp_id = _map_msg_name_grop_id.find(pkey_name);
	if (it_gp_id == _map_msg_name_grop_id.end())
	{
		printf("fail to attach key_name:%s to a group\n", pkey_name);
		return false;
	}
	auto it_msg_consume_unit = _map_group_id_v_comsume_unit.find(it_gp_id->second);
	if (it_msg_consume_unit == _map_group_id_v_comsume_unit.end())
	{
		printf("@fail to find key_name:%s from group:%d\n", pkey_name, it_gp_id->second);
		return false;
	}
	auto& v_cosume_unit = it_msg_consume_unit->second;
	for (auto it_cu = v_cosume_unit.begin(); it_cu != v_cosume_unit.end(); it_cu++)
	{
		if (it_cu->_key_name == pkey_name)
		{
			swap(it_cu->_handle, fn_obj);
			return true;
		}
	}
    printf("@fail to find key_name:%s from group:%d\n", pkey_name, it_gp_id->second);
	return false;

}

bool msg_filter_by_priority_n::handle_custom_data(const char* pkey_name, u8* pvalue, int len)
{
	auto it_gp_id = _map_msg_name_grop_id.find(pkey_name);
	if (it_gp_id == _map_msg_name_grop_id.end())
	{
		if (_psuccessor)
		{
			return _psuccessor->handle_custom_data(pkey_name, pvalue, len);
		}
		else
		{
			return false;
		}
	}
	auto it_msg_consume_unit = _map_group_id_v_comsume_unit.find(it_gp_id->second);
	if (it_msg_consume_unit==_map_group_id_v_comsume_unit.end())
	{
		printf("#fail to find key_name:%s from group:%d\n", pkey_name, it_gp_id->second);
		return false;
	}
	auto& v_cosume_unit = it_msg_consume_unit->second;
	u8 bvalue = *pvalue;
	//bool bfinded = false;
	/*if (strcmp("peps warning", pkey_name) == 0)
	{
		printf("object: %s will be handled by filter\n", pkey_name);

	}
	*/
	for (auto it_cu = v_cosume_unit.rbegin(); it_cu != v_cosume_unit.rend();it_cu++)
	{
		if (it_cu->_key_name == pkey_name)
		{
			if (!bvalue)
			{
				if (it_cu->_value)//通知客户端状态关闭
				{
					it_cu->_value = en_op_off;
					it_cu->_be_preempted = false;
					if (it_cu->_handle)
					{
						it_cu->_handle(&bvalue, 1);
					}
					
				}
				//同时解除被自己抢占的msg cosume unit ,即 en_op_preempted--->en_op_on
				if (it_cu == v_cosume_unit.rbegin())
				{
					break;
				}
				vector<msg_consume_unit>::iterator itf(it_cu.base());
				itf++;
				for (;itf!=v_cosume_unit.end();itf++)
				{
					if (itf->_be_preempted&&itf->_value)
					{
						itf->_value = en_op_on;
						if (itf->_handle)
						{
							itf->_handle(&itf->_value, 1);
						}		
						itf->_be_preempted = false;
						break;
					}
				}
			}
			else
			{
				it_cu->_value = en_op_on;
				it_cu->_be_preempted = false;
				auto it_next=it_cu+1;
				for (; it_next != v_cosume_unit.rend();it_next++)//寻找比自己优先级高的对象是否已经打开，如果打开则自己被抢占
				{
					if (it_next->_value)
					{
						it_cu->_value = en_op_preempted;
						it_cu->_be_preempted = true;
						break;
					}
				}
				if (it_cu->_handle)
				{
					it_cu->_handle(&it_cu->_value, 1);
				}
			}
			return true;
		}
	}
	printf("##fail to find key_name:%s from group:%d\n", pkey_name, it_gp_id->second);
	return false;
}

}