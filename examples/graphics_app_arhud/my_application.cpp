#include "my_application.h"
#include<chrono>
#include "main.h"
#include <sstream>

#include <mutex>
#include "msg_host_n.h"
#include "car_info.h"
#include "media_info.h"
#include "mobileye_adas.h"
#include "navi_info.h"
#include "upgrade.h"

using namespace chrono;
using namespace auto_future_utilities;
msg_host_n g_msg_host;
steady_clock::time_point  lastTime;
int delta_limit = 30;
const float delta_pg = 0.25f;
float pg_value = 0.f;
int dis_id = 0;
#ifdef _SCOMMM
#include "scomm.h"
scomm s_scm;
#endif
my_application::my_application(int argc, char **argv)
	:application(argc,argv)
{
	_screen_width = 800;
	_screen_height = 534;
}
void reg_cmd_handle()
{
	register_car_cmd_handl();
	register_adas_cmd_handl();
	register_media_cmd_handl();
	register_navi_cmd_handl();
	register_upgrade_cmd_handl();
}

void my_application::resLoaded()
{
	lastTime = steady_clock::now();
	if (_arg_list.size() > 0)
	{
		g_msg_host.load_protocol_from_config(_arg_list[0].c_str());
		reg_cmd_handle();
	}
	auto currentTime = std::chrono::high_resolution_clock::now();
	int delta = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(currentTime - lastTime).count();
	printf("msg_host consume%d milli seconds\n", delta);
	lastTime = currentTime;
#ifdef RCF_COMM
	g_rcf_server.set_cmd_handle(std::bind(&msg_host_n::pick_valid_data, &g_msg_host, std::placeholders::_1, std::placeholders::_2));
	g_rcf_server.link();
	currentTime = std::chrono::high_resolution_clock::now();
	delta = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(currentTime - lastTime).count();
	printf("rcf server consume %d milli seconds\n",delta);
#endif

#ifdef _SCOMMM
	s_scm.set_msg_handle(std::bind(&msg_host_n::pick_valid_data, &g_msg_host, std::placeholders::_1, std::placeholders::_2));
	DWORD m_dwThreadId;
	CreateThread(NULL, 0, ThreadLoadApps, &s_scm, 0, &m_dwThreadId);
	currentTime = std::chrono::high_resolution_clock::now();
	delta = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(currentTime - lastTime).count();
	printf("comm consume %d milli seconds\n", delta);
	/*thread scmm_thd([this]{s_scm(); });
	scmm_thd.detach();*/
#endif
}

void my_application::onUpdate()
{
	g_msg_host.execute_data_handle_cmd();
	lane_moving_animation();
#ifdef SHOW_ANIMATION
	auto currentTime = steady_clock::now();
	int delta = chrono::duration_cast<chrono::duration<int, std::milli>>(currentTime - lastTime).count();
	if (delta < delta_limit)
	{
		return;
	}
	pg_value += delta_pg;
	if (pg_value > 1.f)
	{
		pg_value = 0.f;
	}
	dis_id++;
	dis_id %= 4;
	float rt_value = 1 - pg_value;
	set_property_aliase_value("fuel_consume", &pg_value);
	set_property_aliase_value("roate_speed", &rt_value);
	bool be_v = false;
	set_property_aliase_value("show_distance0", &be_v);
	set_property_aliase_value("show_distance1", &be_v);
	set_property_aliase_value("show_distance2", &be_v);
	set_property_aliase_value("show_distance3", &be_v);
	be_v = true;
	stringstream stm_tp;
	stm_tp << "show_distance" << dis_id;
	string pt_dis = stm_tp.str();
	set_property_aliase_value(pt_dis, &be_v);
#endif


}

my_application::~my_application()
{
}
#ifdef _SCOMMM

bool my_application::create_run()
{
	if (_arg_list.size()<1)
	{
		printf("at least you should provide a serial comm number!\n");
		return false;
	}
	int comm_id = atoi(_arg_list[1].c_str());
	int baudrate = 115200;
	if (_arg_list.size()>2)
	   baudrate=atoi(_arg_list[2].c_str());
	if (!s_scm.open(comm_id, baudrate))
	{
		return false;
	}
	
	return application::create_run();
}
#endif
AFGUI_APP(my_application)


