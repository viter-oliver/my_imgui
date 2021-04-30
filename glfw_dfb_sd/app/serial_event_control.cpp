#include "serial_event_control.h"
void serial_event_control::atach_msg_with_handle(int msg_id, function < void()> msg_hide_handle, function < void()> msg_show_handle)
{
	auto ps_eu_item=make_shared<event_unit>();
	ps_eu_item->msg_hide_handle=msg_hide_handle;
	ps_eu_item->msg_show_handle=msg_show_handle;
	_msg_handles[msg_id]=ps_eu_item;
}
bool serial_event_control::provoke_msg_show(int msg_id)
{
	bool be_rt=false;
	for (auto& msg_item:_msg_handles)
	{
		if(msg_item.first==msg_id)
		{
			be_rt=true;
			msg_item.second->msg_show_handle();
		}
		else
			msg_item.second->msg_hide_handle();
	}
	return be_rt;
}
void serial_event_control::all_msg_hide()
{
	for (auto& msg_item:_msg_handles)
	{
		msg_item.second->msg_hide_handle();
	}
}
