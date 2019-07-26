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
		for (int ix = 0; ix < max_timer_num; ix++)
		{
			if (_timer_list[ix]._num_cr&&_timer_list[ix]._handle)
			{
				_timer_list[ix]._num_cr--;
				_timer_list[ix]._rcnt--;
				if (_timer_list[ix]._rcnt == 0)
				{
					_timer_list[ix]._handle(_timer_list[ix]._num_cr);
					_timer_list[ix]._rcnt = _timer_list[ix]._freq_render;
				}
			}
			if (_timer_list_ex[ix]._handle&&_timer_list_ex[ix]._tvalue)
			{
				auto currentTime = steady_clock::now();
				int delta = chrono::duration_cast<chrono::duration<int, std::milli>>(currentTime - _timer_list_ex[ix]._tp).count();
				if (delta >= _timer_list_ex[ix]._tvalue)
				{
					_timer_list_ex[ix]._handle(delta);
					_timer_list_ex[ix]._tvalue = 0;
				}
			}
		}
	}
}