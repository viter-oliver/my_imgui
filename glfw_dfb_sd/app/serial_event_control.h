#ifndef serial_event_control_h
#define serial_event_control_h
#include <functional>
#include <memory>
#include <map>
using namespace std;

class serial_event_control
{
	struct event_unit
	{
		function<void()> msg_hide_handle;
		function<void()> msg_show_handle;
	};
    using ps_eu = shared_ptr<event_unit>;
	map<int,ps_eu> _msg_handles;
public:
	void atach_msg_with_handle(int msg_id,function<void()> msg_hide_handle,function<void()> msg_show_handle);		
	bool provoke_msg_show(int msg_id);
	void all_msg_hide();
};
#endif
