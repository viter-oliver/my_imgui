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
		}
	}
}