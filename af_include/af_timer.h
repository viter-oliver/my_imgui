#pragma once
#include "platform_def.h"
#include <functional>
#include <chrono>
#include <map>
#include <vector>
using namespace std;
using namespace chrono;
namespace auto_future
{
	const int max_timer_num = 0x100;
	class AFG_EXPORT af_timer
	{
		struct timer_unit
		{
			int _num_cr{ 0 };//number of circle of rendering;
			int _freq_render{ 1 };//frequency of handling
			int _rcnt{ 1 };
			function<void(int)> _handle{ nullptr };
		};
		struct timer_unit_ex 
		{
			function<void(int)> _handle{ nullptr };
			steady_clock::time_point  _tp;
 			steady_clock::time_point  _start;
			int _tvalue;//milliseconds
		};
		struct timer_del
		{
		   steady_clock::time_point  _start;
		   int _tvalue;//milliseconds
		};
		timer_unit _timer_list[max_timer_num];
		timer_unit_ex _timer_list_ex[max_timer_num];
		map<int, int> _active_tm_list,_active_tm_ex_list;
          //vector<int> _will_erase_id_list;
          map<int, timer_del> _will_erase_id_list,_will_erase_id_list0;
	public:
		af_timer();
		~af_timer();
		int register_timer( function<void(int)> hdl,int num_cr=0,int fr=0)
		{
			for (int ix = 0; ix < max_timer_num; ix++)
			{
				if (!_timer_list[ix]._handle)
				{
					_timer_list[ix]._num_cr = num_cr;
					_timer_list[ix]._freq_render = fr;
					_timer_list[ix]._rcnt = fr;
					swap(_timer_list[ix]._handle, hdl);
					return ix;
				}
			}
			return -1;
		}


		/**
		*@brief active a timer
		*@param timer_id a id which is returned by method register_timer
		*@param num_cr the number of ticking of the timer
		*@param fc how often the handle will be called
		*@return result
		*  --true success
		*  --false failure
		*/
		bool active_timer(int timer_id, int num_cr, int fc)
		{
			if (timer_id<max_timer_num&&_timer_list[timer_id]._handle)
			{
				_timer_list[timer_id]._num_cr = num_cr;
				_timer_list[timer_id]._freq_render = fc;
				_timer_list[timer_id]._rcnt = fc;
				_active_tm_list[timer_id] = 0;
				return true;
			}
			return false;
		}
		bool is_actived_timer(int timer_id)
		{
			auto ict = _active_tm_list.find(timer_id);
			return ict != _active_tm_list.end();
		}
          void deactive_time( int timer_id,int delay_tm = 0 )
		{
               const auto& iav = _active_tm_list.find( timer_id );
               if( iav == _active_tm_list.end() )
               {
                    return;
               }
               const auto& iwid = _will_erase_id_list0.find( timer_id );
               if( iwid == _will_erase_id_list0.end() )
               {
                    timer_del tdl = { steady_clock::now(), delay_tm };
                    _will_erase_id_list0[ timer_id ] = tdl;
               }
		}	
		bool unregister_timer(int timer_id)
		{
			if (timer_id < max_timer_num)
			{
				_timer_list[timer_id]._num_cr = 0;
				_timer_list[timer_id]._freq_render = 0;
				_timer_list[timer_id]._rcnt = 0;
				_timer_list[timer_id]._handle = nullptr;
				return true;
			}
			return false;
		}

		int register_timer_ex(function<void(int)> hdl)
		{
			for (int ix = 0; ix < max_timer_num; ix++)
			{
				if (!_timer_list_ex[ix]._handle)
				{
					swap(_timer_list_ex[ix]._handle, hdl);
					return ix;
				}
			}
			return -1;
		}
		int get_tvalue_ex( int timer_id )
		{
		   if( timer_id < max_timer_num&&_timer_list_ex[ timer_id ]._handle )
		   {
		        return _timer_list_ex[ timer_id ]._tvalue;
		   }
		   return 0;
		}
		bool active_timer_ex(int timer_id,int tvalue)
		{
			if (timer_id < max_timer_num&&_timer_list_ex[timer_id]._handle)
			{
				_timer_list_ex[timer_id]._tp = steady_clock::now();
                    _timer_list_ex[timer_id]._start=_timer_list_ex[timer_id]._tp ;
				_timer_list_ex[timer_id]._tvalue = tvalue;
				_active_tm_ex_list[timer_id]=0;
				return true;
			}
			return false;
		}
          bool active_timer_with_attachment( int timer_id, int tvalue, int timer_id_attached )
          {
               const auto& iav = _active_tm_ex_list.find( timer_id_attached );
               if( iav == _active_tm_ex_list.end() )
               {
                    _timer_list_ex[ timer_id ]._tp = steady_clock::now();
                    _timer_list_ex[ timer_id ]._start = _timer_list_ex[ timer_id ]._tp;
                    _timer_list_ex[ timer_id ]._tvalue = tvalue;
                    _active_tm_ex_list[ timer_id ] = 0;
                    return false;
               }
               else
               {
                    auto& tm_item = _timer_list_ex[ timer_id_attached ];
                    _timer_list_ex[ timer_id ]._tp = tm_item._tp;
                    _timer_list_ex[ timer_id ]._start = tm_item._start;
                    _timer_list_ex[ timer_id ]._tvalue = tm_item._tvalue;
                    _active_tm_ex_list[ timer_id ] = _active_tm_ex_list[ timer_id_attached ];
               }
               return true;
          }
		int get_timer_duration_for(int timer_id)
		{
		    const auto& ict=_active_tm_ex_list.find(timer_id);
		    if(ict==_active_tm_ex_list.end())
		    {
		        return 0;
		    }
		    auto cur= steady_clock::now();
		    auto istart=_timer_list_ex[timer_id]._start;
		    int duration = chrono::duration_cast<chrono::duration<int, std::milli>>(cur - istart).count();
		    return duration;
		}
		bool is_actived_timer_ex(int timer_id)
		{
			auto ict = _active_tm_ex_list.find(timer_id);
			return ict != _active_tm_ex_list.end();
		}
          void deactive_time_ex( int timer_id,int delay_tm = 0 )
		{
               const auto& iav = _active_tm_ex_list.find( timer_id );
               if( iav == _active_tm_ex_list.end())
               {
                    return;
               }
               const auto& iwid = _will_erase_id_list.find( timer_id );
               if( iwid == _will_erase_id_list.end() )
               {
                    timer_del tdl = { steady_clock::now(), delay_tm };
                    _will_erase_id_list[ timer_id ] = tdl;
               }
			
		}		
          bool is_pending( int timer_id )
          {
               const auto& iwid = _will_erase_id_list.find( timer_id );
               return iwid != _will_erase_id_list.end();
          }
		void execute();
	};

}