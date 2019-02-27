#include "msg_filter_circle_call_n.h"
#include <fstream>
namespace auto_future_utilities
{
using namespace std;
msg_filter_circle_call_n::msg_filter_circle_call_n()
{
}

msg_filter_circle_call_n::~msg_filter_circle_call_n()
{
}

void msg_filter_circle_call_n::parse_protocol_from_json_unit(Json::Value& junit)
{
	using namespace Json;
	Value jname = junit["name"];
	Value jgroup = junit["group"];
	if (jgroup.isNull())
	{
		return;
	}
	Value jtype = jgroup["type"];
	if (jtype.asString() != "by_circle")
	{
		if (_psuccessor)
		{
			return _psuccessor->parse_protocol_from_json_unit(junit);
		}
		return;
	}
	Value jgid = jgroup["id"];

	_map_msg_id_grop_id[jname.asString()] = jgid.asInt();
		
	auto it_cm_ut = _map_group_id_v_comsume_unit.find(jgid.asInt());
	if (it_cm_ut != _map_group_id_v_comsume_unit.end())
	{
		auto vmut = it_cm_ut->second;
		vmut.push_back(msg_consume_unit(jname.asString()));
	}
	else
	{
		std::vector<msg_consume_unit> vmsgut;
		vmsgut.push_back(msg_consume_unit(jname.asString()));
		_map_group_id_v_comsume_unit[jgid.asInt()] = vmsgut;
	}
		
}

void msg_filter_circle_call_n::load_tactic_from_config(const char* config_path)
{

}

bool msg_filter_circle_call_n::attach_handle(const char* pkey_name, msg_handle fn_obj)
{
	auto it_gp_id = _map_msg_id_grop_id.find(pkey_name);
	if (it_gp_id == _map_msg_id_grop_id.end())
	{
		printf("$fail to attach key_name:%s to a group\n", pkey_name);
		return false;
	}
	auto it_msg_consume_unit = _map_group_id_v_comsume_unit.find(it_gp_id->second);
	if (it_msg_consume_unit == _map_group_id_v_comsume_unit.end())
	{
		printf("$fail to find key_name:%s from group:%d\n", pkey_name, it_gp_id->second);
		return false;
	}
	auto v_cosume_unit = it_msg_consume_unit->second;
	for (auto it_cu = v_cosume_unit.begin(); it_cu != v_cosume_unit.end(); it_cu++)
	{
		if (it_cu->_key_name == pkey_name)
		{
			swap(it_cu->_handle, fn_obj);
			return true;
		}
	}
	printf("$fail to find key_name:%s from group:%d\n", pkey_name, it_gp_id->second);
	return false;
}

bool msg_filter_circle_call_n::handle_custom_data(const char* pkey_name, u8* pvalue, int len)
{
	auto it_gp_id = _map_msg_id_grop_id.find(pkey_name);
	if (it_gp_id == _map_msg_id_grop_id.end())
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
	if (it_msg_consume_unit == _map_group_id_v_comsume_unit.end())
	{
		printf("$fail to find key_name:%s from group:%d\n", pkey_name, it_gp_id->second);
		return false;
	}
	auto v_cosume_unit = it_msg_consume_unit->second;
	//bool bfinded = false;
	for (auto it_cu = v_cosume_unit.rbegin(); it_cu != v_cosume_unit.rend(); it_cu++)
	{
		if (it_cu->_key_name == pkey_name)
		{

			it_cu->_value = *pvalue;
			//it_cu->_handle(&it_cu->_value, 1);
			return true;
		}
	}
	printf("$$fail to find key_name:%s from group:%d\n", pkey_name, it_gp_id->second);
	return false;
}

void msg_filter_circle_call_n::circle_excute()
{

}

}
