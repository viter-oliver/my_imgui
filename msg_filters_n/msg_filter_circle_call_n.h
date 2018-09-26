/********************************************************************
	created:	2017/11/06
	created:	6:11:2017   14:17
	filename: 	msg_filter_circle_call.h
	file base:	msg_filter_circle_call
	file ext:	h
	author:		viter
	
	purpose:	scheduling the switch variable by circle
*********************************************************************/
#pragma once
#include "base_filter_n.h"
#include <map>
#include <vector>
namespace auto_future_utilities
{
	using namespace std;
	class AFG_EXPORT msg_filter_circle_call_n :
		public base_filter_n
	{
		struct msg_consume_unit
		{
			string _key_name;
			msg_handle _handle;
			u8 _value;
			msg_consume_unit(string& nm) :_key_name(nm),_value(0){}
			msg_consume_unit(string&& nm) :_key_name(nm), _value(0){}
		};
		typedef std::map<string, u16> map_msg_id_grop_id;
		typedef std::map<u16, std::vector<msg_consume_unit>> map_group_id_v_comsume_unit;
		map_msg_id_grop_id _map_msg_id_grop_id;
		map_group_id_v_comsume_unit _map_group_id_v_comsume_unit;
	public:
		msg_filter_circle_call_n();
		~msg_filter_circle_call_n();
		void parse_protocol_from_json_unit(Json::Value& junit);
		void load_tactic_from_config(const char* config_path);
		bool attach_handle(const char* pkey_name, msg_handle fn_obj);
		bool handle_custom_data(const char* pkey_name, u8* pvalue, int len);
		void circle_excute();
	};
}
