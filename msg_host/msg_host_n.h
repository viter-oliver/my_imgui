#pragma once
#include "utilitis_base_def.h"
#include<map>
#include<mutex>
#include <condition_variable>
#include <thread>
#include "base_filter_n.h"
#include <memory>
#include <queue>
//#include "af_message_queue.h"
//#include<semaphore.h>

namespace auto_future_utilities
{
using namespace std;
using namespace Json;
typedef vector<u8> vdata_list;
//typedef unsigned char u8;
class AFG_EXPORT msg_host_n
{

	struct key_unit
	{
		static bool is_byid;
		u8 _id;
		string _name;
		key_unit() :_id(0){}
		key_unit(u8 id, const string& mname) :_id(id), _name(mname){ _name.shrink_to_fit(); }
		/*
		bool operator ==(const key_unit& ku) const
		{
			return key_unit::is_byid ? this->_id == ku._id : this->_name == ku._name;
		}*/
		bool operator <(const key_unit& ku) const
		{
			return _id < ku._id;
			/*if (key_unit::is_byid)
			{
				
			}
			else
			{
				return _name < ku._name;
			}*/
		}
		
	};
private:
	struct data_unit
	{
		msg_handle _handle;
		queue<vdata_list> _data_buff;
        map<key_unit, data_unit*> _child;
		//unique_ptr<map<key_unit, data_unit> > _pchild;
		bool _is_root;
		data_unit():_is_root(false){}
		~data_unit()
		{
			for (auto itu:_child)
			{
				delete itu.second;
			}
			_child.clear();
		}
		
	};
	data_unit _root_protocol_receive_data_map;
	map<string,data_unit*> _mvalid_data_buff;
	map<string, vector<u8>> _protocol_send_data_map;
	void load_protocol_from_json(Value& junit,data_unit& recv);
	void load_send_protocol_from_json(Value& junit,vector<u8>* pparent_id=NULL);
	data_unit& find_data_unit_by_key_unit(key_unit& ku,data_unit& data_host);
    data_unit& find_data_unit_by_id(u8* pin_buff,data_unit& data_host,u8*& pout_buff,string& key_name);
	base_filter_n* _task_entry;
	msg_handle _send_cmd;
	bool _be_running;
public:
	msg_host_n();
	~msg_host_n();
	void load_protocol_from_config(const char* config_path);
	void load_send_protocol_from_config(const char* config_path);
	bool attach_monitor(const char* object_name, msg_handle fn_obj);
	//void start_data_loop();
	//void execute_data_task();
	void set_task_entry(base_filter_n* pendtry)
	{
		_task_entry = pendtry;
	}
	void set_send_cmd(msg_handle cmd_send)
	{
		swap(_send_cmd, cmd_send);
	}
	void pick_valid_data(u8* pbuff,int len);
	void execute_data_handle_cmd();
	void send_data(const char* object_name,u8* pbuff,int len);
};

}