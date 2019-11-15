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
#include "imgui_impl_glfw_gl3.h"
#include "af_bind.h"
#include "af_timer.h"
using namespace chrono;
using namespace auto_future_utilities;
msg_host_n g_msg_host;
af_timer g_timer;
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
	_win_width = 800;
	_win_height = 267;
	_wposx = 0.f;
	_wposy = 133.5f;
}
bool be_show_border = false;
steady_clock::time_point  show_border_time;
void reg_cmd_handle()
{
	register_car_cmd_handl();
	register_adas_cmd_handl();
	register_media_cmd_handl();
	register_navi_cmd_handl();
	register_upgrade_cmd_handl();
	
}
ft_material_3d* planes3d=nullptr;
float lane_camera_z = 134.0;
float lane_camera_u = 0.5f;
void my_application::resLoaded()
{
	lastTime = steady_clock::now();
	if (_arg_list.size() > 0)
	{
		g_msg_host.load_protocol_from_config(_arg_list[0].c_str());
		planes3d = (ft_material_3d*)get_aliase_ui_control("cur_lane");
		assert(planes3d != nullptr);
		reg_cmd_handle();

		g_msg_host.attach_monitor("steering keys", [&](u8*pbuff, int len){
			enum
			{
				en_powr_on,
				en_horizontal_cal_up,//1
				en_horizontal_cal_dn,//2
				en_height_up,//3
				en_height_dn,//4
				en_brightness_up,//5
				en_brightness_dn,//6
				en_buzzer_on,
				en_buzzer_off,
			};
			enum
			{
				en_key_release,
				en_key_short_press,
				en_key_long_press,
				en_key_hold,
			};
			u8 key_value = *pbuff++;
			u8 key_status = *pbuff;
			static bool be_adj_fov = false;
			printf("key_value=%d,key_status=%d\n",key_value,key_status);
			if (key_status == en_key_release)
			{
				static float screen_y = 133.5;
				static float screeen_angle = 0.0;
				const float angl_unit = 0.001f;
				switch (key_value)
				{
				case en_powr_on:
					break;
				case en_horizontal_cal_up:
				{
					screeen_angle += angl_unit;
					set_rotate_angle(screeen_angle);
				}
				break;
				case en_horizontal_cal_dn:
				{
					screeen_angle -= angl_unit;
					set_rotate_angle(screeen_angle);
				}
				break;
				case en_height_up:
				{
					screen_y += 1.f;
					//set_property_aliase_value("screeny", &screen_y);
					set_windows_pos(0, screen_y);
					show_border_time = steady_clock::now();
					be_show_border = true;
					set_property_aliase_value("show_bg_border", &be_show_border);
				}
				break;
				case en_height_dn:
				{
					screen_y -= 1.f;
					//set_property_aliase_value("screeny", &screen_y);
					set_windows_pos(0, screen_y);
					show_border_time = steady_clock::now();
					be_show_border = true;
					set_property_aliase_value("show_bg_border", &be_show_border);

				}
				break;
				case en_brightness_up:
					if (1)//(be_adj_fov)
					{
						lane_camera_z += lane_camera_u;
						planes3d->set_cam_posz(lane_camera_z);
					}
					break;
				case en_brightness_dn:
					if (1)//(be_adj_fov)
					{
						lane_camera_z -= lane_camera_u;
						planes3d->set_cam_posz(lane_camera_z);
					}
					break;
				case en_buzzer_on:
					break;
				case en_buzzer_off:
					break;

				}
			}
			else
			if (key_status == en_key_long_press)
			{
				if (key_value == en_brightness_dn)
					be_adj_fov = !be_adj_fov;
			}
		});
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
	if (s_scm.is_open())
	{
		s_scm.set_msg_handle(std::bind(&msg_host_n::pick_valid_data, &g_msg_host, std::placeholders::_1, std::placeholders::_2));
		DWORD m_dwThreadId;
		CreateThread(NULL, 0, ThreadLoadApps, &s_scm, 0, &m_dwThreadId);
		g_msg_host.set_send_cmd([&](u8* pbuff, int len){
			u8* plink = new u8[len + 7];
			plink[0] = MCU_SYNC2;
			plink[1] = MCU_SYNC1;
			struct GNU_DEF signal_def
			{
				u8 reserve : 5;
				u8 need_ac : 1;
				u8 frame_tp : 1;
				u8 frame_dir : 1;
			};
			signal_def* psg = (signal_def*)(plink + 2);
			psg->reserve = 0;
			psg->need_ac = 1;
			psg->frame_tp = 1;
			psg->frame_dir = 0;
			static u8 frame_idx = 0;
			plink[2 + en_pos_index] = frame_idx;
			frame_idx++;
			u16 frame_len = len + 1;
			u8* pfm_ln = (u8*)&frame_len;
			plink[2 + en_pos_length_h] = *pfm_ln++;
			plink[2 + en_pos_length_l] = *pfm_ln;
			u8 chk_sm = plink[0] ^ plink[1] ^ plink[2] ^ plink[3] ^ plink[4] ^ plink[5];
			for (int ix = 0; ix < len; ix++)
			{
				plink[2 + en_pos_cmd_head + ix] = pbuff[ix];
				chk_sm = chk_sm^pbuff[ix];
			}
			plink[2 + en_pos_cmd_head + len] = chk_sm;
			s_scm.send_data(plink, len + 7);
			delete[] plink;
		});
	}

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
	g_timer.execute();
	lane_moving_animation();
	if(be_show_border)
	{
		auto currentTime = steady_clock::now();
		int delta = chrono::duration_cast<chrono::duration<int,std::milli>>(currentTime - show_border_time).count();
		if(delta>5000)
		{
			be_show_border=false;
			set_property_aliase_value("show_bg_border",&be_show_border);
		}
	}
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
		printf("fail to open comm:%d\n", comm_id);
		//return false;
	}
	
	return application::create_run();
}
#endif
AFGUI_APP(my_application)


