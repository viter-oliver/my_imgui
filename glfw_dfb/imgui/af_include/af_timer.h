#pragma once
#include "platform_def.h"
#include <functional>
#include <chrono>
#include <map>
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
			int _tvalue;//milliseconds
		};
		timer_unit _timer_list[max_timer_num];
		timer_unit_ex _timer_list_ex[max_timer_num];
              map<int, int> _active_tm_list,_active_tm_ex_list;
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
                   auto ict=_active_tm_list.find(timer_id);
                   return ict!=_active_tm_list.end();
              }
		bool deactive_time(int timer_id)
		{
			if (timer_id < max_timer_num&&_timer_list[timer_id]._handle)
			{
				_timer_list[timer_id]._num_cr = 0;
				_timer_list[timer_id]._freq_render = 0;
				_timer_list[timer_id]._rcnt = 0;
                            const auto& actm = _active_tm_list.find(timer_id);
				if (actm != _active_tm_list.end())
				{
					_active_tm_list.erase(actm);
				}
				return true;
			}
			return false;
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
		bool active_timer_ex(int timer_id,int tvalue)
		{
			if (timer_id < max_timer_num&&_timer_list_ex[timer_id]._handle)
			{
				_timer_list_ex[timer_id]._tp = steady_clock::now();
				_timer_list_ex[timer_id]._tvalue = tvalue;
                            _active_tm_ex_list[timer_id]=0;
				return true;
			}
			return false;
		}

		bool is_actived_timer_ex(int timer_id)
		{
			auto ict = _active_tm_ex_list.find(timer_id);
			return ict != _active_tm_ex_list.end();
		}
		bool deactive_time_ex(int timer_id)
		{
			if (timer_id < max_timer_num&&_timer_list_ex[timer_id]._handle)
			{
				_timer_list_ex[timer_id]._tvalue = 0;
				const auto& actm = _active_tm_ex_list.find(timer_id);
				if (actm != _active_tm_ex_list.end())
				{
					_active_tm_ex_list.erase(actm);
				}
				return true;
			}
			return false;
		}		

		void execute();
	};

}