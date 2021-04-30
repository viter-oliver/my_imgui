/*
* uart.h
*
*  Created on: 2019-12-3
*      Author: viter
*/
#pragma execution_character_set("utf-8")

#ifndef pop_win_h_
#define pop_win_h_
#include <string>
#include <functional>
#ifdef WIN32
#include <gl/gl3w.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#endif // WIN32
#include <glfw/glfw3.h>

#ifdef IMGUI_WAYLAND
#include "RpcDataDefine.h"
#endif

enum comman_base_data
{
	INVALID,
	NORMAL,
	ABNORMAL,
	SUSPICIOUS//suspicious
};

enum comman_telltale_sts
{
	LIGHT_HIDE,
	LIGHT_SHOW,
	LIGHT_BLINK_1HZ,
	LIGHT_BLINK_2HZ
};

#define CAR_DOOR_NUM 6
enum Em_PoPId
{
	em_none,                  //
	em_key_ok,                  //ok 按键
	em_car_door_left_front_open,
	em_car_door_left_back_open,
	em_car_door_right_front_open,
	em_car_door_right_back_open,
	em_car_door_engine_open,  //引擎盖未关
	em_car_door_esy_open,     //车尾箱未关
	em_coor_temp_height,      //水温过高
	em_oil_low,               //燃油低 
	em_main_safe_belt_no_suit,//主驾驶安全带未系
	em_sub_safe_belt_no_suit,//副安全带未系
	em_tirepress_check_fault,//胎压监测系统故障，请联系维修人员
	em_tirepressure_value_abnormal,   //胎压高
	//em_tirepressure_low,     //胎压低 
	//em_tire_temp_hight,     //胎温高
	//em_tire_temp_low,       //胎温低
	em_bluetooth_talk,		//蓝牙电话	
	em_brake_until_show_P_gear,     //请踩下刹车直至显示P档
	em_find_no_key_in_car,     //车内未发现钥匙
	em_switch_P_N_gear_bef_start,     //启车前请挂P/N档
	em_brake_bef_start,     //启动前请踩刹车踏板
	em_stop_switch_P_gear,     //熄火请挂P档
	em_max
};

enum Em_priority
{
	Em_priority_A,
	Em_priority_B,
	Em_priority_C,
	Em_priority_Max
};

enum Em_car_door
{
	em_left_front_door,
	em_left_rear_door,
	em_right_front_door,
	em_right_rear_door,
	em_engine_door,
	em_esy_door,
	em_car_door_max
};


enum Em_menu_first{
	em_menu_fault_list,
	em_menu_media,
	em_menu_setting,
	em_menu_tc,
	em_menu_max
};
enum Em_radio
{
	em_AM=0,
	em_FM=1,
};
typedef std::function<void(bool)>  HandleCallBack;


class MyPopWin
{
public:
	MyPopWin(){};
	~MyPopWin(){};
	//StructPopWin  pop_arr[em_max - 1] = { StructPopWin{} };

	Em_PoPId cur_id;
	bool visable;
	bool need_interupt{ false };
	bool need_open_car_door{ false };
	bool all_door_close{ false };

	bool car_door[em_car_door_max];
	void init_pop_win_data(void);
	void Pop_win_vec();
	void Pop_Window(Em_PoPId id, bool visable);
	bool isInBlueToothTalk(void);
	void hide_curren_win(void);
	void hide_win_by_id(Em_PoPId id);
	bool show_next_win(void);
	bool is_in_thirty_pop_min(Em_PoPId id);
	void get_one_car_door_data(Em_PoPId & id, bool & sts);
	bool is_one_pop_from_map_pop(void);
	bool deal_with_car_door_open(Em_PoPId id,bool sts);
	void deal_with_car_door_close(Em_PoPId id, bool sts);
	void update_car_door_data(Em_PoPId id, bool sts);
	bool is_car_door_open(void);
	bool is_car_door_id(Em_PoPId id);
	void send_car_door_fault_list(void);
	void active_timer_a_thirtymin(Em_PoPId id);
	void clear_pop_win_from_map_pop(Em_PoPId id);
	void clear_car_door_data_from_map_pop();

	//故障列表
	void send_to_fault_list(Em_PoPId id, bool sts);

	void add_fault_list(Em_PoPId id, bool sts);

};

#include "AppDef.h"
class Menu_admin
{
public:
	void hudong(float a, float b, bool b_next);
	void switch_to(float a, float b);
	void switch_to_menu(Em_menu_first id);
	void switch_to_pre(void);
	void switch_to_next(void);
	Em_main_menu getCurrentMenuId(void){ return cur_menu_id; };
	float start_value{0.0f};
	float end_value{0.0f};
	float value{ 0.0f };

	float k = 0.2f;
	float offsetb = 20.0f;

	bool is_next{ false };
	bool need_quick_ani{false};
	Em_main_menu cur_menu_id{ main_menu_media };

};

extern void register_adas_cmd_handl();
extern void register_main_view_page();
extern void KeyTest( int action );
extern void register_pop_win_cmd();
//extern void 
extern void KeyCallback(GLFWwindow*, int key, int, int action, int mods);
extern int count_fps;
#endif /* UART_H_ */
