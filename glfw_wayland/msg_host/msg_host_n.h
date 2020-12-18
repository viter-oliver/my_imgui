#pragma once
#include "utilitis_base_def.h"
#include <fstream>
#include<map>
#include<mutex>
#include <condition_variable>
#include <thread>
#include <json/json.h>
#include <memory>
#include <atomic>
#include <chrono>
#include <semaphore.h>
//#include "af_message_queue.h"
//#include<semaphore.h>

namespace auto_future_utilities
{
using namespace std;
using namespace Json;
using namespace chrono;

typedef vector<u8> vdata_list;
typedef function<u8(u8*,string& )> f_get_cmd;
typedef function<bool(u8*,int)> f_batching_handle;
using vbatching_handle= vector<f_batching_handle>;
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
       vbatching_handle _batching_handle_list;
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
	bool _be_replaying={false};
	bool _be_recording={false};
	ofstream _frcording;
	ifstream _fin_rp;
	string _recording_file_name;
	steady_clock::time_point  _pickdata_tm;
	sem_t m_sem;
	atomic<bool> _waitting_replay{false};
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
        int reg_batching_cmd( int id, f_batching_handle fbat )
        {
            auto bat_cnt = _batching_handle_list.size();
            if( id < bat_cnt )
            {
               swap( _batching_handle_list[ id ], fbat );
               return id;
            }
            else
            {
               _batching_handle_list.emplace_back( fbat );
               return bat_cnt;
            }
        }
	//void start_data_loop();
	//void execute_data_task();
	
	void begin_recording(string recording_name)
    {
		_be_recording=true;
		_recording_file_name=recording_name;
	}
	void end_recooding()
	{
		_be_recording=false;
		if(_frcording.is_open())
		{
			_frcording.close();
		}
	}
	void replay_file(string recording_file);
	void end_replay()
	{
		_be_recording=false;
	}
	bool is_waitting_replay()
	{
		return _waitting_replay;
	}
	void restart_play()
	{
		sem_post(&m_sem);
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