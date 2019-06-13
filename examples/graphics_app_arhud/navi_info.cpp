#include "afg.h"
#include "af_bind.h"
#include "navi_info.h"
#include "msg_host_n.h"
#include "enum_txt_name0.h"
using namespace auto_future_utilities;
extern msg_host_n g_msg_host;
#define MAX_CONTENT_LEN 0x100
static char str_show[MAX_CONTENT_LEN] = { 0 };
static bool bvalue = false;
extern void print_buff(u8* pbuff, int len);
MSC_PACK_HEAD
void register_navi_cmd_handl()
{
	 g_msg_host.attach_monitor("navi state",[&](u8*pbuff,int len){
		 bvalue = *pbuff == 0;
		 set_property_aliase_value("active_navigation", &bvalue);
		
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
		simple_info_of_navi_guidance* psg=(simple_info_of_navi_guidance*)pbuff;
		pbuff += sizeof(simple_info_of_navi_guidance);
		static int  idx = 0;
		//printf("road_name_length=%d idx=%d\n",psg->road_name_length,idx++);
		if (psg->road_name_length>MAX_CONTENT_LEN-1)
		{
			psg->road_name_length =MAX_CONTENT_LEN - 1;
		}
		memcpy(str_show, pbuff, psg->road_name_length);
		str_show[psg->road_name_length] = '\0';

		set_property_aliase_value("next_road_name", str_show);

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
		 print_buff(pbuff, len);
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
		printf("number  of_lanes:%d\n", lane_num);
		float flane_num = lane_num;
		set_property_aliase_value("number_of_lanes", &flane_num);
		printf("number_of_lanes:%f\n", flane_num);
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
			en_interflow,en_tunnel,en_bridge,en_railway,en_turn_around,en_stone_fall,en_accident_prone_area,
			en_undefined,en_traffic_light_camera,en_illegal_camera,en_undefined1,en_watch_for_children,
			en_rough_road,en_road_narrows,en_village,en_undefined2,en_no_overtaking,en_sound_horn,en_retrograde_ban,
			en_no_occupation_of_bus_lanes,en_sharpe_turn_left,en_sharpe_turn_right,en_reverse_curve,en_winding_road,
			en_note_dangerous, en_up_steep_slope, en_dn_steep_slope,en_steep_mountain_road,en_beware_of_crosswind,
			en_slippery_road, en_embankment_road,en_unguarded_railway,en_guarded_railway,en_hump_road,en_long_decent,
			en_ferry,en_no_illegal_parking,en_no_illegal_lane_change,en_a_monitor,en_school,en_raiway_crossing,
			en_section_num
		};
		u8 atype = psection->assistant_type;
		u16 rdis = psection->remain_distance;
		bvalue = (atype == en_traffic_light_camera || atype == en_illegal_camera) && rdis > 0;
		set_property_aliase_value("camera1", &bvalue);
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
	 g_msg_host.attach_monitor("navigational line",[&](u8*pbuff,int len){
		u8 navi_state=*pbuff;
	});
	 g_msg_host.attach_monitor("destination",[&](u8*pbuff,int len){
	 	pbuff+=16;
		u8 des_name_len=*pbuff++;
		char* des_name=(char*)pbuff;
	});
	 
	 
}
