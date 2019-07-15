#pragma once
#include "platform_def.h"
#include <functional>
using namespace std;
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
		timer_unit _timer_list[max_timer_num];
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
		bool active_timer(int timer_id, int num_cr, int fc)
		{
			if (timer_id<max_timer_num&&_timer_list[timer_id]._handle)
			{
				_timer_list[timer_id]._num_cr = num_cr;
				_timer_list[timer_id]._freq_render = fc;
				_timer_list[timer_id]._rcnt = fc;
				return true;
			}
			return false;
		}
		bool deactive_time(int timer_id)
		{
			if (timer_id < max_timer_num&&_timer_list[timer_id]._handle)
			{
				_timer_list[timer_id]._num_cr = 0;
				_timer_list[timer_id]._freq_render = 0;
				_timer_list[timer_id]._rcnt = 0;
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
		void execute();
	};

}