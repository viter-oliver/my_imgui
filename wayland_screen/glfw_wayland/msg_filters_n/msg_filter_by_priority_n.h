/********************************************************************
	created:	2017/11/06
	created:	6:11:2017   11:52
	filename: 	msg_filter_by_priority.h
	file base:	msg_filter_by_priority
	file ext:	h
	author:		viter
	
	purpose:	scheduling the switch variable by it's priority
*********************************************************************/
#pragma once
#include "base_filter_n.h"
#include <map>
#include <vector>
namespace auto_future_utilities
{ 
	/*
	基于优先级的调度，必须假定所有调度数据是开关量，也就是说value非0即1
	*/
	using namespace std;
	class AFG_EXPORT msg_filter_by_priority_n :public base_filter_n
	{
	struct msg_consume_unit 
	{
		string _key_name;
		u8 _priority;//越小则越大，则排到前面		
		msg_handle _handle;
		u8 _value;
		bool _be_preempted;
		msg_consume_unit(string& ky_nm) :_key_name(ky_nm), _priority(0), _value(0), _be_preempted(false){}
		msg_consume_unit(string&& ky_nm) :_key_name(ky_nm), _priority(0), _value(0), _be_preempted(false){}
		msg_consume_unit(string& ky_nm, u8 pry) :_key_name(ky_nm), _priority(pry), _value(0), _be_preempted(false){}
		msg_consume_unit(string&& ky_nm, u8 pry) :_key_name(ky_nm), _priority(pry), _value(0), _be_preempted(false){}

	};
	typedef std::map<string, u16> map_msg_name_grop_id;
	typedef std::map<u16, std::vector<msg_consume_unit>> map_group_id_v_comsume_unit;

		map_msg_name_grop_id _map_msg_name_grop_id;
		map_group_id_v_comsume_unit _map_group_id_v_comsume_unit;
	public:
		msg_filter_by_priority_n();
		~msg_filter_by_priority_n();
		void parse_protocol_from_json_unit(Json::Value& junit);
		void load_tactic_from_config(const char* config_path);
		bool attach_handle(const char* pkey_name, msg_handle fn_obj);
		bool handle_custom_data(const char* pkey_name, u8* pvalue, int len);
	};
}
