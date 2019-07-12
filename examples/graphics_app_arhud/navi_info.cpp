#include "afg.h"
#include "af_bind.h"
#include "navi_info.h"
#include "msg_host_n.h"
#include "enum_txt_name0.h"
#include "af_state_manager.h"
#include "af_timer.h"
#include "af_material.h"
using namespace auto_future;
using namespace auto_future_utilities;
extern msg_host_n g_msg_host;
extern u8 navi_state;
extern af_timer g_timer;
extern int cs_tar;
u8 navi_direct_state=en_navi_no_direction;
bool navi_start = false;
const char str_navi_start[13] = { 0xe5, 0xaf, 0xbc, 0xe8, 0x88, 0xaa, 0xe5, 0xbc, 0x80, 0xe5, 0xa7, 0x8b, 0 };//导航开始
const char str_navi_close[13]= { 0xe5, 0xaf, 0xbc, 0xe8, 0x88, 0xaa, 0xe5, 0x85, 0xb3, 0xe9, 0x97, 0xad, 0 };//导航关闭
const char str_navi_reroute[] = {0xe9,0x87,0x8d,0xe6,0x96,0xb0,0xe8,0xb7,0xaf,0xe5,0xbe,0x84,0xe8,0xa7,0x84,0xe5,0x88,0x92,0};//重新路径规划
const char str_navi_departure[] = { 0xe5, 0xb7, 0xb2, 0xe5, 0x81, 0x8f, 0xe7, 0xa6, 0xbb, 0xe5, 0xaf, 0xbc, 0xe8, 0x88, 0xaa, 0xe8, 0xb7, 0xaf, 0xe5, 0xbe, 0x84, 0 };//已偏离导航

#define MAX_CONTENT_LEN 0x100
static char str_show[MAX_CONTENT_LEN] = { 0 };
static bool bvalue = false;
extern void print_buff(u8* pbuff, int len);
static bool show_turn_right = false;
static bool show_turn_left = false;
static int activate_navi_tmid = -1;
MSC_PACK_HEAD
void register_navi_cmd_handl()
{
	activate_navi_tmid = g_timer.register_timer([&](int tick){
		if (tick==0)
		{
			bvalue = true;
			set_property_aliase_value("active_navigation", &bvalue);
			set_property_aliase_value("show_navi_info", &bvalue);
		}

	});
	 g_msg_host.attach_monitor("navi state",[&](u8*pbuff,int len){
		 bvalue = *pbuff == 0;
		 navi_state = bvalue;
		 if (bvalue)
		 {
			 memcpy(str_show, str_navi_start, sizeof(str_navi_start));
			 set_mp_text_uf("mt_sp", "text", "road_navi.png");
		 }
		 else
		 {
			 memcpy(str_show, str_navi_close, sizeof(str_navi_close));
			 set_mp_text_uf("mt_sp", "text", "road.png");
		 }
		 set_property_aliase_value("navi_start_info", str_show);
		 navi_start = bvalue;
		 if (navi_start)
		 {
			g_timer.active_timer(activate_navi_tmid, 24, 1);
		 }
		 
	});
	 g_msg_host.attach_monitor("navigational line",[&](u8*pbuff, int len){
		if (*pbuff==0)
		{
			memcpy(str_show, str_navi_departure, sizeof(str_navi_departure));
		}
		else
		{
			memcpy(str_show, str_navi_reroute, sizeof(str_navi_reroute));
			g_timer.active_timer(activate_navi_tmid, 24, 1);
		}
		set_property_aliase_value("navi_start_info", str_show);
		//navi_start = false;
		//set_property_aliase_value("active_navigation", &navi_start);
		//set_property_aliase_value("show_navi_info", &navi_start);

	 });
	 g_msg_host.attach_monitor("destination", [&](u8*pbuff, int len){
		 pbuff += 16;
		 u8 des_str_len = *pbuff;
		 pbuff++;
		 memcpy(str_show, pbuff, des_str_len);
		 str_show[des_str_len] = '\0';
		 set_property_aliase_value("navi_start_info", str_show);
		 //navi_start = false;
		 //set_property_aliase_value("active_navigation", &navi_start);
		 //set_property_aliase_value("show_navi_info", &navi_start);

	 });
	 reg_trans_handle("navi_lt2rt", [&](int from, int to){
		 bvalue = false;
		 set_property_aliase_value("show_navi_lt2rt", &bvalue);
		 set_property_aliase_value("show_right_bend", &bvalue);
		 show_turn_right = false;
	 });
	 reg_trans_handle("navi_rt2lt", [&](int from, int to){
		 bvalue = false;
		 set_property_aliase_value("show_navi_rt2lt", &bvalue);
		 set_property_aliase_value("show_left_bend", &bvalue);
		 show_turn_left = false;
	 });

	 g_msg_host.attach_monitor("simple info of navi guidance",[&](u8*pbuff,int len){
		//print_buff(pbuff,len);
		struct GNU_DEF simple_info_of_navi_guidance
		{
			u8 guidance_type;
			u32 remain_distance;
			u8 road_name_length;
			//char road_name_head;
		};
		enum em_navi_assistant
		{
			en_na_invalid,en_na_straight,en_na_turn_right_ft,en_na_turn_right,en_na_turn_right_rr,//0x04
			en_na_turn_around, en_na_turn_left_rr, en_na_turn_left, en_na_turn_left_ft,en_na_roundabout,//0x09
			en_na_roundabout_exit,en_na_bifucation_left,en_na_bifucation_right,en_na_left_local_line,en_na_far_left,//0x0f
			en_na_right_local_line, en_na_far_right, en_na_middle_line, en_na_bifucation_left_ic,en_na_bifucation_right_ic,//0x13
			en_na_tri_div_left, en_na_tri_div_right, en_na_tri_div_middle,en_na_starting_point,en_na_destination,//0x18
			en_na_way_point1, en_na_way_point2, en_na_way_point3,en_na_way_point4, en_na_enter_ferry,//0x1d
			en_na_exit_ferry, en_na_toll_station, en_na_bifucation_left_straight_ic,en_na_bifucation_right_straight_ic, en_na_bifucation_left_straight,//0x22
            en_na_bifucation_right_straight,en_na_tri_div_left_straight,en_na_tri_div_middle_straight,en_na_tri_div_right_straight,en_na_tri_div_left_ic,//0x27
			en_na_tri_div_middle_ic, en_na_tri_div_right_ic, en_na_ic_tri_div_left_straight,en_na_ic_tri_div_middle_straight,en_na_ic_tri_div_right_straight,//0x2c
			en_na_8dirs_left_straight,en_na_8dirs_right_straight, en_na_8dirs_far_left_straight,en_na_8dirs_middle_straight,en_na_8dirs_far_right_straight,//0x31
			en_na_8dirs_turn_left_left, en_na_8dirs_turn_left_right,en_na_8dirs_turn_left_far_left,en_na_8dirs_turn_left_middle,en_na_8dirs_turn_left_far_right,//0x36
			en_na_8dirs_turn_right_left,en_na_8dirs_turn_right_right,en_na_8dirs_turn_right_far_left,en_na_8dirs_turn_right_middle,en_na_8dirs_turn_right_far_right,//0x3b
			en_na_8dirs_ft_left_left,en_na_8dirs_ft_left_right, en_na_8dirs_ft_right_left, en_na_8dirs_ft_right_right,en_na_left_1st,//0x40
			en_na_left_2nd, en_na_left_3rd, en_na_left_4th, en_na_right_1st, en_na_right_2nd, //0x45
			en_na_right_3rd,en_na_right_4th, en_na_viaduct_straight, en_na_viaduct_left, en_na_viaduct_right, //0x4a
			en_na_no_viaduct_straight,en_na_no_viaduct_left,en_na_no_viaduct_right,en_na_dn_viaduct,en_na_dn_viaduct_left,//0x4f
			en_na_dn_viaduct_right,en_na_no_dn_vaiduct_straight, en_na_no_dn_vaiduct_left, en_na_no_dn_vaiduct_right,en_na_count//0x54
		};
		int navi_as_txt[en_na_count]=
		{
			en_na_straight_png, en_na_straight_png, en_na_turn_right_ft_png, en_na_turn_right_png, en_na_turn_right_rr_png,//0x04
			en_na_turn_around_png, en_na_turn_left_rr_png, en_na_turn_left_png, en_na_turn_left_ft_png, en_na_roundabout_png,//0x09
			en_na_roundabout_exit_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png,//0x0f
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png,//0x13
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_destination_png,//0x18
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png,//0x1d
			en_na_straight_png, en_na_toll_station_png, en_na_straight_png, en_na_straight_png, en_na_straight_png,//0x22
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png,//0x27
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png,//0x2c
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png,//0x31
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png,//0x36
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png,//0x3b
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png,//0x40
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, //0x45
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, //0x4a
			en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png, en_na_straight_png,//0x4f
			en_na_straight_png, en_na_no_dn_vaiduct_straight_png, en_na_straight_png, en_na_straight_png
		};
		simple_info_of_navi_guidance* psg=(simple_info_of_navi_guidance*)pbuff;
		float car_speed_s = (float)cs_tar*0.277;
		int remain_s = psg->remain_distance / car_speed_s;
		printf("cs_tar=%d km/h,car_speed=%f m/s remain_dis=%dm remain_s=%ds\n", cs_tar, car_speed_s,psg->remain_distance,remain_s);
		if (psg->guidance_type == en_na_turn_right)
		{
			if (remain_s<10&&!show_turn_right)
		    {
				show_turn_right = true;
				bvalue = true;
				set_property_aliase_value("show_right_bend", &bvalue);
				navi_direct_state = en_navi_direct_right;
				set_property_aliase_value("show_navi_lt2rt", &bvalue);
				play_trans("navi_lt2rt", 0, 1);
		    }
			
		}
		else
	    if (psg->guidance_type == en_na_turn_left)
		{
			if (remain_s < 10 && !show_turn_left)
			{
				show_turn_left = true;
				bvalue = true;
				set_property_aliase_value("show_left_bend", &bvalue);
				navi_direct_state = en_navi_direct_left;
				set_property_aliase_value("show_navi_rt2lt", &bvalue);
				play_trans("navi_rt2lt", 0, 1);
			}
		}
		else
		{
			navi_direct_state = en_navi_no_direction;
			bvalue=false;
			set_property_aliase_value("show_right_bend", &bvalue);
			set_property_aliase_value("show_left_bend", &bvalue);
			set_property_aliase_value("show_navi_rt2lt", &bvalue);
			set_property_aliase_value("show_navi_lt2rt", &bvalue);

		}
		navi_state = 1;
		pbuff += sizeof(simple_info_of_navi_guidance);
		static int  idx = 0;
		//printf("road_name_length=%d idx=%d\n",psg->road_name_length,idx++);
		if (psg->road_name_length>MAX_CONTENT_LEN-1)
		{
			psg->road_name_length =MAX_CONTENT_LEN - 1;
		}
		//print_buff(pbuff, psg->road_name_length);
		memcpy(str_show, pbuff, psg->road_name_length);
		str_show[psg->road_name_length] = '\0';

		set_property_aliase_value("next_road_name", str_show);
		memset(str_show, 0, psg->road_name_length);
		sprintf_s(str_show, MAX_CONTENT_LEN, "%dm", psg->remain_distance);
		set_property_aliase_value("road_distance", str_show);
		int guide_txt = navi_as_txt[psg->guidance_type];
		set_property_aliase_value("control_navi_guide", &guide_txt);
		if (!navi_start)
		{
			navi_start = true;
			set_property_aliase_value("active_navigation", &navi_start);
			set_property_aliase_value("show_navi_info", &navi_start);
		}

	});
	 g_msg_host.attach_monitor("assistant info of navi guidance",[&](u8*pbuff,int len){
		struct GNU_DEF assitant_info_of_navi_guidance
		{
			u8 assitant_type;
			u32 remain_distance_from_camera;
			u32 camera_speed;
		};
		assitant_info_of_navi_guidance* passitant_info_of_navi_guidance=(assitant_info_of_navi_guidance*)pbuff;
	});
	 g_msg_host.attach_monitor("lane info from navi",[&](u8*pbuff,int len){
		u8 lane_num=*pbuff++;
		enum en_lane_type
		{
			en_lane_straight,
			en_lane_left,
			en_lane_left_or_straight,
			en_lane_right,
			en_lane_right_or_straight,
			en_lane_left_turnround,
			en_lane_left_or_right,
			en_lane_left_or_right_or_straight,
			en_lane_right_turnround,
			en_lane_straight_or_left_turnround,
			en_lane_straight_or_right_turnround,
			en_lane_left_or_left_turnround,
			en_lane_right_or_right_turnround,
			en_lane_undefined,
			en_lane_non_optional,
			en_lane_type_num,
		};
		struct GNU_DEF lane_control 
		{
			int invalid_lane_type_txt_id;
			int valid_lane_type_txt_id;
		};
		static const lane_control lcontrol[en_lane_type_num] = {
			{en_navi_straight1_png,en_navi_straight_png},//en_lane_straight
			{ en_navi_turn_left1_png, en_navi_turn_left_png },//en_lane_left
			{ en_navi_turn_left_straight1_png, en_navi_turn_left_straight_png },//en_lane_left_or_straight
			{ en_navi_turn_right1_png, en_navi_turn_right_png },//en_lane_right
			{ en_navi_turn_right_straight1_png, en_navi_turn_right_straight_png },//en_lane_right_or_straight
			{ en_navi_left_around1_png, en_navi_left_around_png},//en_lane_left_turnround
			{ en_navi_left_right1_png, en_navi_left_right_png},//en_lane_left_or_right
			{ en_navi_straight_left_right1_png, en_navi_straight_left_right_png},//en_lane_left_or_right_or_straight
			{ en_navi_turn_right_around1_png, en_navi_turn_right_around_png },//en_lane_right_turnround
			{ en_navi_straight_left_around1_png, en_navi_straight_left_around_png },//en_lane_straight_or_left_turnround
			{ en_navi_turn_right_straight_around1_png, en_navi_turn_right_straight_around_png },//en_lane_straight_or_right_turnround
			{ en_navi_turn_left_or_around1_png, en_navi_turn_left_or_around_png},//en_lane_left_or_left_turnround
			{ en_navi_turn_right_or_around1_png, en_navi_turn_right_or_around_png },//en_lane_right_or_right_turnround
			{ en_navi_undefine_png, en_navi_undefine_png},//en_lane_undefined
			{ en_navi_prohibition_png, en_navi_prohibition_png},//en_lane_non_optional

		};
		struct lane_info
		{
			u8 lane_status:1;
			u8 lane_type:7;
		};
		if (lane_num>8)
		{
			lane_num = 8;
		}
		float flane_num = lane_num;
		set_property_aliase_value("number_of_lanes", &flane_num);
		
		for (int ix = 0; ix < lane_num;ix++)
		{
			lane_info* plinfo = (lane_info*)pbuff++;
			sprintf_s(str_show, MAX_CONTENT_LEN,"control_lane%d", ix);
			u8 lane_type = plinfo->lane_type;
			if (lane_type>en_lane_non_optional)
			{
				lane_type = en_lane_undefined;
			}
			int txt_id = plinfo->lane_status ? lcontrol[lane_type].invalid_lane_type_txt_id : lcontrol[lane_type].valid_lane_type_txt_id;
			set_property_aliase_value(str_show, &txt_id);
			
		}
	});
	 g_msg_host.attach_monitor("car speed from navi",[&](u8*pbuff,int len){
		u8 car_speed=*pbuff;
	});
	 g_msg_host.attach_monitor("special section",[&](u8*pbuff,int len){
		struct GNU_DEF special_section
		{
			u8 assistant_type;
			u16 remain_distance;
		};
		special_section* psection=(special_section*)pbuff;
		enum en_section_type
		{
			en_interflow,en_tunnel,en_bridge,en_railway,en_turn_around,en_steep_slope,en_stone_fall,en_accident_prone_area,
			en_undefined,en_traffic_light_camera,en_illegal_camera,en_undefined1,en_watch_for_children,
			en_rough_road,en_road_narrows,en_village,en_undefined2,en_no_overtaking,en_sound_horn,en_retrograde_ban,
			en_no_occupation_of_bus_lanes,en_sharpe_turn_left,en_sharpe_turn_right,en_reverse_curve,en_winding_road,
			en_note_dangerous, en_up_steep_slope, en_dn_steep_slope,en_steep_mountain_road,en_beware_of_crosswind,
			en_slippery_road, en_embankment_road,en_unguarded_railway,en_guarded_railway,en_hump_road,en_long_decent,
			en_ferry,en_no_illegal_parking,en_no_illegal_lane_change,en_a_monitor,en_school,en_drive_with_caution,
			en_section_num
		};
		int icon_navi_note[en_section_num] =
		{
			en_nt_interflow_png, en_nt_tunnel_png, en_nt_bridge_png, en_nt_guarded_railway_png, en_nt_turn_around_png,
			en_nt_steep_slope_png, en_nt_stone_fall_png, en_nt_accident_prone_area_png,			en_navi_undefine_png,
			en_nt_a_monitor_png, en_nt_a_monitor_png, en_navi_undefine_png, en_nt_watch_for_children_png,
			en_nt_rough_road_png, en_nt_road_narrows_png,en_nt_village_png,en_navi_undefine_png, en_nt_no_overtaking_png, 
			en_nt_sound_horn_png, en_nt_retrograde_ban_png,en_nt_bus_lanes_png, en_nt_sharpe_turn_left_png, 
			en_nt_sharpe_turn_right_png, en_nt_reverse_curve_png, en_nt_winding_road_png,en_nt_note_dangerous_png,
			en_nt_up_steep_slope_png, en_nt_dn_steep_slope_png, en_nt_steep_mountain_road_png, en_nt_be_ware_of_crosswind_png,
			en_nt_slippery_road_png, en_nt_embankment_road_png, en_nt_unguarded_railway_png, en_nt_guarded_railway_png,
			en_nt_hump_road_png, en_nt_long_decent_png,en_nt_ferry_png,en_nt_no_illegal_parking_png, 
			en_nt_no_overtaking_png, en_nt_a_monitor_png, en_nt_school_png, en_nt_drive_with_caution_png
		};
		u8 atype = psection->assistant_type;
		if (atype >= en_section_num)
		{
			atype = en_undefined;
		}
		u16 rdis = psection->remain_distance;
		bvalue =  rdis > 0;
		set_property_aliase_value("show_navi_note", &bvalue);
		set_property_aliase_value("control_navi_note", &icon_navi_note[atype]);
	    
		/*static const char* road_type_name[0x30]={
			"合流","隧道","桥梁","铁道","转弯","陡坡","落石","事故多发区","未定义","交通信号灯摄像头",
			"违章摄像头","未定义","注意儿童","路面不平","道路变窄","前面村庄","未定义","禁止超车","请鸣喇叭","禁止逆行",
			"禁止占用公交车道","向左急转弯","向右急转弯","反向弯路","连续弯路","注意危险","上陡坡","下陡坡","傍山险路","注意横风",
			"易滑路段","堤坝路","无人看守铁路","有人看守铁路","驼峰路","连续下坡","渡口","禁止违章停车","禁止违章换道","有监控",
			"学校","前方铁道路口，请小心驾驶",};*/
	});
	 
	 g_msg_host.attach_monitor("limited speed",[&](u8*pbuff,int len){
		u8 limited_speed=*pbuff;
		sprintf_s(str_show, MAX_CONTENT_LEN, "%d", limited_speed);
		set_property_aliase_value("speed_limited", str_show);
		pbuff++;
		u16*premain_distance=(u16*)pbuff;
		bvalue = *premain_distance > 0;
		set_property_aliase_value("show_limited_speed", &bvalue);
	});
	 g_msg_host.attach_monitor("distance from destination",[&](u8*pbuff,int len){
		u32* premain_distance=(u32*)pbuff;
		u32 remain_dis = *premain_distance / 1000;
		sprintf_s(str_show, MAX_CONTENT_LEN, "%d", remain_dis);
		set_property_aliase_value("value_remain_mileage", str_show);
		pbuff+=4;
		u32* premain_time=(u32*)pbuff;
		u32 remain_time = *premain_time / 60;
		sprintf_s(str_show, MAX_CONTENT_LEN, "%d", remain_time);
		set_property_aliase_value("value_remain_time", str_show);

	});
	 g_msg_host.attach_monitor("current road",[&](u8*pbuff,int len){
		u8 name_lenth=*pbuff;
		pbuff++;
		char* cur_road_name=(char*)pbuff;
	});
	 g_msg_host.attach_monitor("signal of gps",[&](u8*pbuff,int len){
		u8 gps_signal=*pbuff;
	});
	 g_msg_host.attach_monitor("compass", [&](u8*pbuff, int len){
		 u8 compass = *pbuff;
		 float angle = (float)compass * 2.f / 45.0;
		 set_property_aliase_value("navi_compass", &angle);
	 });
	 
	 g_msg_host.attach_monitor("navigational line",[&](u8*pbuff,int len){
		u8 navi_state=*pbuff;
	});
	 g_msg_host.attach_monitor("destination",[&](u8*pbuff,int len){
	 	pbuff+=16;
		u8 des_name_len=*pbuff++;
		char* des_name=(char*)pbuff;
	});
	 
	 
}
