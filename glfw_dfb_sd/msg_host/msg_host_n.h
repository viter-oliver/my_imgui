#pragma once
#include "utilitis_base_def.h"
#include<map>
#include<mutex>
#include <condition_variable>
#include <thread>
#include <json/json.h>
#include <memory>
#include <atomic>
//#include "af_message_queue.h"
//#include<semaphore.h>

namespace auto_future_utilities
{
using namespace std;
using namespace Json;
typedef vector<u8> vdata_list;
typedef function<u8(u8*,string& )> f_get_cmd;

//typedef unsigned char u8;
class AFG_EXPORT msg_host_n
{

	struct sub_unit
	{
		string _name;
		u8 _seg_len{0};
		msg_handle _handle{nullptr};
	};
	struct data_unit
	{
		msg_handle _handle{nullptr};
		vector<sub_unit> _vsub_unit;		
	};
	using ps_du = shared_ptr<data_unit>;
	using mprotocol = map<string, ps_du >;
	using mpid_name = map<string, string>;
	mprotocol _recieve_protocol;
	mpid_name _rec_id_name;
	mprotocol _send_protocol;
	mpid_name _sd_id_name;
#define static_buff_len  256
	struct buff_unit
	{
		u32 _data_len{ 0 };
		u8 _static_buff[static_buff_len];
		u8* _pdynamic_buff = nullptr;
	};
#define queque_len 512
	atomic<unsigned int > _rear_id{ 0 }, _front_id{ 0 };
	buff_unit _cmd_queue[queque_len];
	//data_unit _root_protocol_receive_data_map;
	//map<string, vector<u8>> _protocol_send_data_map;

	msg_handle _send_cmd;
       f_get_cmd _get_cmd;
	bool _be_running;
public:
	msg_host_n();
	~msg_host_n();
	void load_protocol_from_config(const char* config_path);
	void load_send_protocol_from_config(const char* config_path);
    	void set_f_get_cmd(f_get_cmd gcmd)
	{
		_get_cmd = gcmd;
	}
	bool attach_monitor(const char* object_name, msg_handle fn_obj);
	bool attach_monitor(const char* object_name, const char* seg_name, msg_handle fn_obj);
	bool attach_monitor(const char* object_name, u8 seg_id, msg_handle fn_obj);

	//void start_data_loop();
	//void execute_data_task();
	
	void set_send_cmd(msg_handle cmd_send)
	{
		swap(_send_cmd, cmd_send);
	}
	void pick_valid_data(u8* pbuff,int len);
	void execute_data_handle_cmd();
	void send_data(const char* object_name,u8* pbuff,int len);
};

}
