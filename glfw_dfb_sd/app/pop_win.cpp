

#pragma execution_character_set("utf-8")

#include "pop_win.h"
#include "msg_host_n.h"
#include "af_bind.h"
#include "af_primitive_object.h"
#include "af_material.h"
#include "af_state_manager.h"
//#include "enum_txt_name0.h"
#include "ft_base.h"
#include "afg.h"
#include<chrono>
#include "af_timer.h"
#include <thread>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include <mutex>
#include <map>
#include <vector>
#include <math.h>
#include <glfw/glfw3.h>
#include "PageManager.h"
#include "AppDef.h"

#include "enum_txt_res_name.h"

using namespace chrono;
using namespace auto_future_utilities;
using namespace auto_future;
using namespace std;
extern msg_host_n g_msg_host;

extern af_timer g_timer;
ft_image_file* g_image_show;

ft_image_play *pturn_left, *pturn_right;

ft_textblock *ptext_speed, *ptext_tach, *ptext_tripa, *ptext_odo, *ptext_fps;
//页面切换节点
ft_base *pbase_music;


float c2u = 0.0000005f;
int tmid_cave_lnae = 0, tmid_lane_ldw = 0, tmid_tsr = 0, tmid_speed = 0;
int tmid_mspeed = 0, tmid_mtach = 0, tmid_fuel = 0, tmid_cooltemp = 0, tmid_odo = 0;
int tmid_fps = 0, count_fps = 0;
enum color_md
{
	en_clr_day,
	en_clr_night,
	en_clr_snow,
	en_clr_cnt
};
u8 theme_id;

void print_buff(u8* pbuff, int len)
{
	static char num_tb[0x10] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};
	struct buff_data
	{
		u8 data_l : 4;
		u8 data_h : 4;
	};
	string pt_buff;
	while (len > 0)
	{
		buff_data* pdata = (buff_data*)pbuff++;
		len--;
		pt_buff += " x";
		pt_buff += num_tb[pdata->data_h];
		pt_buff += num_tb[pdata->data_l];
	}
	printf("%s\n", pt_buff.c_str());
}


extern char str_show[MAX_CONTENT_LEN];

//const char* fifo_path="/tmp/fifo_hq";
bool g_debug_stm = false;
string show_image_path;
//int fd_fifo=0;
//bool recieve_cmd=false;
bool update_cmd = false;



#include "debug_var_set.h"
#include "control_common_def.h"

enum em_fault_list{
	em_fault_red,
	em_fault_yellow,
	em_fault_list_max
};

enum em_pop_bg_list{
	em_pop_bg_red,
	em_pop_bg_yellow,
	em_pop_bg_white,
	em_pop_bg_max
};

extern int tmid_back_tc;

int item_fault_list[em_fault_list_max] = { en_gid_0icon_ic_warning_red_png, en_gid_0icon_ic_warning_yellow_png };

int pop_bg_list[em_pop_bg_max] = { en_gid_0img_alert_frame_red_png, en_gid_0img_alert_frame_yellow_png, en_gid_0img_alert_frame_white_png };

map<string, u8> signal_int;

bool sig_show = false;


std::string  PageIdToString(EmPageID id)
{
	switch (id)
	{
	case main_menu:
		return std::string("menu_base");//aliase name
		break;

	case setting_menu_1st:
		return std::string("setting_1st_menu");
		break; 
	case media:
		return std::string("media_no_signal");
		break;
	case fault_list:
		return std::string("fault_base");
		break;
	case TC_in_main_menu:
		return std::string("tc");
		break;
	case setting_2nd_navi:
		return std::string("setting_2nd_map");
		break;
	case setting_2nd_reset_TC_data:
		return std::string("setting_2nd_trip");
		break;
	case setting_2nd_reset_factory:
		return std::string("setting_2nd_reset");
		break;
	case media_2nd_usb_music:
	case media_2nd_bluetooth_music:
	case media_2nd_locate_music:
		return std::string("media_menu_music");
		break;
	case media_2nd_radio_fm:
	case media_2nd_radio_am:
		return std::string("media_menu_fm");
		break;
	case setting_3rd_reset_avg:
		return std::string("setting_3rd_avg");
		break;
	case setting_3rd_reset_tripa:
		return std::string("setting_3rd_tripa");
		break;

	case setting_3rd_easy_map:
		return std::string("");
		break;
	case setting_3rd_big_map:
		return std::string("");
		break;
	case page_adas:
		return std::string("");
		break;
	case TC_menu:
		return std::string("tc");
		break;
	case  id_none:
		return std::string("page_none");
		break;
	default: return std::string(""); break;
	}
}


void register_my_page(){}

void imp_register_page(EmPageID id)
{
	ft_base *ptr = nullptr;
	std::string str = PageIdToString(id);

	ptr = (ft_base *)get_aliase_ui_control(str=std::string("show_") + str);

	if (ptr == nullptr)
	{
		printf("can not get ui control id=%d\n", id);
		return;
	}
	//PageManager::getInstance()->mmapFtBasePageID[id] = ptr;
	PageManager::getInstance()->mmapFtBasePageID.insert(std::pair<EmPageID, ft_base*>(id, ptr));
	PageManager::getInstance()->mmapPageIDStr.insert(std::pair<EmPageID, std::string>(id, str));

}


template<typename type_name_pre, typename ... type_name>
void register_my_page(type_name_pre name_pre, type_name ...  name)
{
	imp_register_page(name_pre);
	register_my_page(name ...);
}

void register_main_view_page()
{
	PageManager::getInstance()->init();

	register_my_page(main_menu, setting_menu_1st, media, fault_list, TC_in_main_menu,
	setting_2nd_navi, setting_2nd_reset_TC_data, setting_2nd_reset_factory, media_2nd_usb_music,
		media_2nd_radio_fm, setting_3rd_reset_avg, setting_3rd_reset_tripa, setting_3rd_easy_map,
		setting_3rd_big_map, page_adas, TC_menu,id_none);
	PageManager::getInstance()->pageSwitchTo(TC_menu);

}



#include<chrono>
#include <functional>
#include <map>

using namespace std::chrono;

#define  THIRTY_MIN  1800000  //30min 


std::vector<Em_PoPId> vec_list; //存储需要30分钟后触发的弹窗

std::map<Em_PoPId, int> map_rest_list;

std::map<Em_PoPId, bool> map_pop;        //显示弹窗 
std::map<Em_PoPId, bool> map_pop_data;   //存储数据


//故障列表

std::map<Em_PoPId, std::string > map_fault_list;
std::vector<Em_PoPId> vec_fault_list;

struct StructPopWin
{
	Em_PoPId id;
	Em_priority pri;
	bool bInterupt;     //允许被打断 
	std::function<void(bool)> handle;
};


/* 宏函数 */

MyPopWin my_popWin;

#ifdef WIN32			
#define  DEBUG_POP_TIMER()   g_timer.deactive_time_ex(tmid_back_tc)
#else
#define  DEBUG_POP_TIMER()
#endif


#define  CAR_DOOR_OPEN(a,b)    restore_trans_value(#a, !b);\
		play_tran(#a,!b,b);


#define  SHOW_POP_BG(a)   \
				ft_image * p = (ft_image *)get_aliase_ui_control("control_pop_bg");\
				if(!(a>=em_pop_bg_max)) \
																				{\
						p->set_texture_id({0,pop_bg_list[a]});\
						p->set_visible(true);\
																} 				



#define  SHOW_NO_POP_BG()      ft_image * p = (ft_image *)get_aliase_ui_control("control_pop_bg"); \
				p->set_visible(false); \


StructPopWin  pop_arr[em_max] = {
	{ em_none, Em_priority_A, false, [&](bool visable){} },
	{ em_key_ok, Em_priority_A, false, [&](bool visable){} },
	{ em_car_door_left_front_open, Em_priority_A, false,
	[&](bool visable){
		if (visable)
		{
			set_property_aliase_value("show_car_door", &visable);
			CAR_DOOR_OPEN(left_front_door_open, 1);
			SHOW_NO_POP_BG();
			DEBUG_POP_TIMER();
			PageManager::getInstance()->hideWithShowPage(true);
		}
		else
		{
			CAR_DOOR_OPEN(left_front_door_open, 0);
		}

	} },

	{ em_car_door_left_back_open, Em_priority_A, false,
	[&](bool visable){
		if (visable)
		{
			set_property_aliase_value("show_car_door", &visable);
			CAR_DOOR_OPEN(left_back_door_open, 1);
			SHOW_NO_POP_BG();
			DEBUG_POP_TIMER();
			PageManager::getInstance()->hideWithShowPage(true);
		}
		else
		{
			CAR_DOOR_OPEN(left_back_door_open, 0);
			//restore_trans_value("left_back_door_open", 1);
			//play_tran("left_back_door_open", 1, 0);
		}

	} },

	{ em_car_door_right_front_open, Em_priority_A, false,
	[&](bool visable){
		if (visable)
		{
			set_property_aliase_value("show_car_door", &visable);
			CAR_DOOR_OPEN(right_front_door_open, 1);
			SHOW_NO_POP_BG();
			DEBUG_POP_TIMER();
			PageManager::getInstance()->hideWithShowPage(true);

		}
		else
		{
			CAR_DOOR_OPEN(right_front_door_open, 0);
		}
	} },

	{ em_car_door_right_back_open, Em_priority_A, false,
	[&](bool visable){
		if (visable)
		{
			set_property_aliase_value("show_car_door", &visable);
			CAR_DOOR_OPEN(right_back_door_open, 1);
			SHOW_NO_POP_BG();
			DEBUG_POP_TIMER();
			PageManager::getInstance()->hideWithShowPage(true);

		}
		else
		{
			CAR_DOOR_OPEN(right_back_door_open, 0);
		}

	} },
		//车前盖
	{ em_car_door_engine_open, Em_priority_A, false,
	[&](bool visable){
		if (visable)
		{
			set_property_aliase_value("show_car_door", &visable);
			CAR_DOOR_OPEN(hood_open, 1);
			SHOW_NO_POP_BG();
			DEBUG_POP_TIMER();
			PageManager::getInstance()->hideWithShowPage(true);

		}
		else
		{
			CAR_DOOR_OPEN(hood_open, 0);
		}

	} },
		//车后盖
	{ em_car_door_esy_open, Em_priority_A, false,
	[&](bool visable){
		if (visable)
		{
			set_property_aliase_value("show_car_door", &visable);
			CAR_DOOR_OPEN(esy_open, 1);
			SHOW_NO_POP_BG();
			DEBUG_POP_TIMER();
			PageManager::getInstance()->hideWithShowPage(true);
		}
		else
		{
			CAR_DOOR_OPEN(esy_open, 0);
		}

	} },
		//冷却液温度过高
	{ em_coor_temp_height, Em_priority_A, false,
	[&](bool visable){
		if (visable)
		{
			SHOW_POP_BG(em_pop_bg_red);
			PageManager::getInstance()->showWithShowPage();
		}
		set_property_aliase_value("show_pop_coor_temp_height", &visable);
	} },
		//请马上加油
	{ em_oil_low, Em_priority_A, false,
	[&](bool visable){
		if (visable)
		{
			SHOW_POP_BG(em_pop_bg_red);
			PageManager::getInstance()->showWithShowPage();
		}
		set_property_aliase_value("show_pop_oil_low", &visable);
	} },
		//驾驶座椅安全带未系
	{ em_main_safe_belt_no_suit, Em_priority_B, false, [&](bool visable){} },
	{ em_sub_safe_belt_no_suit, Em_priority_B, false, [&](bool visable){} },
	////胎压界面,胎压监测系统故障，请联系维修人员
	{ em_tirepress_check_fault, Em_priority_B, false,
	[&](bool visable){
		if (visable)
		{
			SHOW_POP_BG(em_pop_bg_yellow);
			set_property_aliase_value("show_c_pop", &visable);
			PageManager::getInstance()->showWithShowPage();

		}
		set_property_aliase_value("show_pop_tirepress_check_fault", &visable);
	} },

	{ em_tirepressure_value_abnormal, Em_priority_B, false, [&](bool visable){
		//报警文字
		if (visable)
		{
			//SHOW_POP_BG(em_pop_bg_yellow);
			bool b = false;
			set_property_aliase_value("show_tc_up", &b);
			set_property_aliase_value("show_tc_down", &b);
			set_property_aliase_value("show_tc_bg", &b);
			DEBUG_POP_TIMER();
			SHOW_NO_POP_BG();
			PageManager::getInstance()->hideWithShowPage(true);
		}
		//set_property_aliase_value("show_pop_tirepress_check_fault", &visable);
		//tc页面
		set_property_aliase_value("show_tc", &visable);
		set_property_aliase_value("show_tc_car", &visable);
		bool b = false;
		set_property_aliase_value("show_tc_base_value", &b);

	} },

		//蓝牙电话
	{ em_bluetooth_talk, Em_priority_B, false,
	[&](bool visable){
		if (visable)
		{
			SHOW_NO_POP_BG();
		}
		//控制显示c类弹窗
		bool b = !visable;
		set_property_aliase_value("show_c_pop", &b);
	} },
		//请踩下刹车直至显示P档
	{ em_brake_until_show_P_gear, Em_priority_C, false,
	[&](bool visable){
		if (visable)
		{
			SHOW_POP_BG(em_pop_bg_yellow);
			set_property_aliase_value("show_c_pop", &visable);
			PageManager::getInstance()->showWithShowPage();
		}
		set_property_aliase_value("show_pop_brake_P_gear", &visable);
	} },
		//车内未发现钥匙
	{ em_find_no_key_in_car, Em_priority_C, false,
	[&](bool visable){
		if (visable)
		{
			SHOW_POP_BG(em_pop_bg_white);
			set_property_aliase_value("show_c_pop", &visable);
			PageManager::getInstance()->showWithShowPage();
		}
		set_property_aliase_value("show_pop_no_key_in_car", &visable);
	} },
		//启车前请挂P/N档 
	{ em_switch_P_N_gear_bef_start, Em_priority_C, false,
	[&](bool visable){
		if (visable)
		{
			SHOW_POP_BG(em_pop_bg_red);
			set_property_aliase_value("show_c_pop", &visable);
			PageManager::getInstance()->showWithShowPage();
		}
		set_property_aliase_value("show_pop_P_N_gear_bef_start", &visable);
	} },
		//启动前请踩刹车踏板
	{ em_brake_bef_start, Em_priority_C, false,
	[&](bool visable){
		if (visable)
		{
			SHOW_POP_BG(em_pop_bg_white);
			set_property_aliase_value("show_c_pop", &visable);
			//if (PageManager::getInstance()->getCurrentPageId() == TC_menu)
			//	PageManager::getInstance()->switchToCurrentDlg();
			PageManager::getInstance()->showWithShowPage();

		}
		set_property_aliase_value("show_pop_brake_bef_start", &visable);
	} },
		//熄火请挂P档
	{ em_stop_switch_P_gear, Em_priority_C, false,
	[&](bool visable){
		if (visable)
		{
			SHOW_POP_BG(em_pop_bg_white);
			set_property_aliase_value("show_c_pop", &visable);
			PageManager::getInstance()->showWithShowPage();

		}
		set_property_aliase_value("show_pop_stop_after_P_gear", &visable);
	} },
};





int tmid_one_sec = 0, tmid_five_sec = 0, tmid_pop_reset = 0;
int tmid_pop_door = 0, tmid_pop_coor = 0, tmid_pop_oil = 0;

int tmid_one_msec = 0;


//menu animation

int tmid_menu = 0;

int tmid_test = 0;

Menu_admin menu_ani;


void MyPopWin::init_pop_win_data(void)
{
	map_pop.clear();
	map_pop_data.clear();
	vec_fault_list.clear();
	map_fault_list.clear();
	map_rest_list.clear();
	//map_pop_data[em_key_ok] = false;
	need_open_car_door = true;

	map_fault_list[em_car_door_left_front_open] = "左前车门未完全关闭 ";
	map_fault_list[em_car_door_left_back_open] = "左后门未完全关闭 ";
	map_fault_list[em_car_door_right_front_open] = "右前门未完全关闭 ";
	map_fault_list[em_car_door_engine_open] = "引擎盖未完全关闭 ";
	map_fault_list[em_car_door_right_back_open] = "右后门未完全关闭 ";
	map_fault_list[em_car_door_esy_open] = "后备箱未完全关闭 ";
	map_fault_list[em_coor_temp_height] = "冷却液温度过高 ";
	map_fault_list[em_oil_low] = "请马上加油 ";
	map_fault_list[em_main_safe_belt_no_suit] = "驾驶员安全带未系 ";
	map_fault_list[em_sub_safe_belt_no_suit] = "副驾驶座椅安全带未系 ";
	map_fault_list[em_tirepress_check_fault] = "胎压监测系统故障，请联系维修人员 ";
	map_fault_list[em_tirepressure_value_abnormal] = "胎压/胎温异常 请减速小心驾驶 ";

}

void MyPopWin::Pop_win_vec()
{
	//for (auto &n :vec)
	//{
	//	Pop_Window();
	//}
}

void MyPopWin::Pop_Window(Em_PoPId id, bool visable)
{

	if (id > em_max || id < em_none)
		return;

	//相同的弹窗，状态也相同过滤
	//if (map_pop_data[id] == visable /*&& id!=em_key_ok*/)
	if (map_pop[id] == visable /*&& id!=em_key_ok*/)
		return;

	if (visable)
	{
		//存储数据
		map_pop_data[id] = visable;
		update_car_door_data(id, visable);

		if (cur_id == em_none)
		{
			map_pop[id] = visable;

			//c类弹窗 
			if (id >= em_brake_until_show_P_gear)
			{
				pop_arr[id].handle(true);
				cur_id = id;
				pop_arr[id].bInterupt = false;

				//启动1s定时器处理pop_arr[id]允许被打断 
				if (g_timer.is_actived_timer_ex(tmid_one_sec))
				{
					g_timer.deactive_time_ex(tmid_one_sec);
					printf("-----------1\n");
				}
				g_timer.active_timer_ex(tmid_one_sec, 1000);
				//启动5s定时器处理pop_arr[id]到下一个显示 
				if (g_timer.is_actived_timer_ex(tmid_five_sec))
				{
					g_timer.deactive_time_ex(tmid_five_sec);
				}
				g_timer.active_timer_ex(tmid_five_sec, 5000);

			}
			else if (id <= em_bluetooth_talk &&id > em_key_ok)
			{
				pop_arr[id].handle(true);
				cur_id = id;
				pop_arr[id].bInterupt = false;
				//启动1s定时器处理pop_arr[id]允许被打断
				if (g_timer.is_actived_timer_ex(tmid_one_sec))
				{
					g_timer.deactive_time_ex(tmid_one_sec);
					//printf("-----------2\n");
				}
				g_timer.active_timer_ex(tmid_one_sec, 1000);
			}
			else if (id == em_key_ok)
			{
				map_pop[id] = false;
				map_pop_data[id] = false;
			}
		}
		else
		{
			map_pop[id] = visable;

			if (deal_with_car_door_open(id, visable))
			{
				return;
			}
			//根据id类型c 
			if (id >= em_brake_until_show_P_gear)
			{

				if (id < cur_id)
				{
					if (pop_arr[cur_id].bInterupt)
					{
						//隐藏当前弹窗 
						hide_curren_win();
						bool b = show_next_win();
						//printf("-------interupt show_next_win cur_id=%d\n", cur_id);

						if (!b)
						{
							cur_id = em_none;
							SHOW_NO_POP_BG();
							//PageManager::getInstance()->hideWithShowPage(false);
							PageManager::getInstance()->hideWithShowPage(false);
						}
						else
						{
							//启动1s定时器处理pop_arr[id]允许被打断 
							if (g_timer.is_actived_timer_ex(tmid_one_sec))
							{
								//g_timer.deactive_time_ex(tmid_one_sec);
								g_timer.active_timer_ex(tmid_one_sec, 1000);
								//printf("-----------3\n");
							}
							else
							{
								g_timer.active_timer_ex(tmid_one_sec, 1000);
							}
							//启动5s定时器处理pop_arr[id]到下一个显示 
							if (g_timer.is_actived_timer_ex(tmid_five_sec))
							{
								g_timer.deactive_time_ex(tmid_five_sec);
							}
							g_timer.active_timer_ex(tmid_five_sec, 5000);
						}

					}
					else
					{
						need_interupt = true;//需要打断 
						printf("need_interupt=true wait timer show id=%d\n", id);
					}
				}


			}
			//A,B类弹窗 
			else if (id < em_brake_until_show_P_gear &&id >em_key_ok)
			{
				if (id < cur_id)
				{
					if (pop_arr[cur_id].bInterupt)
					{
						//隐藏当前弹窗 
						hide_curren_win();

						pop_arr[id].handle(true);
						pop_arr[id].bInterupt = false;
						need_interupt = false;
						//启动1s定时器处理pop_arr[id]允许被打断 
						if (g_timer.is_actived_timer_ex(tmid_one_sec))
						{
							g_timer.deactive_time_ex(tmid_one_sec);
							//printf("-----------4\n");
						}
						g_timer.active_timer_ex(tmid_one_sec, 1000);
						//当前A,B类弹窗被打断
						if (cur_id > em_key_ok && cur_id < em_bluetooth_talk)
						{
							//如果是a类弹窗 
							if (cur_id <= em_oil_low &&cur_id > em_key_ok)
							{
								//30分钟定时
								active_timer_a_thirtymin(cur_id);
								printf("30min show id=%d\n", cur_id);
							}
							//添加故障列表 
							send_to_fault_list(cur_id, true);
						}
						else
						{
							;
						}
						cur_id = id;
					}
					else
					{
						need_interupt = true;//需要打断
					}
				}
			}
			else if (em_key_ok == id)//ok 按键处理
			{
				//清除id状态
				map_pop[em_key_ok] = false;
				map_pop_data[em_key_ok] = false;
				//当前弹窗在显示
				if (map_pop[cur_id])
				{
					if (pop_arr[cur_id].bInterupt)
					{

						//关闭车门弹窗
						if (cur_id <= em_car_door_esy_open && cur_id >= em_car_door_left_front_open)
						{
							bool visable = false;
							set_property_aliase_value("show_car_door", &visable);
							//清除车门弹窗数据
							clear_car_door_data_from_map_pop();
						}
						else
						{
							//隐藏当前弹窗
							hide_curren_win();
						}

						//当前A,B类弹窗被打断
						if (cur_id > em_key_ok && cur_id < em_bluetooth_talk)
						{
							//如果是a类弹窗 
							if (cur_id <= em_oil_low &&cur_id > em_key_ok)
							{
								active_timer_a_thirtymin(cur_id);
								printf("30min show id=%d\n", cur_id);
							}

							//if car door
							if (is_car_door_open())
							{
								send_car_door_fault_list();
								printf("add car open fault list\n");
							}
							else
							{
								send_to_fault_list(cur_id, true);
								printf("add not car open fault list\n");
							}
						}

						bool b = false;
						for (auto n = map_pop.begin(); n != map_pop.end(); n++)
						{
							if (n->second)
							{
								int mid = n->first;
								pop_arr[mid].handle(true);
								pop_arr[mid].bInterupt = false;
								need_interupt = false;
								//启动1s定时器处理pop_arr[id]允许被打断
								//如果是蓝牙,看需不需要1s定时
								if (g_timer.is_actived_timer_ex(tmid_one_sec))
								{
									g_timer.deactive_time_ex(tmid_one_sec);
									printf("-----------5\n");
								}
								g_timer.active_timer_ex(tmid_one_sec, 1000);

								cur_id = id;
								b = true;
								break;
							}
						}
						//没有弹窗找到
						if (!b)
						{
							cur_id = em_none;
							SHOW_NO_POP_BG();
							//PageManager::getInstance()->hideLowlvPage();
							//PageManager::getInstance()->hideLowlvPage();
							PageManager::getInstance()->hideWithShowPage(false);
						}
					}
					else
					{
						need_interupt = true;//需要打断
					}

				}
			}

		}

	}
	//弹窗不可见visable=false
	else
	{
		//存储数据
		map_pop_data[id] = visable;
		//
		map_pop[id] = visable;

		if (cur_id == em_none)
		{
			return;
		}
		else
		{
			//发送取消对应的故障列表,need do
			send_to_fault_list(id, false);

			//update car_door_data
			update_car_door_data(id, visable);

			//cur_id >= em_car_door_left_front_open &&cur_id <= em_car_door_esy_open
			//关闭车门
			if (cur_id >= em_car_door_left_front_open &&cur_id <= em_car_door_esy_open)
			{
				if (id >= em_car_door_left_front_open &&id <= em_car_door_esy_open)
				{
					deal_with_car_door_close(id, visable);

					if (!is_car_door_open())
					{
						bool b = false;
						//寻找下一个需要显示的弹窗
						for (auto n = map_pop.begin(); n != map_pop.end(); n++)
						{
							if (n->second)
							{
								Em_PoPId mid = n->first;
								pop_arr[mid].handle(true);
								pop_arr[mid].bInterupt = false;
								need_interupt = false;
								cur_id = mid;
								//启动1s定时器处理pop_arr[id]允许被打断
								//如果是蓝牙,看需不需要1s定时
								if (g_timer.is_actived_timer_ex(tmid_one_sec))
								{
									g_timer.deactive_time_ex(tmid_one_sec);
									printf("-----------6\n");
								}
								g_timer.active_timer_ex(tmid_one_sec, 1000);

								//如果是c类弹窗5s后消失
								if (cur_id >= em_brake_until_show_P_gear && cur_id < em_max)
								{
									if (g_timer.is_actived_timer_ex(tmid_five_sec))
									{
										g_timer.deactive_time_ex(tmid_five_sec);
									}
									g_timer.active_timer_ex(tmid_five_sec, 5000);
								}
								b = true;
								break;
							}
						}
						//找不到下一弹窗 
						if (!b)
						{
							cur_id = em_none;
							SHOW_NO_POP_BG();
							//PageManager::getInstance()->switchToCurrentDlg();
							//PageManager::getInstance()->hideLowlvPage();
							PageManager::getInstance()->hideWithShowPage(false);
						}
					}
				}
				//没有车门 
				else if (id > em_car_door_esy_open)
				{
					hide_win_by_id(id);
				}
			}
			//cur_id>em_car_door_esy_open
			else
			{
				if (id == cur_id)
				{
					hide_curren_win();
					bool b = false;
					//寻找下一个需要显示的弹窗
					for (auto n = map_pop.begin(); n != map_pop.end(); n++)
					{
						if (n->second)
						{
							Em_PoPId mid = n->first;
							pop_arr[mid].handle(true);
							pop_arr[mid].bInterupt = false;
							need_interupt = false;
							cur_id = mid;
							//启动1s定时器处理pop_arr[id]允许被打断
							//如果是蓝牙,看需不需要1s定时
							if (g_timer.is_actived_timer_ex(tmid_one_sec))
							{
								g_timer.deactive_time_ex(tmid_one_sec);
								printf("-----------6\n");
							}
							g_timer.active_timer_ex(tmid_one_sec, 1000);

							//如果是c类弹窗5s后消失
							if (cur_id >= em_brake_until_show_P_gear && cur_id < em_max)
							{
								if (g_timer.is_actived_timer_ex(tmid_five_sec))
								{
									g_timer.deactive_time_ex(tmid_five_sec);
								}
								g_timer.active_timer_ex(tmid_five_sec, 5000);
							}
							b = true;
							break;
						}
					}
					//找不到下一弹窗 
					if (!b)
					{
						cur_id = em_none;
						SHOW_NO_POP_BG();
						//PageManager::getInstance()->switchToCurrentDlg();
						//PageManager::getInstance()->hideLowlvPage();
						PageManager::getInstance()->hideWithShowPage(false);
					}
				}
				else ///(id > cur_id) &&id<cur_id
				{
					hide_win_by_id(id);
				}

			}

		}
	}


	printf("rev pop id=%d,sts=%d \n", id, map_pop[id]);
}

bool MyPopWin::isInBlueToothTalk()
{
	return false;
}

void MyPopWin::hide_curren_win(void)
{
	pop_arr[cur_id].handle(false);
	pop_arr[cur_id].bInterupt = false;
	map_pop[cur_id] = false;
	need_interupt = false;
	printf("remove pop id=%d\n", cur_id);
}

void MyPopWin::hide_win_by_id(Em_PoPId id)
{
	if (id <em_none || id >em_max)
		return;

	pop_arr[id].handle(false);
	pop_arr[id].bInterupt = false;
}

bool MyPopWin::show_next_win(void)
{
	bool b = false;
	printf("map_pop size=%d\n", map_pop.size());
	for (auto n = map_pop.begin(); n != map_pop.end(); n++)
	{
		if (n->second)
		{
			Em_PoPId mid = n->first;
			pop_arr[mid].handle(true);
			pop_arr[mid].bInterupt = false;
			need_interupt = false;
			cur_id = mid;
			b = true;
			printf("-------next id=%d\n", cur_id);
			break;
		}
		//printf("map id=%d,sts=%d\n", n->first, n->second);
	}

	return b;
}

bool MyPopWin::is_in_thirty_pop_min(Em_PoPId id)
{

	if (map_pop_data.find(id) != map_pop_data.end())
	{
		auto iter = map_pop_data.find(id);
		if (iter->second)
		{
			if (id <em_main_safe_belt_no_suit &&id >em_key_ok)
			{
				return true;
			}
		}
	}
	return false;

}

void MyPopWin::get_one_car_door_data(Em_PoPId & id, bool & sts)
{
	sts = false;
	for (int i = 0; i < sizeof(car_door) / sizeof(car_door[0]); i++)
	{
		if (car_door[i])
		{
			id = Em_PoPId(id + 2);
			sts = true;
			return;
		}
	}
}

bool MyPopWin::is_one_pop_from_map_pop(void)
{

	for (auto &n : map_pop)
	{
		if (n.second)
		{
			if (n.first <em_max &&n.first >em_key_ok)
				return true;
		}
	}
	return false;
}

bool MyPopWin::deal_with_car_door_open(Em_PoPId id, bool sts)
{
	bool b = false;
	if (sts)
	{
		if (cur_id >= em_car_door_left_front_open && cur_id <= em_car_door_esy_open && id >= em_car_door_left_front_open &&id <= em_car_door_esy_open)
		{
			pop_arr[id].handle(sts);
			update_car_door_data(id, sts);
			b=is_car_door_open();
		}
	}
	return b;
}


void MyPopWin::deal_with_car_door_close(Em_PoPId id, bool sts)
{
	if (!is_car_door_open())
	{
		all_door_close = true;
		need_open_car_door = true;
	}

	if (cur_id >= em_car_door_left_front_open && cur_id <= em_car_door_esy_open && id >= em_car_door_left_front_open &&id <= em_car_door_esy_open)
	{
		pop_arr[id].handle(sts);
		pop_arr[id].bInterupt = false;
		update_car_door_data(id, sts);
	}
}


void MyPopWin::update_car_door_data(Em_PoPId id, bool sts)
{
	if (id >= em_car_door_left_front_open &&id <= em_car_door_esy_open)
	{
		Em_car_door  pid = Em_car_door(id - 2);
		car_door[pid] = sts;
	}
}

bool MyPopWin::is_car_door_open(void)
{
	bool b = false;
	for (auto &n : car_door)
	{
		if (n)
			return true;
	}
	return b;
}


bool MyPopWin::is_car_door_id(Em_PoPId id)
{
	return (id >= em_car_door_left_front_open &&id <= em_car_door_esy_open);
}

void MyPopWin::send_car_door_fault_list(void)
{
	for (int i = em_left_front_door; i < em_car_door_max; i++)
	{
		if (car_door[i])
		{
			Em_PoPId id = Em_PoPId(i + 2);
			send_to_fault_list(id, true);
		}
	}
}

void MyPopWin::active_timer_a_thirtymin(Em_PoPId id)
{
	if (is_car_door_id(id))
	{
		g_timer.active_timer_ex(tmid_pop_door, THIRTY_MIN);
	}
	else if (id == em_coor_temp_height)
	{
		g_timer.active_timer_ex(tmid_pop_coor, THIRTY_MIN);
	}
	else if (id == em_oil_low)
	{
		g_timer.active_timer_ex(tmid_pop_oil, THIRTY_MIN);
	}
}


void MyPopWin::clear_pop_win_from_map_pop(Em_PoPId id)
{
	if (map_pop.find(id) != map_pop.end())
	{
		map_pop[id] = false;
	}
}

void MyPopWin::clear_car_door_data_from_map_pop()
{
	map_pop[em_car_door_left_front_open] = false;
	map_pop[em_car_door_left_back_open] = false;
	map_pop[em_car_door_right_back_open] = false;
	map_pop[em_car_door_right_front_open] = false;
	map_pop[em_car_door_engine_open] = false;
	map_pop[em_car_door_esy_open] = false;
}

void MyPopWin::send_to_fault_list(Em_PoPId id, bool sts)
{

	//for (auto &n:)
	if (map_fault_list.find(id) == map_fault_list.end())
		return;

	if (sts)
	{
		for (auto &n : vec_fault_list)
		{
			if (n == id)
			{
				return;
			}
		}
		// 添加故障列表 
		vec_fault_list.emplace_back(id);
		add_fault_list(id, sts);
	}
	else
	{
		for (auto iter = vec_fault_list.begin(); iter != vec_fault_list.end(); iter++)
		{
			if (*iter == id)
			{
				// 移除故障列表 
				vec_fault_list.erase(iter);
				add_fault_list(id, sts);

				if (vec_fault_list.size() <= 0)
				{
					bool b = false;
					set_property_aliase_value("show_fault_list", &b);
				}
				return;
			}
		}
	}

}


void MyPopWin::add_fault_list(Em_PoPId id, bool sts)
{
	ft_base * ptr = (ft_base *)get_aliase_ui_control("show_fault_list");

	ft_base * pitem = (ft_base *)get_aliase_ui_control("show_fault_template");

	if (sts)
	{
		if (ptr && pitem)
		{
			ft_image*  p = (ft_image*)pitem->get_copy_of_object();

			ft_textblock   * t = (ft_textblock *)(p->get_child(0));
			printf("fault =%s\n", map_fault_list[id].c_str());

			t->set_content(map_fault_list[id].c_str());

			if (id >= em_car_door_left_front_open && id <= em_oil_low)
			{
				p->set_texture_id({ 0, en_gid_0icon_ic_warning_red_png });
			}

			else if (id >= em_main_safe_belt_no_suit && id <= em_tirepressure_value_abnormal)
			{
				p->set_texture_id({ 0, en_gid_0icon_ic_warning_yellow_png });
			}
			ptr->add_child(p);
			p->set_visible(true);
		}
	}
	else
	{
		int n = 0;
		for (auto iter = vec_fault_list.begin(); iter != vec_fault_list.end(); iter++)
		{
			if (id == *iter)
			{
				break;
			}
			n++;
		}

		if (ptr->get_child_count())
			ptr->remove_child(ptr->get_child(n));
	}
}


std::function<void(int a, int b)> fun_play_back = [&](int a, int b){
	if (a&&!b)//关门动效 
	{
		if (my_popWin.all_door_close)
		{
			//如果没有弹窗，车身回正 
			if (!my_popWin.is_one_pop_from_map_pop())
			{
				play_tran("car_roation", 1, 0);
				my_popWin.all_door_close = false;
			}
			else
			{
				bool visable = false;
				set_property_aliase_value("show_car_door", &visable);
				restore_trans_value("car_roation", 0);
				my_popWin.all_door_close = false;
			}

		}
	}
	//开门动效
	else if (!a&&b)
	{
		if (my_popWin.need_open_car_door)
		{
			my_popWin.need_open_car_door = false;
			//restore_trans_value("car_roation", 0);
			//车门旋转
			play_tran("car_roation", 0, 1);
		}
	}
};
#include <math.h> 
ft_video* pshow_navi_video_stream=nullptr;

void register_pop_win_cmd()
{
    //pshow_navi_video_stream=(ft_video*)get_aliase_ui_control("show_navi_video_stream");
	pshow_navi_video_stream = (ft_video*)get_aliase_ui_control("show_big_navi");

	g_mp_var_set["test_jpg"] = [&](char* str_buff){
		ifstream fin;
		fin.open(str_buff,ios::binary);
		if (fin.is_open())
		{
			auto file_size=fin.tellg();
			fin.seekg(0, ios::end);
			file_size = fin.tellg() - file_size;
			fin.seekg(0, ios::beg);
			char* pimage_buff=new char[file_size];
			fin.read(pimage_buff,file_size);
			pshow_navi_video_stream->update_text_from_image_buff((u8*)pimage_buff, file_size);
			printf("update jpg:%s size=%d\n",str_buff,file_size);
			delete[] pimage_buff;
			fin.close();
		}
	};
	g_mp_var_set[string("test")] = [&](char* str_buff){
		printf("recv:%s\n", str_buff);
		char str[] = { 0x00, 0x1, 0x1, 0x21 };
		struct Struct_Tripa
		{
			u8 is_vaild : 8;
			u32 value : 24;
		};
		Struct_Tripa *p = (Struct_Tripa *)str;

		printf("value=%d\n", p->value);
	};

	g_mp_var_set["speed_value"] = [&](char* str_buff){
		printf("recv:%s\n", str_buff);
		int value = atof(str_buff);
		float  b = value / 240.0;
		//mtl_speed->set_value("percent", &b, 1);
		ptext_speed->set_content(str_buff);
	};
	printf("debug_test function\n");

	reg_trans_handle("left_front_door_open", [&](int a, int b){
		fun_play_back(a, b);
	});
	reg_trans_handle("right_front_door_open", [&](int a, int b){
		fun_play_back(a, b);
	});
	reg_trans_handle("left_back_door_open", [&](int a, int b){
		fun_play_back(a, b);
	});
	reg_trans_handle("right_back_door_open", [&](int a, int b){
		fun_play_back(a, b);
	});

	reg_trans_handle("esy_open", [&](int a, int b){
		fun_play_back(a, b);
	});
	reg_trans_handle("hood_open", [&](int a, int b){
		fun_play_back(a, b);
	});

	reg_trans_handle("car_roation", [&](int a, int b){
		if (a&&!b)//关门动效 
		{
			//如果没有弹窗，车身回正 
			if (!my_popWin.is_one_pop_from_map_pop())
			{
				bool visable = false;
				set_property_aliase_value("show_car_door", &visable);
				restore_trans_value("car_roation", 0);
			}
		}
	});

	tmid_one_msec = g_timer.register_timer_ex([&](int tid){
		//c类弹窗 
		if (my_popWin.cur_id >= em_brake_until_show_P_gear && my_popWin.cur_id <= em_stop_switch_P_gear)
		{
			if (g_timer.is_actived_timer_ex(tmid_five_sec))
			{
				g_timer.deactive_time_ex(tmid_five_sec);
				printf("-----------10\n");
			}
			g_timer.active_timer_ex(tmid_five_sec, 5000);
		}
		//a,b类弹窗 
		if (my_popWin.cur_id >= em_car_door_left_front_open && my_popWin.cur_id <= em_stop_switch_P_gear)
		{
			if (g_timer.is_actived_timer_ex(tmid_one_sec))
			{
				g_timer.deactive_time_ex(tmid_one_sec);
			}
			g_timer.active_timer_ex(tmid_one_sec, 1000);
		}

		//a类弹窗

		g_timer.deactive_time_ex(tmid_one_msec);

	});

	// 1s定时器 
	tmid_one_sec = g_timer.register_timer_ex([&](int tid){
		printf("timer1 tid=%d\n", tid);
		pop_arr[my_popWin.cur_id].bInterupt = true;
		if (my_popWin.need_interupt)
		{
			// 隐藏当前弹窗 
			my_popWin.hide_curren_win();
			//如果是a,b类弹窗 
			if (my_popWin.cur_id <= em_brake_until_show_P_gear &&my_popWin.cur_id > em_key_ok)
			{
				//如果是a类弹窗 
				if (my_popWin.cur_id <= em_oil_low &&my_popWin.cur_id > em_key_ok)
				{
					my_popWin.active_timer_a_thirtymin(my_popWin.cur_id);
					printf("30min show id=%d\n", my_popWin.cur_id);
				}
				my_popWin.send_to_fault_list(my_popWin.cur_id, true);

			}

			// 显示下一个弹窗 
			printf("timer1 remove id=%d\n", my_popWin.cur_id);
			bool b = my_popWin.show_next_win();
			if (!b)
			{
				my_popWin.cur_id = em_none;
				SHOW_NO_POP_BG();
				//PageManager::getInstance()->switchToCurrentDlg();
				//PageManager::getInstance()->hideLowlvPage();
				PageManager::getInstance()->hideWithShowPage(false);
			}
			else
			{
				//if (!g_timer.is_actived_timer_ex(tmid_one_msec))
				{
					g_timer.active_timer_ex(tmid_one_msec, 50);
					printf("1ms timer\n");
				}
			}
		}
		g_timer.deactive_time_ex(tmid_one_sec);

	});

	//5s,c类弹窗 
	tmid_five_sec = g_timer.register_timer_ex([&](int tid){
		// 当前非c类弹窗，关闭定时 
		if (my_popWin.cur_id < em_brake_until_show_P_gear)
		{
			g_timer.deactive_time_ex(tmid_five_sec);
			return;
		}
		// 隐藏当前弹窗 
		my_popWin.hide_curren_win();
		// 显示下一个弹窗 

		bool b = my_popWin.show_next_win();
		if (!b)
		{
			my_popWin.cur_id = em_none;
			SHOW_NO_POP_BG();
			//PageManager::getInstance()->switchToCurrentDlg();
			//PageManager::getInstance()->hideLowlvPage();
			PageManager::getInstance()->hideWithShowPage(false);
		}
		g_timer.deactive_time_ex(tmid_five_sec);

	});

	//车门30分钟
	tmid_pop_door = g_timer.register_timer_ex([&](int tid){
		Em_PoPId id = em_none;
		bool b = false;
		my_popWin.get_one_car_door_data(id, b);
		//找到对应的车门未关
		if (b)
		{
			my_popWin.Pop_Window(id, b);
		}
		//车门都是关闭的
		else
		{
			printf("timer tmid_pop_door all car door close\n");
		}
		g_timer.deactive_time_ex(tmid_pop_door);

	});

	//水温30分钟
	tmid_pop_coor = g_timer.register_timer_ex([&](int tid){
		if (my_popWin.is_in_thirty_pop_min(em_coor_temp_height))
		{
			my_popWin.Pop_Window(em_coor_temp_height, true);
		}
		else
		{
			printf("timer tmid_pop_coor coor_temp_height close\n");
		}
		g_timer.deactive_time_ex(tmid_pop_coor);
	});

	//燃油30分钟
	tmid_pop_oil = g_timer.register_timer_ex([&](int tid){
		if (my_popWin.is_in_thirty_pop_min(em_oil_low))
		{
			my_popWin.Pop_Window(em_oil_low, true);
		}
		else
		{
			printf("timer  tmid_pop_oil em_coor_temp_height close\n");
		}
		g_timer.deactive_time_ex(tmid_pop_oil);
	});

	tmid_test = g_timer.register_timer_ex([&](int tid) {
		static bool myb = false;
		my_popWin.Pop_Window(em_car_door_left_front_open, myb);
		myb = ! myb;
	});

	//popWin init data
	my_popWin.init_pop_win_data();

	g_msg_host.attach_monitor("pop_win", [&](u8*pbuff, int len){
		struct GNU_DEF struct_pop_msg
		{
			//bit 0
			u8 lf_door_open:1;//左前
			u8 rf_door_open : 1;//右前
			u8 lr_door_open : 1;//左后
			u8 rr_door_open : 1;//右后
			u8 engine_door_open : 1;//引擎
			u8 esy_door_open : 1;//后背箱
			u8 coor_temp_high : 1;//冷却液温度过高
			u8 fuel_oil_low : 1;//燃油液位过低报警
			//bit 1
			u8 main_safe_belt_no_fasten : 1;//驾驶员安全带未系
			u8 sub_safe_belt_no_fasten : 1;//副驾驶座椅安全带未系
			u8 tire_system_fault : 1;//胎压系统故障报警
			u8 tire_fault : 1;//胎压过高/过低，胎温过高报警
			u8 brake_press_epb:1;//请踩下刹车踏板 按EPB按键
			u8 find_no_key : 1;//车内未发现钥匙
			u8 switch_p_n_gear_bef_star : 1;//启车前请挂P/N档
			u8 brake_bef_star : 1;//启动前请踩刹车踏板
			//bit 2
			u8 switch_p_when_stop : 1;//熄火请挂P档
			u8 lf_tire_pressure_high : 1;//左前轮胎压高
			u8 lf_tire_pressure_low : 1;//左前轮胎压低
			u8 lf_tire_temperature_high : 1;//左前轮胎温高

			u8 rf_tire_pressure_high : 1;// 右前轮胎压高
			u8 rf_tire_pressure_low : 1;// 右前轮胎压低
			u8 rf_tire_temperature_high : 1;// 右前轮胎温高

			u8 lr_tire_pressure_high : 1;// 左后轮胎压高
			//bit 3
			u8 lr_tire_pressure_low : 1;// 左后轮胎压低
			u8 lr_tire_temperature_high : 1;// 左后轮胎温高

			u8 rr_tire_pressure_high : 1;// 右后轮胎压高
			u8 rr_tire_pressure_low : 1;// 右后轮胎压低
			u8 rr_tire_temperature_high : 1;// 右后轮胎温高
			u8 resove : 3;
		};
		struct_pop_msg *p = (struct_pop_msg *)pbuff;

		unsigned int *pt = (unsigned int *)pbuff;
		if (*pt)
		{
			//printf("hide current dlg before show pop\n");
		}

		//车门没关
		if (p->lf_door_open)
			my_popWin.Pop_Window(em_car_door_left_front_open, true);
		else
			my_popWin.Pop_Window(em_car_door_left_front_open, false);

		if (p->rf_door_open)
			my_popWin.Pop_Window(em_car_door_right_front_open, true);
		else
			my_popWin.Pop_Window(em_car_door_right_front_open, false);

		if (p->lr_door_open)
			my_popWin.Pop_Window(em_car_door_left_back_open, true);
		else
			my_popWin.Pop_Window(em_car_door_left_back_open, false);

		if (p->rr_door_open)
			my_popWin.Pop_Window(em_car_door_right_back_open, true);
		else
			my_popWin.Pop_Window(em_car_door_right_back_open, false);

		if (p->engine_door_open)
			my_popWin.Pop_Window(em_car_door_engine_open, true);
		else
			my_popWin.Pop_Window(em_car_door_engine_open, false);

		if (p->esy_door_open)
			my_popWin.Pop_Window(em_car_door_esy_open, true);
		else
			my_popWin.Pop_Window(em_car_door_esy_open, false);

		//冷却液温度高
		if (p->coor_temp_high)
			my_popWin.Pop_Window(em_coor_temp_height, true);
		else
			my_popWin.Pop_Window(em_coor_temp_height, false);

		//燃油低
		if (p->fuel_oil_low)
			my_popWin.Pop_Window(em_oil_low, true);
		else
			my_popWin.Pop_Window(em_oil_low, false);

		//主驾驶安全带未系
		if (p->main_safe_belt_no_fasten)
			my_popWin.Pop_Window(em_main_safe_belt_no_suit, true);
		else
			my_popWin.Pop_Window(em_main_safe_belt_no_suit, false);

		//副驾驶安全带未系
		if (p->main_safe_belt_no_fasten)
			my_popWin.Pop_Window(em_main_safe_belt_no_suit, true);
		else
			my_popWin.Pop_Window(em_main_safe_belt_no_suit, false);

		//胎压系统故障报警
		if (p->tire_system_fault)
			my_popWin.Pop_Window(em_tirepress_check_fault, true);
		else
			my_popWin.Pop_Window(em_tirepress_check_fault, false);

		//胎压故障报警
		//if (p->tire_fault)
		//{
		//	my_popWin.Pop_Window(em_tirepressure_value_abnormal, true);
		//}
		//else{
		//	my_popWin.Pop_Window(em_tirepressure_value_abnormal, false);
		//}

		//请踩下刹车踏板 按EPB按键
		if (p->brake_press_epb)
			my_popWin.Pop_Window(em_brake_until_show_P_gear, true);
		else
			my_popWin.Pop_Window(em_brake_until_show_P_gear, false);

		//车内未发现钥匙
		if (p->find_no_key)
			my_popWin.Pop_Window(em_find_no_key_in_car, true);
		else
			my_popWin.Pop_Window(em_find_no_key_in_car, false);

		//启车前请挂P/N档
		if (p->switch_p_n_gear_bef_star)
			my_popWin.Pop_Window(em_switch_P_N_gear_bef_start, true);
		else
			my_popWin.Pop_Window(em_switch_P_N_gear_bef_start, false);

		//启动前请踩刹车踏板
		if (p->brake_bef_star)
			my_popWin.Pop_Window(em_brake_bef_start, true);
		else
			my_popWin.Pop_Window(em_brake_bef_start, false);

		//熄火请挂P档
		if (p->switch_p_when_stop)
			my_popWin.Pop_Window(em_stop_switch_P_gear, true);
		else
			my_popWin.Pop_Window(em_stop_switch_P_gear, false);

		int tmp = 0;
		tmp += p->lf_tire_pressure_high;
		tmp += p->lf_tire_pressure_low;
		tmp += p->lf_tire_temperature_high;

		tmp += p->rf_tire_pressure_high;
		tmp += p->rf_tire_pressure_low;
		tmp += p->rf_tire_temperature_high;

		tmp += p->lr_tire_pressure_high;
		tmp += p->lr_tire_pressure_low;
		tmp += p->lr_tire_temperature_high;

		tmp += p->rr_tire_pressure_high;
		tmp += p->rr_tire_pressure_low;
		tmp += p->rr_tire_temperature_high;
		//胎压报警
		if (tmp){
			my_popWin.Pop_Window(em_tirepressure_value_abnormal, true);
		}
		else{
			my_popWin.Pop_Window(em_tirepressure_value_abnormal, false);
		}

		int tp = 0;
		tp += p->lf_door_open;
		tp += p->rf_door_open;
		tp += p->lr_door_open;
		tp += p->rf_door_open;
		tp += p->engine_door_open;
		tp += p->esy_door_open;

		if (tmp || tp)
		{
			//PageManager::getInstance()->hideCurrentPage();
			g_timer.deactive_time_ex(tmid_back_tc);
		}
		else
		{
			//if (PageManager::getInstance()->getCurrentPageId() != TC_menu)
			//{
			//	PageManager::getInstance()->hideCurrentPage();
			//}
		}

	});

	//菜单界面animation
	tmid_menu = g_timer.register_timer_ex([&](int tid) {

		//static int cut = 0;
		menu_ani.value = menu_ani.k *(menu_ani.end_value - menu_ani.value) + menu_ani.value;
		if (fabs(menu_ani.value - menu_ani.end_value) < 0.5 || menu_ani.need_quick_ani)
		{
			if (menu_ani.is_next)
			{
				ft_base *ptr = (ft_base *)get_aliase_ui_control("show_menu_list");
				ft_image *p = static_cast<ft_image *>(ptr->get_child(0));
				ptr->remove_child(p, false);
				ptr->add_child(p);

				//复位
				float v = -50.f;
				set_property_aliase_value("control_menu_top", &v);
				//cut = 0;
				menu_ani.need_quick_ani = false;
				g_timer.deactive_time_ex(tmid_menu);
				return;
			}
			menu_ani.value = menu_ani.end_value;
			//cut = 0;
			set_property_aliase_value("control_menu_top", &menu_ani.value);
			menu_ani.need_quick_ani = false;
			g_timer.deactive_time_ex(tmid_menu);
		}
		else
		{
			set_property_aliase_value("control_menu_top", &menu_ani.value);
		}
		//cut++;
		//printf("cut =%d, value=%f\n", cut, menu_ani.value);

	});

	//仪表按键
	g_msg_host.attach_monitor("rv_key", [&](u8*pbuff, int len){
		struct GNU_DEF struct_key_msg
		{
			u8 key_value;//
			u8 key_state;//
			u8 key_from;
		};
		struct_key_msg *p = (struct_key_msg *)pbuff;
		printf("value=%d,state=%d,from=%d\n", p->key_value, p->key_state, p->key_from);

		//按键 上下左右ok
		if (p->key_value >= key_up &&p->key_value <= key_right || p->key_value == key_ok)
		{
			//如果有弹窗，则由弹窗处理按键
			if (my_popWin.is_one_pop_from_map_pop() && p->key_state== key_state_press)
			{
				if (p->key_value==key_ok)
					my_popWin.Pop_Window(em_key_ok, true);
				return;
			}

			PageManager::getInstance()->currentDlgDealKey((EmKey)p->key_value, (Em_key_state)p->key_state);
		}

	});


	//g_timer.active_timer_ex(tmid_test,500);

}


void Menu_admin::hudong(float a, float b, bool b_next)
{
	start_value = a;
	end_value = b;
	value = a;
	is_next = b_next;
	if (is_next)
	{
		//存储id
		int id = cur_menu_id;
		id++;
		if (id >= main_menu_max)
			id = 0;
		cur_menu_id = (Em_main_menu)id;
		printf("cur_menu_id =%d\n",cur_menu_id);
	}
	else
	{
		//存储id
		int id = cur_menu_id;
		id--;
		if (id < 0)
			id = main_menu_max - 1;
		cur_menu_id = (Em_main_menu)id;
		printf("cur_menu_id =%d\n", cur_menu_id);
	}

}


void Menu_admin::switch_to(float a, float b)
{

}

void Menu_admin::switch_to_menu(Em_menu_first id)
{

}

void Menu_admin::switch_to_pre(void)
{
	float a = -50 - 154.0f;

	if (g_timer.is_actived_timer_ex(tmid_menu))
	{
		need_quick_ani = true;
		printf("timer on switch_to_pre cur id=%d\n", cur_menu_id);
	}
	else
	{
		set_property_aliase_value("control_menu_top", &a);

		ft_base *ptr = (ft_base *)get_aliase_ui_control("show_menu_list");

		ft_image *p = static_cast<ft_image *>(ptr->get_child(3));

		ptr->remove_child(ptr->get_child(3), false);

		ptr->insert_child(0, p);

		//menu_ani.play(a, -50.0, false);
		hudong(a, -50, false);
		g_timer.active_timer_ex(tmid_menu, 20);
		printf("timer close switch_to_pre cur id=%d\n", cur_menu_id);

	}

}

void Menu_admin::switch_to_next(void)
{
	float a = -50 - 204.0f;
	//set_property_aliase_value("control_menu_top", &a);
	//ft_base *ptr = (ft_base *)get_aliase_ui_control("show_menu_list");
	if (g_timer.is_actived_timer_ex(tmid_menu))
	{
		need_quick_ani = true;
	}
	else
	{
		//menu_ani.play(-50, a, true);
		hudong(-50, a, true);
		g_timer.active_timer_ex(tmid_menu, 20);
		printf("switch_to_next cur id=%d\n", cur_menu_id);
	}


}



#ifdef WIN32
//tmp = 81 tmp = 87 tmp = 69 tmp = 82 tmp = 65 tmp = 83 tmp = 68 tmp = 70 tmp = 90 tmp = 88 tmp = 67 tmp = 86
//tmp=265 tmp=264 tmp=263 tmp=262 tmp=257
#define key_q  81 
#define key_w  87
#define key_e  69
#define key_r  82
#define key_a 65
#define key_s 83
#define key_d 68
#define key_f 70
#define key_z 90
#define key_x 88
#define key_c 67
#define key_v 86
#define key_up 265
#define	key_down 264
#define	key_left 263
#define	key_right 262
#define	key_enter 257
#define key_t 84
#define key_y 89

#include "AppDef.h"
#include "IndicateBase.h"
#include <stdio.h>

void KeyCallback(GLFWwindow*, int key, int, int action, int mods)
{
	static int stic_a = 0;

	if (action == GLFW_RELEASE)
	{
#if 1
		if (key == key_z)
		{
			printf("cccccccccc\n");
			struct struct_music
			{
				std::string currentMusicName;
				std::string currentSinger;
				u32 currentTime;
				u32 totalTime;
				bool isSwitchMusic;
				bool bPlay;
			};
			static std::string str;
			struct_music music;
			music.currentMusicName = std::string("野蜂飞舞asdfweawfad");
			music.currentSinger = std::string("null");
			music.currentTime = 1800;
			music.totalTime = 36000;
			music.isSwitchMusic = false;
			music.bPlay = true;

			//struct_music *p = &music;
			//bool b = false;
			//set_property_aliase_value("show_media_fm_am", &b);
			//b = true;
			//set_property_aliase_value("show_media_music", &b);
			//DATA_STORE.set_media_music_fm(media_2nd_usb_music);
			//DATA_STORE.set_song_name(music.currentMusicName);
			//DATA_STORE.set_song_play(true);
			//set_property_aliase_value("control_media_song_play",&music.bPlay);
			////ft_slider *t = (ft_slider*)get_aliase_ui_control("show_small_music_slider");
			//float v = p->currentTime / (p->total_time *1.0);
			//set_property_aliase_value("control_small_music_progress", &v);

			//ft_slider * t = (ft_slider*)get_aliase_ui_control("show_big_music_slider");
			//t->set_progress(v);

			//set_property_aliase_value("show_big_music_play", &(p->bPlay));
			//set_property_aliase_value("show_small_music_play", &(p->bPlay));
			//set_property_aliase_value("control_song_name_small", (void *)(p->currentMusicName.c_str()));
			//set_property_aliase_value("control_song_name_big", (void *)(p->currentMusicName.c_str()));
			//set_property_aliase_value("control_singer_name_big", (void *)(p->currentSinger.c_str()));


			//b = true;
			//set_property_aliase_value("show_music_big", &b);
			//b = false;
			//set_property_aliase_lazy_value("show_music_big", 2000, &b);

			printf("get music data from IVI\n");
			static std::string str3("");
			printf("name=%s,singer=%s\n", music.currentMusicName.c_str(), music.currentSinger.c_str());
			struct_music *p = &music;
			//关闭顶部的FM/AM
			bool b = false;
			set_property_aliase_value("show_media_fm_am", &b);
			b = true;
			set_property_aliase_value("show_media_music", &b);
			printf("1111111111111111\n");
			float v = p->currentTime / (p->totalTime *1.0);
			set_property_aliase_value("control_small_music_progress", &v);

			ft_slider * t = (ft_slider*)get_aliase_ui_control("show_big_music_slider");
			t->set_progress(v);

			set_property_aliase_value("show_big_music_play", &(p->bPlay));
			set_property_aliase_value("show_small_music_play", &(p->bPlay));

			//if (str != p->currentMusicName)
			//	str = p->currentMusicName;
			//else
			//	return;
			DATA_STORE.set_song_name(p->currentMusicName);
			DATA_STORE.set_media_music_fm(media_2nd_usb_music);
			DATA_STORE.set_song_play(p->bPlay);

			printf("currT=%d,TTime=%d,switch=%d,bplay=%d\n", p->currentTime, p->totalTime, p->isSwitchMusic, p->bPlay);
			set_property_aliase_value("control_singer_name_big", (void *)(p->currentSinger.c_str()));
			set_property_aliase_value("control_song_name_big", (void *)(p->currentMusicName.c_str()));
			set_property_aliase_value("control_song_name_small", (void *)(p->currentMusicName.c_str()));
			set_property_aliase_value("control_media_menu_song_play", &(p->bPlay));
			b = true;
			set_property_aliase_value("control_media_song_play", &b);
			printf("IVI music end\n");
			b = true;
			set_property_aliase_value("show_music_big", &b);
			b = false;
			set_property_aliase_lazy_value("show_music_big", 2000, &b);

		}
		else if (key == key_q)
		{
			struct GNU_DEF struct_radio{
				u32 channel; //频道
				u32 frequency;     //频率
			};

			struct_radio a;
			static bool stic_e = false;
			a.channel = stic_e;
			a.frequency = 650;
			struct_radio *p =&a;
			stic_e = !stic_e;
			//关闭顶部的音乐
			bool b = false;
			set_property_aliase_value("show_media_music", &b);
			b = true;
			set_property_aliase_value("show_media_fm_am", &b);

			DATA_STORE.set_media_music_fm(media_2nd_radio_fm);


			if (p->channel == em_AM)
			{
				sprintf(str_show, "%d.%d", p->frequency / 10, p->frequency % 10);
				set_property_aliase_value("control_fm_value", &(str_show));
				bool t = true;
				set_property_aliase_value("show_top_fm", &t);
				char buf[0x10] = { "AM" };
				set_property_aliase_value("control_fm_name", &buf);
			}
			else if (p->channel == em_FM)
			{
				sprintf(str_show, "%d.%d", p->frequency / 10, p->frequency % 10);
				set_property_aliase_value("control_fm_value", &(str_show));
				bool t = true;
				set_property_aliase_value("show_top_fm", &t);
				char buf[0x10] = { "FM" };
				set_property_aliase_value("control_fm_name", &buf);
			}
		}
		else  if (key == key_a)
		{
			static int stic_id = 19;
			printf("send id =%d\n", stic_id);
			my_popWin.Pop_Window(Em_PoPId(stic_id), true);
			stic_id = stic_id <= em_key_ok ? 19 : (--stic_id);
			printf("aaaaa\n");
		}
		else if (key == key_d)
		{
			static int stic_idd = 19;
			my_popWin.Pop_Window(Em_PoPId(stic_idd), false);
			stic_idd = stic_idd <= em_key_ok ? 19 : (--stic_idd);
			printf("ddddddd\n");

			//static bool stic_v = true;
			//set_property_aliase_value("show_pop_brake_bef_start", &stic_v);
			//stic_v = !stic_v;
			//pop_arr[em_car_door_left_front_open];

		}
		else if (key == key_s)
		{
			//pop_arr[2].handle(true);
			//ft_base * ptr = (ft_base *)get_aliase_ui_control("show_ft_navi_arrow");
			//printf("navi cout %d\n", ptr->get_child_count());

			//auto p = ptr->get_child(0);
			//p->set_visible(true);
			//printf("sssss\n");

			//SingIndicate::getInstance().test_indicate();
			my_popWin.Pop_Window(em_key_ok, true);
			printf("key =sssssssssss\n");
		}
		else if (key == key_w)
		{
			//if (!g_timer.is_actived_timer_ex(tmid_one_sec))
			//{
			//	g_timer.active_timer_ex(tmid_one_sec, 3000);
			//	printf("wwwww1\n");
			//}
			//printf("wwwwwwwww\n");
			//pop_arr[2].handle(false);
			//static bool vv = false;
			//if (!vv)
			//{
			//	SingIndicate::getInstance().indicate_init();
			//	vv = true;
			//}
			//SingIndicate::getInstance().self_check();
			//PageManager::getInstance()->hideCurrentPage();
			static bool stic_b = false;
			stic_b = !stic_b;
			if (stic_b)
			{
				//PageManager::getInstance()->hideWithShowPage();
			}
			////my_popWin.Pop_Window(em_car_door_left_front_open, stic_b=!stic_b);
			my_popWin.Pop_Window(em_tirepressure_value_abnormal, stic_b);
			//static u8 * stic_p=new u8[1920*720*3];
			//fread()
		}
		else if (key == key_y)
		{
			static bool stic_b = false;
			stic_b = !stic_b;
			my_popWin.Pop_Window(em_car_door_right_back_open, stic_b);
		}
		else if (key == key_c)
		{
			static bool stic_b = false;
			stic_b = !stic_b;
			my_popWin.Pop_Window(em_coor_temp_height, stic_b);
			//for (auto n = map_pop.begin(); n != map_pop.end(); n++)
			//{
			//	if (n->second)
			//		printf("--------show all id=%d ,sts=%d\n",n->first,n->second);
			//}
		}
		else if (key == key_v)
		{
			static bool stic_b = false;
			stic_b = !stic_b;
			my_popWin.Pop_Window(em_brake_bef_start, stic_b);
		}
		else if (key == key_t)
		{
			//PageManager::getInstance()->pageSwitchTo(id_none);
#if 1
			FILE *fp=NULL;
			fp = fopen("3.jpg", "rb+");
			if (fp == NULL)
			{
				printf("3.jpg open fail\n");
				return;
			}

			unsigned int length;
			//length=fseek(fp, 0, SEEK_END);
			fseek(fp, 0, SEEK_END); 
			length = ftell(fp) + 1;
			printf("length =%d\n", length);
			unsigned char *p = new unsigned char[length];
			unsigned char *pt = p;
			fseek(fp, 0, SEEK_SET);

			while (fread(pt, 1024, 10, fp))
			{
				pt+=1024*10;
			}
			ft_video *stream = (ft_video *)get_aliase_ui_control("show_big_navi");
			bool b=true;
			//set_property_aliase_value("show_big_navi",&b);
			stream->update_text_from_image_buff(p,length);
			stream->set_visible(true);
			delete[] p;
			fclose(fp);
#else 
			//fseek()
			ft_video *stream = (ft_video *)get_aliase_ui_control("show_big_navi");
			//ft_base *pbase = (ft_base *)get_aliase_ui_control("show_big_navi");
			bool b = true;
			set_property_aliase_value("show_big_navi", &b);
			ifstream fin;
			fin.open("3.jpg", ios::binary);
			if (fin.is_open())
			{
				auto file_size = fin.tellg();
				fin.seekg(0, ios::end);
				file_size = fin.tellg() - file_size;
				printf("---file=%d\n",file_size);
				fin.seekg(0, ios::beg);
				char* pimage_buff = new char[file_size];
				fin.read(pimage_buff, file_size);
				stream->update_text_from_image_buff((u8*)pimage_buff, file_size);
				printf("update jpg:%s\n", "3.jpg");
				delete[] pimage_buff;
				fin.close();
			}
#endif
		}
		else if (key == key_enter)
		{
			//my_popWin.Pop_Window(em_key_ok, true);
			PageManager::getInstance()->currentDlgDealKey(EmKey(9), key_state_press);

		}
		else if (key == key_up)
		{
			//menu_ani.switch_to_next();
			PageManager::getInstance()->currentDlgDealKey(EmKey(3), key_state_press);
			//PageManager::getInstance()->currentDlgDealKey(EmKey(9), key_state_press);
		}
		else if (key == key_down)
		{
			//menu_ani.switch_to_pre();
			//printf("down\n");
			//indicate;
			PageManager::getInstance()->currentDlgDealKey(EmKey(4), key_state_press);
		}
		else if (key == key_right)
		{
			PageManager::getInstance()->currentDlgDealKey(EmKey(6), key_state_press);
		}
		else if (key == key_left)
		{
			PageManager::getInstance()->currentDlgDealKey(EmKey(5), key_state_press);
		}
#endif
		//printf("tmp=%d ", key);
	}

	//}
}


#endif
