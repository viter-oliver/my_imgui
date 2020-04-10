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
          auto currentTime = steady_clock::now();
          for( auto eid = _will_erase_id_list.begin(); eid != _will_erase_id_list.end(); )
          {
               auto& tid = eid->first;
               auto& tdly = eid->second;
              
               int delta = chrono::duration_cast<chrono::duration<int, std::milli>>( currentTime - tdly._start ).count();
               if( delta > tdly._tvalue )
               {
                    const auto& actm = _active_tm_ex_list.find( tid );
                    if( actm != _active_tm_ex_list.end() )
                    {
                         _active_tm_ex_list.erase( actm );
                         _timer_list_ex[ tid ]._tvalue = 0;
                    }
                    eid = _will_erase_id_list.erase( eid );
              }
               else
               {
                    eid++;
               }
              
               /**
               const auto& iactm = _active_tm_list.find( eid );
               if( iactm != _active_tm_list.end() )
               {
                    _active_tm_list.erase( actm );
                    _timer_list_ex[ eid ]._tvalue = 0;
               }*/
          }
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