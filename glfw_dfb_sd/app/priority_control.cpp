#include <assert.h>
#include "priority_control.h"
namespace auto_future
{

bool signal_priority_control::try_signal_on(int mid,bool be_visible,int sub_id)
{
	auto sig_cnt=_psignal_control_unit_list.size();
	assert(mid<sig_cnt&&"invalid mid");
	if(be_visible)
	{
		if (mid>0)
		{
			for (int ix = mid - 1; ix >= 0; ix--)
			{
				if (_psignal_control_unit_list[ix]->_visible)
				{
				    //printf("%d is on,so %d is preempted\n",ix,mid);
					_psignal_control_unit_list[mid]->be_preempted = true;
					_psignal_control_unit_list[mid]->_visible = false;
					_psignal_control_unit_list[mid]->queue(sub_id);//queue
					return true;
				}
			}			
		}
		_psignal_control_unit_list[mid]->be_preempted = false;
		_psignal_control_unit_list[mid]->_visible = true;
		_psignal_control_unit_list[mid]->handle(true,sub_id);
		//printf("sig on mid=%d sid=%d\n",mid,cur_sub_id);
		for(int ix=mid+1;ix<sig_cnt;ix++)
		{
			if (_psignal_control_unit_list[ix]->_visible)
			{
				_psignal_control_unit_list[ix]->_visible = false;
				_psignal_control_unit_list[ix]->be_preempted = true;
				
				_psignal_control_unit_list[ix]->handle(false,-1);//hide top element
				return true;
			}
		}
		return true;
	}
	else
	{
		auto finish=_psignal_control_unit_list[mid]->handle(false,sub_id);
		if(finish)
		{
			_psignal_control_unit_list[mid]->_visible = false;
			_psignal_control_unit_list[mid]->be_preempted=false;
			for (int ix = 0; ix < mid; ix++)
			{
				if (_psignal_control_unit_list[ix]->_visible)
				{
					//printf("%d is already on,sid:%d quit restore lowwer id \n",ix,sid);
					return true;
				}
			}
			for (int ix = mid + 1; ix < sig_cnt;++ix)
			{
				if (_psignal_control_unit_list[ix]->_visible)
				{
					return true;
				}
				if (_psignal_control_unit_list[ix]->be_preempted)
				{
					_psignal_control_unit_list[ix]->be_preempted = false;
					_psignal_control_unit_list[ix]->_visible = true;
					_psignal_control_unit_list[ix]->handle(true,-1);//show top element
					//printf("%d:%d is restored by %d\n",ix,tar_queue[0],mid);
					return true;
				}
			}
		}
        return true;
	}
}

bool signal_priority_control::flash_signal(int mid,bool be_visible,int sub_id)
{
	auto sig_cnt=_psignal_control_unit_list.size();
	assert(mid<sig_cnt&&"invalid mid for flash signal");
	if(_psignal_control_unit_list[mid]->_visible)
	{
		_psignal_control_unit_list[mid]->flash(be_visible,sub_id);
		return true;
	}
	return false;
}
en_sg_statatus signal_priority_control::signal_status(int mid,int sub_id)
{
	auto sig_cnt=_psignal_control_unit_list.size();
	assert(mid<sig_cnt&&"invalid mid for signal_status");
	if(_psignal_control_unit_list[mid]->_visible)
	{
		return _psignal_control_unit_list[mid]->sig_status(sub_id);
	}
	return en_sg_missing;
}
bool signal_unit_deque::handle(bool on, int sid)
{
	if (sid<0)//-1
	{
		assert(!_deque.empty()&&"deque shouldn't be empty!");
		auto fitem=_deque.front();
 		if(on)
 		{
			_sub_sig_list[fitem]._handle_sub_sig(true);
		}
		else
		{
			_sub_sig_list[fitem]._handle_sub_sig(false);
			_sub_sig_list[sid]._on=false;
			_deque.pop_front();
			if(_visible&&!_deque.empty())
			{
				auto nfitem=_deque.front();
				_sub_sig_list[nfitem]._handle_sub_sig(true);
			}
		}
	}
    else if(sid<_sub_sig_list.size()&&_sub_sig_list[sid]._handle_sub_sig)
    {
    	if(on)
    	{
			if(!_sub_sig_list[sid]._on)
			{
				_sub_sig_list[sid]._on=true;
				_deque.push_back(sid);
				auto fitem=_deque.front();
				if(fitem==sid) _sub_sig_list[sid]._handle_sub_sig(true);
			}
		}
		else
		{
			if (_sub_sig_list[sid]._on)
			{
				
				_sub_sig_list[sid]._on=false;
				_sub_sig_list[sid]._handle_sub_sig(false);
				
				auto fitem=_deque.front();
				for (auto idq	 = _deque.begin(); idq	!=_deque.end(); 	)
				{
					if(*idq==sid)//find the position of sid at deque
					{
						_deque.erase(idq);
						if(!_deque.empty())
						{
							if (_visible&& fitem==sid)
							{
								auto nfitem=_deque.front();
								_sub_sig_list[nfitem]._handle_sub_sig(true);
							}							
						}
						else
						{
							return true;
						}
						return false;
					}
					else
						idq++;
				}
			}
		}
    }
	return false;
}
en_sg_statatus signal_unit_deque::sig_status(int sid)
{
	if (sid<_sub_sig_list.size())
	{
	
		if(!_sub_sig_list[sid]._on)
		{
			return en_sg_missing;
		}
		else
		{
			auto iftm=_deque.front();
			if(iftm==sid)
			{
				return en_sg_on;
			}
			else
			{
				return en_sg_queuing;
			}
		}
	}
	return en_sg_missing;
}
void signal_unit_deque::queue(int sid)
{
	if (sid<_sub_sig_list.size()&&!_sub_sig_list[sid]._on)
	{
		_sub_sig_list[sid]._on=true;
		_deque.push_back(sid);
	}
}
bool signal_unit_deque::flash(bool visible,int sid)
{
	if(_visible&& sid<_sub_sig_list.size()&&_sub_sig_list[sid]._handle_sub_sig)
    {
		if(_sub_sig_list[sid]._on)
		{
			 _sub_sig_list[sid]._handle_sub_sig(visible);
			 return true;
		}
    }
	return false;
}
bool signal_unit_coexist::flash(bool visible,int sid)
{
	if(_visible&& sid<_sub_sig_list.size()&&_sub_sig_list[sid]._handle_sub_sig)
    {
		if(_sub_sig_list[sid]._on)
		{
			 _sub_sig_list[sid]._handle_sub_sig(visible);
			 return true;
		}
    }
	return false;
}

bool signal_unit_coexist::handle(bool on, int sid)
{
    if(sid<_sub_sig_list.size()&&_sub_sig_list[sid]._handle_sub_sig)
	{
		_sub_sig_list[sid]._on=on;
		_sub_sig_list[sid]._handle_sub_sig(on);
	}
	return false;
}
en_sg_statatus signal_unit_coexist::sig_status(int sid)
{
	if (sid<_sub_sig_list.size())
	{
	
		if(!_sub_sig_list[sid]._on)
		{
			return en_sg_missing;
		}
		else
		{
			return en_sg_on;
		}
	}
	return en_sg_missing;
}

}

