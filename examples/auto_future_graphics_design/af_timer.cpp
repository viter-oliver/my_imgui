#include "af_timer.h"

namespace auto_future
{

	af_timer::af_timer()
	{
	}

	af_timer::~af_timer()
	{
	}

	void af_timer::execute()
	{
		for (auto& ictm : _active_tm_list)
		{
			auto& ix = ictm.first;
	
			_timer_list[ix]._num_cr--;
			_timer_list[ix]._rcnt--;
			if (_timer_list[ix]._rcnt == 0)
			{
				if (_timer_list[ix]._handle)
				{
					_timer_list[ix]._handle(_timer_list[ix]._num_cr);
				}
				_timer_list[ix]._rcnt = _timer_list[ix]._freq_render;
			}
		}
		for (auto& ictm: _active_tm_ex_list)
		{
			auto& ix=ictm.first;
			auto& ivalue=ictm.second;
			
			if (_timer_list_ex[ix]._handle&&_timer_list_ex[ix]._tvalue)
			{
				auto currentTime = steady_clock::now();
				int delta = chrono::duration_cast<chrono::duration<int, std::milli>>(currentTime - _timer_list_ex[ix]._tp).count();
				if (delta >= _timer_list_ex[ix]._tvalue)
				{
                         _timer_list_ex[ ix ]._tp = currentTime;
					_timer_list_ex[ix]._handle(ivalue);
					ivalue++;
				}
			}	
		}
	}
}