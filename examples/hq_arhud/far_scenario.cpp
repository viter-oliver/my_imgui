
#include "far_scenario.h"
#include "msg_host_n.h"
#include "af_bind.h"
#include "af_primitive_object.h"
#include "af_material.h"
#include "af_state_manager.h"
#include "enum_txt_name0.h"
#include "af_playlist_group.h"
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
#include "common_functions.h"
//#include <cmath>

#ifndef PI
#define PI 3.1415926545
#endif
static const double half_pi = PI / 2;
#include <math.h>
#include <GLFW/glfw3.h>
using namespace chrono;
using namespace auto_future_utilities;
using namespace auto_future;
using namespace std;
extern msg_host_n g_msg_host;
//extern msg_host_n g_mh_pc;
#define g_mh_pc g_msg_host 
extern af_timer g_timer;
ft_image_file* g_image_show;
ft_material_3d *pfcw_p,*pfcw_v,*psacc,*parrow0,*parrow1,*parrow2;
ft_image_play *pturn_left,*pturn_right;

ps_mtl mtl_lane,mtl_sacc;
//float lc2=0.000004f,rc2=-0.000004f;
//float g_c2=lc2;
float c2u=0.0000005f;
int tmid_cave_lnae=0,tmid_lane_ldw=0,tmid_tsr=0,tmid_speed=0;
enum color_md
{
	en_clr_day,
	en_clr_night,
	en_clr_snow,
	en_clr_cnt
};
u8 theme_id;

enum en_lane_state
{
	en_lane_off,
	en_lane_on,
	en_lane_change,
	en_lane_ldw,
	en_lane_num,
};
u8 llane_status_id=en_lane_off,rlane_status_id=en_lane_off;
const char* lane_txt_name[en_clr_cnt][en_lane_num]=
{
{"lane_d.png","lane_n_day.png","lane_change_day.png","lane_LDW_day.png"},
{"lane_d.png","lane_n_night.png","lane_change_night.png","lane_LDW_day.png"},
{"lane_d.png","lane_n_snow.png","lane_change_snow.png","lane_LDW_day.png"},
};
bool be_show=false;
const char* lane_mt="road";

const char* arrow_txt_name[en_clr_cnt]={"arrow.png","arrow_night.png","arrow_snow.png"};
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
char*  txt_left_wing[en_clr_cnt]={"left_wing.png","night_left_wing.png","snow_left_wing.png"};
char*  txt_right_wing[en_clr_cnt]={"right_wing.png","night_right_wing.png","snow_right_wing.png"};
void thame_change()
{
	set_mp_text_uf(lane_mt, "llane", lane_txt_name[theme_id][llane_status_id]);
	set_mp_text_uf(lane_mt, "rlane", lane_txt_name[theme_id][rlane_status_id]);
	set_mp_text_uf("obs0","text",arrow_txt_name[theme_id]);
	set_property_aliase_value("txt_left_wing", txt_left_wing[theme_id]);
	pturn_left->link();
	set_property_aliase_value("txt_right_wing", txt_right_wing[theme_id]);
	pturn_right->link();
	
}
enum lane_type
{
	en_lane_dashed,
	en_lane_solid,
	en_lane_none,
	en_lane_road_edge,
	en_lane_double_lane_mark,
	en_lane_bott_dots,
	en_lane_invalid,
};

enum
{
	en_model_error,
	en_linear_model,
	en_parabolic_model,
	en_3rd_degree_model,
};

enum en_obj_type
{
	en_invalid,
	en_car,
	en_truck,
	en_motor_bike,
	en_bicycle,
	en_pedestran,
	en_genral_objcet,
	en_animal,
	en_unsued,
	en_unused1,
	en_unused2,
	en_obstacle_type_num
};
enum en_motion_status
{
       en_ms_invalid,
	en_ms_unknown,
	en_ms_moving,
	en_ms_stationary,
	en_ms_stopped,
	en_ms_moving_slowly,
	en_ms_reserved,
	en_ms_reserved2,
	en_ms_cnt
};
enum en_motion_pattern
{
       en_mt_invalid,
	en_mt_undefined,
	en_passing,
	en_passing_in,
	en_passing_out,
	en_close_cut_in,
	en_moving_in,
	en_moving_out,
	en_crossing,
	en_LTAP,
	en_RATP,
	en_moving,
	en_preceeding,
	en_on_coming,
	en_mt_reserved1,
	en_mt_reserved2,
	en_mt_cnt
};
#define MAX_OBJ_CNT 10
struct
{
	u8 obj_id;
	u8 obj_type:4;
	u8 obj_motion_pattern:4;
	u8 obj_motion_status:4;
	u8 reserved:4;
	s16 obj_TTC;
	float xpos_near;
	float ypos_near;
	float width;
	float xspeed;
	float yspeed;
	float length;
	float height;
} g_obj_tbs[MAX_OBJ_CNT]={0};
static u8 obj_vechicle_cnt=0,obj_pedestrain_cnt=0,obj_cipv_id=0;
u8 object_cnt()
{
	return obj_vechicle_cnt+obj_pedestrain_cnt;
}
const char* fcw_p_mark[3]={"mark_fcw_level1_p.png","mark_fcw_level2_p.png","mark_fcw_AEB_p.png"};
const char* fcw_v_mark[3]={"mark_fcw_level1_v.png","mark_fcw_level2_v.png","mark_fcw_AEB_v.png"};
void obj_fcw_hide()
{
	pfcw_v->set_visible(false);
	pfcw_p->set_visible(false);
	psacc->set_visible(false);
}
enum en_cipv_prioprity
{
	en_cipv_off,
	en_cipv_aeb,
	en_cipv_fcw,
	en_cipv_sacc,
	//en_adas_ldw,
};
u8 cipv_status;
bool g_be_acc=false;
bool g_be_lca=false;
void  obj_fcw_handle(u8 level)
{
	if(level==0||level==3)
	{
		obj_fcw_hide();
		return;
	}
	u8 level_id=level-1;
	for(u8 idx=0;idx<MAX_OBJ_CNT;idx++)
	{
		if(g_obj_tbs[idx].obj_id==obj_cipv_id)
		{
			switch(g_obj_tbs[idx].obj_type)
			{
				case en_car:
				case en_truck:
				case en_motor_bike:
					{
						const float w_h_scale=31.f/137.f;
						auto width_obj=g_obj_tbs[idx].width*1000+200.f;
						auto height_obj=width_obj*w_h_scale;
						pfcw_v->set_visible(true);
						pfcw_v->set_trans_scalex(width_obj);
						pfcw_v->set_trans_scaley(height_obj);
						pfcw_v->set_trans_tly(-height_obj);
						auto xoffset=g_obj_tbs[idx].ypos_near*1000-width_obj/2;
						pfcw_v->set_trans_tlx(xoffset);
						auto zoffset=g_obj_tbs[idx].xpos_near*1000;
						pfcw_v->set_trans_tlz(zoffset);
						printf("ypos=%f,xpos=%f\n",g_obj_tbs[idx].ypos_near,g_obj_tbs[idx].xpos_near);
						printf("fcw width_obj:%f,height_obj:%f,xoffset:%f,zoffset:%f\n",width_obj,height_obj,xoffset,zoffset);
						set_mp_text_uf("fcw_v", "text", fcw_v_mark[level_id]);
					}
					break;
				case en_bicycle:
				case en_pedestran:
				case en_genral_objcet:
				case en_animal:
					{
						const float w_h_scale=60.f/68.f;
						auto width_obj=g_obj_tbs[idx].width*1000;
						auto height_obj=width_obj*w_h_scale;
						pfcw_p->set_visible(true);
						pfcw_p->set_trans_scalex(width_obj);
						pfcw_p->set_trans_scaley(height_obj);
						auto obj_height=g_obj_tbs[idx].height*1000;
						pfcw_p->set_trans_tly(obj_height);
						auto xoffset=g_obj_tbs[idx].ypos_near*1000-width_obj/2;
						pfcw_p->set_trans_tlx(xoffset);
						auto zoffset=g_obj_tbs[idx].xpos_near*1000;
						pfcw_p->set_trans_tlz(zoffset);
						set_mp_text_uf("fcw", "text", fcw_p_mark[level_id]);		
				       }
					break;
				default:
					printf("fcw invalid obj[%d] type:%x,obj_cipv_id:%x\n",idx,g_obj_tbs[idx].obj_type,obj_cipv_id);
					break;
			}
			return;
		}
	}
	printf("obj_cipv_id:%d is invalid\n",obj_cipv_id);
}

void  obj_AEB_handle()
{
	for(u8 idx=0;idx<MAX_OBJ_CNT;idx++)
	{
		if(g_obj_tbs[idx].obj_id==obj_cipv_id)
		{
			switch(g_obj_tbs[idx].obj_type)
			{
				case en_car:
				case en_truck:
				case en_motor_bike:
					{
						const float w_h_scale=31.f/137.f;
						auto width_obj=g_obj_tbs[idx].width*1000+200.f;
						auto height_obj=width_obj*w_h_scale;
						pfcw_v->set_visible(true);
						pfcw_v->set_trans_scalex(width_obj);
						pfcw_v->set_trans_scaley(height_obj);
						pfcw_v->set_trans_tly(-height_obj);
						auto xoffset=g_obj_tbs[idx].ypos_near*1000-width_obj/2;
						pfcw_v->set_trans_tlx(xoffset);
						auto zoffset=g_obj_tbs[idx].xpos_near*1000;
						pfcw_v->set_trans_tlz(zoffset);
						set_mp_text_uf("fcw_v", "text", fcw_v_mark[1]);
						pfcw_p->set_visible(true);
						const float tp_w_h_scale=76.f/110.f;
						auto top_width_obj=g_obj_tbs[idx].width*1000;
						auto top_height_obj=top_width_obj*tp_w_h_scale;
						pfcw_p->set_trans_scalex(top_width_obj);
						pfcw_p->set_trans_scaley(top_height_obj);
						auto obj_height=g_obj_tbs[idx].height*1000-top_height_obj;
						pfcw_p->set_trans_tly(obj_height);
						xoffset=g_obj_tbs[idx].ypos_near*1000-top_width_obj/2;
						pfcw_p->set_trans_tlx(xoffset);
						pfcw_p->set_trans_tlz(zoffset);
						set_mp_text_uf("fcw", "text", fcw_v_mark[2]);
					}
					break;
				case en_bicycle:
				case en_pedestran:
				case en_genral_objcet:
				case en_animal:
					{
						const float w_h_scale=60.f/68.f;
						auto width_obj=g_obj_tbs[idx].width*1000;
						auto height_obj=width_obj*w_h_scale;
						pfcw_p->set_visible(true);
						pfcw_p->set_trans_scalex(width_obj);
						pfcw_p->set_trans_scaley(height_obj);
						auto obj_height=g_obj_tbs[idx].height*1000;
						pfcw_p->set_trans_tly(obj_height);
						auto xoffset=g_obj_tbs[idx].ypos_near*1000-width_obj/2;
						pfcw_p->set_trans_tlx(xoffset);
						auto zoffset=g_obj_tbs[idx].xpos_near*1000;
						pfcw_p->set_trans_tlz(zoffset);
						set_mp_text_uf("fcw", "text", fcw_p_mark[2]);		
				       }
					break;
				default:
					printf("invalid obj[%d] type:%d\n",idx,g_obj_tbs[idx].obj_type);
					break;
			}
			return;
		}
	}
	printf("obj_cipv_id:%d is invalid\n",obj_cipv_id);
}
float HAD_TimeGapSetting=0;
void obj_sacc_handle()
{
	if(g_be_acc&&g_be_lca)
	{
		bool be_show=false;
		set_property_aliase_value("show_acc", &be_show);
		pfcw_v->set_visible(true);
		psacc->set_visible(true);
		set_mp_text_uf("fcw_v", "text", "mark_ACC_vehicle.png");
		mtl_sacc->set_value("level",&HAD_TimeGapSetting,1);
		for(u8 idx=0;idx<MAX_OBJ_CNT;idx++)
		{
			if(g_obj_tbs[idx].obj_id==obj_cipv_id)
			{
				const float w_h_scale=31.f/137.f;
				auto width_obj=g_obj_tbs[idx].width*1000+200.f;
				auto height_obj=width_obj*w_h_scale;
				pfcw_v->set_trans_scalex(width_obj);
				pfcw_v->set_trans_scaley(height_obj);
				pfcw_v->set_trans_tly(-height_obj);
				auto xoffset=g_obj_tbs[idx].ypos_near*1000-width_obj/2;
				pfcw_v->set_trans_tlx(xoffset);
				auto zoffset=g_obj_tbs[idx].xpos_near*1000;
				pfcw_v->set_trans_tlz(zoffset);
				const float sacc_w=321.f,sacc_h=204.f;
				float sacc_w_scalue=width_obj/sacc_w;
				
				psacc->set_trans_scalex(sacc_w_scalue);
				float s_xoffset=g_obj_tbs[idx].ypos_near*1000;
				psacc->set_trans_tlx(s_xoffset);
				float s_zoffset=zoffset-100;
				psacc->set_trans_tlz(s_zoffset);
				return;
			}
		}
		//g_timer.deactive_time_ex(tmid_speed);
	}
	else
	{
		set_property_aliase_value("show_acc", &g_be_acc);
		pfcw_v->set_visible(false);
		psacc->set_visible(false);
	}
	
}

enum en_proj
{
	en_unsatisfied,
	en_satisfied,
	en_object_loced,
};
enum en_navi_dir_status
{
	navi_invalid,
	navi_dir_straight,
	navi_dir_turn,
	navi_dir_round_about,
	navi_dir_other,
};
enum en_lane_shape_status
{
	lane_undetected,
	lane_straight,
	lane_curve,
	lane_any,//don't care?
};
enum en_steering_angle
{
	sta_any,
	sta_invalid,
	sta_turn,
	sta_lane_change,
	sta_no_op,
};
enum en_steering_angle_a
{
	sta_a_greater_than_cali_value,
	sta_a_less_than_cali_value,
};
enum en_turn_radius_curvature_status
{
	trc_r_lessthan_cali_value,
	trc_r_greaterthan_cali_value,
	trc_R_r_lessthan_cali_value,
	trc_R_r_greaterthan_cali_value,
};

extern char str_show[MAX_CONTENT_LEN];

enum en_signal_status
{
	signal_vailable,
	signal_lost,
};
enum en_HAD_WarningCmd_LDW
{
	ldw_inactive,
	LeftWarning,
	RightWarning,
	wn_reserved,
};

u8 lane_ldw_status=ldw_inactive;

enum en_HAD_WarningCmd_TSR
{
	tsr_inactive,
	tsr_warning1,
	tsr_warning2,
	tsr_reserved,
};
u8 HAD_WarningCmd_TSR;
const float base_value = -400.f;
vector<paire_value<float>> direc_trans0_seg_detas = {
     { 7, -1450.f - base_value },
     { 8, -2100.f - base_value },
     { 9, 694.f - base_value },
     { 10, 1000.f - base_value },
     { 11, -500.f - base_value },
     { 12, -600.f - base_value },
     { 17, -1600.f - base_value },
     { 14, -500.f - base_value },
     { 15, 550.f - base_value },
     { 16, 600.f - base_value }
};
vector<paire_value<float>> direc_trans1_seg_detas = {
     { 7, -1400.f - base_value },
     { 8, -2000.f - base_value },
     { 9, 600.f - base_value },
     { 10, 800.f - base_value },
     { 11, -500.f - base_value },
     { 12, -1150.f - base_value },
     { 18, -2150.f - base_value },
     { 15, -500.f - base_value },
     { 16, 480.f - base_value },
     { 17, 1150.f - base_value }
};
vector<paire_value<float>> direc_trans2_seg_detas = {
     { 7, -1100.f - base_value },
     { 8, -1700.f - base_value },
     { 9, 210.f - base_value },
     { 10, 600.f - base_value },
     { 11, -500.f - base_value },
     { 12, -1500.f - base_value },
     { 18, -2500.f - base_value },
     { 15, -500.f - base_value },
     { 16, 900.f - base_value },
     { 17, 1500.f - base_value }
};
float lc0=0,lc1=0,lc2=0,rc0=0,rc1=0,rc2=0;
bool lanec[4]={false,false,false,false};
float arrow_z0=6000,arrow_z1=80000,arrow_z2=9000;
float arrow_x0 = base_value, arrow_x1 = base_value, arrow_x2 = base_value;
void calcu_arrows_transx()
{
	if(lanec[0]&&lanec[1]&&lanec[2]&&lanec[3])
	{
		float left_x0=lc2*arrow_z0*arrow_z0+lc1*arrow_x0+lc0;
		float right_x0=rc2*arrow_z0*arrow_z0+rc1*arrow_x0+rc0;
		arrow_x0=(left_x0+right_x0)*0.5f-250.f;
		parrow0->set_trans_tlx(arrow_x0);
		
		float left_x1=lc2*arrow_z1*arrow_z1+lc1*arrow_x1+lc0;
		float right_x1=rc2*arrow_z1*arrow_z1+rc1*arrow_x1+rc0;
		arrow_x1=(left_x1+right_x1)*0.5f-250.f;
		parrow1->set_trans_tlx(arrow_x1);
		
		float left_x2=lc2*arrow_z2*arrow_z2+lc1*arrow_x2+lc0;
		float right_x2=rc2*arrow_z2*arrow_z2+rc1*arrow_x2+rc0;
		arrow_x2=(left_x2+right_x2)*0.5f-250.f;
		parrow2->set_trans_tlx(arrow_x2);
		lanec[0]=lanec[1]=lanec[2]=lanec[3]=false;
	}
	
	
}
enum navi_dir_play
{
     ndir_no_play,
     ndir_turn_left_front,
     ndir_turn_left,
     ndir_left_turning_round,
     ndir_turn_right_front,
     ndir_turn_right,
     ndir_right_turning_round,
     ndir_lane_switch_left,
     ndir_lane_switch_right,

};
int pre_turn_count = 0;
bool be_visble = false;
struct navi_dir_logic
{
	u8  satisfied_projection:2;
	u8  navi_dir_status:3;
	u8  lane_shape_status:3;
	u8  steering_angle:3;
	u8  steering_angle_a:1;
	u8 turn_radius_curvature_status:4;
};

class arrow_turn_control
{
     float radious;
     float linear_dis;
     int duration;
     float max_angle;
     float turn_scale = { 0 };
     steady_clock::time_point tm_st;
     ImVec2 prev_pos = { 0.f, 0.f };
     float sum_dis = { 0.f };
public:
     arrow_turn_control( float rd, float dt,float ldis,float mag=180.f) 
          :radious( rd ), duration( dt ),linear_dis(ldis),max_angle(mag) {}
     void active()
     {
          tm_st = steady_clock::now();
          sum_dis = 0.f;
          prev_pos = { 0, 0 };
     }
     void calcu( float&xoff_set, float&z_offset, float& angle )
     {
          auto curt=steady_clock::now();
          int delta = chrono::duration_cast<chrono::duration<int, std::milli>>( curt - tm_st ).count();
          if( delta > duration )
          {
               active();
               return;

          }
          angle = (float)delta*max_angle/(float)duration;
          auto radian = PI*angle / 180.f;
          z_offset = radious* sin( radian );
          xoff_set = radious*( 1 - cos( radian ) );
     }
     void set_turn_scale( float tscale)
     {
          turn_scale = tscale;
     }
     bool calcu2( float&xoff_set, float&z_offset, float& angle,float& delta_dis,float& scale )
     {
          auto curt=steady_clock::now();
          int delta = chrono::duration_cast<chrono::duration<int, std::milli>>( curt - tm_st ).count();
          if( delta > duration )
          {
               active();
               return false;

          }
          scale=(float)delta/(float)duration;
          angle = scale*max_angle;
          xoff_set = ( radious + radious )*scale;
          auto dd_unit = xoff_set - radious;
          z_offset = 80000.f-0.02f * dd_unit*dd_unit;
          delta_dis = scale*linear_dis;
          return true;
     }
     bool calcu3( float&xoff_set, float&z_offset, float& angle,float& delta_dis,float& scale )
     {
          auto curt=steady_clock::now();
          int delta = chrono::duration_cast<chrono::duration<int, std::milli>>( curt - tm_st ).count();
          if( delta > duration )
          {
               active();
               return false;

          }
          scale=(float)delta/(float)duration;
          delta_dis = scale*linear_dis;

          angle = scale*max_angle;
          auto squre = [&]( float su )
          {
               return su*su;
          };
          auto equation0 = [&]( float x )
          {
               float z=80000.f - 0.02f * squre( x - radious );
               return z;
          };
          auto eg_xoff = 2 * radious*turn_scale;
          auto eg_zoff = equation0( eg_xoff );

          auto equation_z = [&]( float x )
          {
               float z = 80000.f - eg_zoff - 0.02f*squre( x - radious + eg_xoff );
               return z;
          };

          auto equation_dz_dx = [&]( float x )
          {
               float dz_dx = -0.04*( x - radious + eg_xoff );
               return dz_dx;
          };
          auto dz_dx0 = equation_dz_dx( 0.f );
          float angle_turn = atanf( dz_dx0 ) -PI*0.5f;
          //printf( "ange_turn=%f\n", angle_turn );
          xoff_set = ( radious + radious )*scale;// *cos( angle_turn );

          auto z_of = equation_z( xoff_set );
          ImVec2 pt0 = { xoff_set, z_of }, pt1 = { 0, 0 };
          ImVec2 des = rotate_point_by_zaxis( pt0, angle_turn, pt1 );
          xoff_set = des.x;
          z_offset = des.y;
          
          return true;
     }
     bool calcu4( float&xoff_set, float&z_offset, float& angle, float& delta_dis, float& scale )
     {
          auto curt = steady_clock::now();
          int delta = chrono::duration_cast<chrono::duration<int, std::milli>>( curt - tm_st ).count();
          if( delta > duration )
          {
               active();
               return false;

          }
          scale = (float)delta / (float)duration;
          delta_dis = scale*linear_dis;
          auto tn_dis = delta_dis ;
          auto dis_delta = tn_dis - sum_dis;
          sum_dis = tn_dis;
          auto squre = [&]( float su )
          {
               return su*su;
          };
          auto dis_delta_sq = squre( dis_delta );

          auto pt_sq_length = [&]( ImVec2& pt0, ImVec2& pt1 )
          {
               auto sq_dis = squre( pt1.x - pt0.x ) + squre( pt1.y - pt0.y );
          };

          auto equation0 = [&]( float x )
          {
               float z = 80000.f - 0.02f * squre( x - radious );
               return z;
          };

          auto equation_dz_dx = [&]( float x )
          {
               float dz_dx = -0.04*( x - radious );
               return dz_dx;
          };          
          
          auto find_pt = [&]( ImVec2& base_bt, ImVec2& des_pt )
          {
               auto dz_dx_base = equation_dz_dx( base_bt.x );
               auto angle_base = atanf( dz_dx_base);
               auto x_delta=dis_delta*cos( angle_base );
               des_pt.x= base_bt.x + x_delta;
               des_pt.y = equation0( des_pt.x );
          };
          ImVec2 next_pos;
          find_pt( prev_pos, next_pos );
          prev_pos = next_pos;
          xoff_set = next_pos.x;
          z_offset = next_pos.y;
          return true;
     }
};
arrow_turn_control arrow_cc(2000.f, 2000.f,50000.f);
static float aw_x = 0, aw_z2 = 150000.f, aw_z1 = 100000.f, aw_z0 = 50000.f, aw_zh=0.f,aw_a = -15.f;
static const float tga = 6.f / 500.f;
static bool be_a_moving = false;
static float g_turn_scale = 0;


static const int arrow_cont = 4;
ft_material_3d* parrows[ arrow_cont ];
class arrow_animation_control
{
     enum motion_status
     {
          en_forward,
          en_turning,
          en_finished,
     };
     float speed = 30;//毫米/毫秒
     float base_z = 150000.f;
	float radious;
     float top_curvature;
     float max_zoff;
     float finish_scale = {1.f};
     struct aw_unit 
     {
          ft_material_3d* parw;
          motion_status on_going;
          ImVec2 point_pre;
     };
     vector<aw_unit> varrow;
     steady_clock::time_point tm_st;
     float squre( float su )
     {
          return su*su;
     }
     float pt_sq_length( ImVec2& pt0, ImVec2& pt1 )
     {
          auto sq_dis = sqrt( squre( pt1.x - pt0.x ) + squre( pt1.y - pt0.y ) );
          return sq_dis;
     }
     float  equation0 ( float x )
     {
          float z = max_zoff - top_curvature * squre( x - radious );
          return z;
     };
     float equation_dz_dx( float x )
     {
          float dz_dx = -2*top_curvature*( x - radious );
          return dz_dx;
     };
     float equation_finish( float x )
     {
          auto x0 = finish_scale * 2 * radious;
          auto az0 = equation_dz_dx( x0 );
          auto bz = -az0*x0;
          auto zoff = az0*x + bz;
          return zoff;
     }
public:
     arrow_animation_control(float rd,float t_c)
          :radious(rd), top_curvature(t_c)
     {
          max_zoff = top_curvature*radious*radious;
     }
     void active()
     {
          tm_st = steady_clock::now();
     }
     void add_arrow( ft_material_3d* paw )
     {
          aw_unit tn = { paw, en_forward, { 0, 0 } };
          varrow.emplace_back( tn );
     }
     void set_finish_scale( float fs )
     {
          finish_scale = fs;
     }
     void handle()
     {
          auto curt = steady_clock::now();
          int time_delta = chrono::duration_cast<chrono::duration<int, std::milli>>( curt - tm_st ).count();
          tm_st = curt;
          float dis = time_delta*speed;
          if( dis < 10 )
               return;
          auto isz = varrow.size();
          ImVec2 og_pt = { 0, base_z };
          auto xf0 = finish_scale * 2 * radious;//抛物线轨迹的终点
          auto find_pt = [&](ImVec2& base_bt,float& angle_cur, float dis_delta, ImVec2& des_pt)//抛物线上寻找下一个点
          {
               auto dz_dx_base = equation_dz_dx(base_bt.x);
               auto angle_base = atanf(dz_dx_base);
               auto x_delta = dis_delta*cos(angle_base);
               ImVec2 tp_pt;
               tp_pt.x = base_bt.x + x_delta;
               tp_pt.y = equation0( tp_pt.x );
               auto tp_dis = pt_sq_length(base_bt, tp_pt);

               auto dis_c = dis_delta - tp_dis;

               //printf( "angle_base=%f,x_delta=%f,dis_c=%f\n", angle_base, x_delta, dis_c );
               
               auto dz_dx_tp = equation_dz_dx(tp_pt.x);
               auto angle_tp = atanf(dz_dx_tp);
               auto tp_x_delta = dis_c*cos(angle_tp);
               des_pt.x= tp_pt.x + tp_x_delta;
               des_pt.y = equation0(des_pt.x);
               ImVec2 anm = des_pt - base_bt;
               auto anm_c = anm.y / anm.x;
               angle_cur = PI*0.5 - atanf( anm_c );


          };
          printf("~~~~~~~~~~~\n");
          for( int ix = 0; ix < isz; ix++ )
          {
               auto& cur_aw = varrow[ ix ];
               auto& cur_paw = *cur_aw.parw;
               ImVec2& pre_point = cur_aw.point_pre;
               auto tz = cur_paw.get_trans_tlz()+dis;
		     auto dis_tp = dis;
               switch( cur_aw.on_going )
               {
                    case en_forward:
                         if( tz < base_z )
                         {
                              cur_paw.set_trans_tlz( tz );
                              auto ty = tz*tga;
                              cur_paw.set_trans_tly( ty );
                              cur_paw.set_trans_tlx( 0 );
                              cur_paw.set_alpha( 1.f );
                              cur_paw.set_trans_rty( 0);
                              break;
                         }
                         else
                         {
                              cur_aw.on_going = en_turning;
                              dis_tp = tz - base_z;
                         }
                    case en_turning:
                         if( pre_point.x<xf0 )
                         {
                              ImVec2 cur_pt;
                              float angle_turn=0.f;
                              find_pt( pre_point,angle_turn, dis_tp, cur_pt );
                              pre_point = cur_pt;
                              ImVec2 r_pt = og_pt + cur_pt;
                              cur_paw.set_trans_tlz( r_pt.y );
                              auto ty = r_pt.y*tga;
                              cur_paw.set_trans_tly( ty );
                              cur_paw.set_trans_tlx( r_pt.x );
                              float tangle = angle_turn * 180.f/PI;

                              printf( "tangle=%f，pre_x=%f,cur_x=%f\n", tangle,pre_point.x, cur_pt.x );
                            cur_paw.set_trans_rty( tangle );
                              break;
                         }
                         else
                         {
                              cur_aw.on_going = en_finished;
                         }
                    case en_finished:
                         if (pre_point.x<(xf0+1*radious))
                         {
                              auto af = equation_dz_dx( xf0 );//finish line的曲率
                              auto angle_base = atanf( af);
                              auto x_delta = dis_tp*cos( angle_base );
                              if( x_delta < 0 ) x_delta = -x_delta;
                              auto xf = pre_point.x + x_delta;
                              auto zf = equation_finish( xf );
                              pre_point = { xf, zf };
                              ImVec2 r_pt = og_pt + pre_point;
                              cur_paw.set_trans_tlz( r_pt.y );
                              auto ty = r_pt.y*tga;
                              cur_paw.set_trans_tly( ty );
                              cur_paw.set_trans_tlx( r_pt.x );
                              auto tangle = angle_base*180.f/PI;
                              cur_paw.set_trans_rty( tangle );
                         }
                         else
                         {
                              cur_aw.on_going = en_forward;
                              cur_aw.point_pre.x = 0;
                              cur_aw.point_pre.y = 0;
                              cur_paw.set_trans_tlx( 0 );
                              cur_paw.set_trans_tly( 0 );
                              cur_paw.set_trans_tlz( 0 );
                              cur_paw.set_trans_rty( 0 );
                         }
                         break;
                    default:
                         break;
               }
               float tlx = cur_paw.get_trans_tlx();
               float tly = cur_paw.get_trans_tly();
               float tlz = cur_paw.get_trans_tlz();
               //printf( "%d>>%f,%f,%f,%d\n", ix, tlx, tly, tlz, cur_aw.on_going );
         }
     }

};
arrow_animation_control g_acm_c( 2000.f, 0.02f);
u8 top_arrow_index = arrow_cont-1;
u8 prev_arrow_index( u8 by_index )
{
     if (by_index==0)
     {
          return arrow_cont-1;
     }
     else
     {
          return --by_index;
     }
}
u8 next_arrow_index( u8 by_index )
{
     by_index++;
     by_index %= arrow_cont;
     return by_index;
}


void handle_navi_direct(u8 nv_play)
{
     if( nv_play == ndir_no_play )
     {
	     parrow0->set_visible(false);
	     parrow1->set_visible(false);
	     parrow2->set_visible(false);
	     return;
     }
     parrow0->set_visible(true);
     parrow1->set_visible(true);
     parrow2->set_visible(true);
     prop_ele_position arrow0_transx = { parrow0, 1, 9 };
     set_trans_pair_state_list_delta( "navi_direct", arrow0_transx, 0, arrow_x0, direc_trans0_seg_detas );
     prop_ele_position arrow1_transx = { parrow1, 1, 9 };
     set_trans_pair_state_list_delta( "navi_direct1", arrow1_transx, 0, arrow_x1, direc_trans1_seg_detas );
     prop_ele_position arrow2_transx = { parrow2, 1, 9 };
     set_trans_pair_state_list_delta( "navi_direct2", arrow2_transx, 0, arrow_x2, direc_trans2_seg_detas );

     switch( nv_play )
     {
          case ndir_turn_left:
               play_tran_playlist( "navi_direct", 4 );
               play_tran_playlist( "navi_direct1", 4 );
               play_tran_playlist( "navi_direct2", 4 );
               break;
          case ndir_turn_right:
               play_tran_playlist( "navi_direct", 5 );
               play_tran_playlist( "navi_direct1", 5 );
               play_tran_playlist( "navi_direct2", 5 );
               break;
          case ndir_lane_switch_left:
               play_tran_playlist( "navi_direct", 2 );
               play_tran_playlist( "navi_direct1", 2 );
               play_tran_playlist( "navi_direct2", 2 );
               break;
          case ndir_lane_switch_right:
               play_tran_playlist( "navi_direct", 3 );
               play_tran_playlist( "navi_direct1", 3 );
               play_tran_playlist( "navi_direct2", 3 );
               break;
     }
}

void KeyTest(  int key) 
{
     switch( key )
     {
          case GLFW_KEY_LEFT:
               pre_turn_count = 4;
               be_visble = false;
               set_property_aliase_value( "show_arrows", &be_visble );
               be_visble = true;
               set_property_aliase_value( "show_left_turn_round0", &be_visble );
               play_tran( "play_left_turn_round0", 0, 1 );
               restore_trans_value( "play_left_turn_round0", 0 );
               break;
          case GLFW_KEY_RIGHT:
               play_tran_playlist( "navi_direct", 1 );
               play_tran_playlist( "navi_direct1", 1 );
               play_tran_playlist( "navi_direct2", 1 );
               break;
          case GLFW_KEY_L:
               play_tran_playlist( "navi_direct", 2 );
               play_tran_playlist( "navi_direct1", 2 );
               play_tran_playlist( "navi_direct2", 2 );
               break;
          case GLFW_KEY_R:
               play_tran_playlist( "navi_direct", 3 );
               play_tran_playlist( "navi_direct1", 3 );
               play_tran_playlist( "navi_direct2", 3 );
               break;
          case GLFW_KEY_0:
               restore_trans_value( "navi_direct", 0 );
               restore_trans_value( "navi_direct1", 0 );
               restore_trans_value( "navi_direct2", 0 );
               break;
          case GLFW_KEY_4:
               //play_tran_playlist( "navi_direct", 4 );
               //play_tran_playlist( "navi_direct1", 4 );
               //play_tran_playlist( "navi_direct2", 4 );
               play_playlist_group( "navi_turn_left" );
               break;
          case GLFW_KEY_5:
               //play_tran_playlist( "navi_direct", 5 );
               //play_tran_playlist( "navi_direct1", 5 );
               //play_tran_playlist( "navi_direct2", 5 );
               play_playlist_group( "navi_turn_right" );
            break;
          case GLFW_KEY_6:
               be_show = !be_show;
               set_property_aliase_value( "show_navi_dir", &be_show );
               break;
          case GLFW_KEY_7:
               be_a_moving = !be_a_moving;
               if (be_a_moving)
               {
                    //g_acm_c.active();
               }
               break;
          case GLFW_KEY_8:
               if( be_a_moving )
               {
                    g_turn_scale += 0.1;
                    if( g_turn_scale > 10.f )
                         g_turn_scale = 0;
                    arrow_cc.set_turn_scale( g_turn_scale );
               }
               break;          
         default:
               break;
     }
     
     
}
void adas_update()
{
#if 0
     if( be_a_moving )
     {
          float xf = 0.f, zf = 0.f, ag = 0.f,linear_zdelta=0.f,scale=0.f;
          if (!arrow_cc.calcu4( xf, zf, ag,linear_zdelta,scale))
          {
               top_arrow_index = prev_arrow_index( top_arrow_index );
          }
          
          printf( "linear_zdelta=%f\n", linear_zdelta );
          auto tx = aw_x + xf;
          auto tz = aw_z2 + zf;
          auto ax = aw_a - ag;
          auto alpha = 1.f - scale*0.8;
          //printf( "....(%f,%f,%f,%f)\n", tx, tz, ax, ag );
          auto& top_arrow = parrows[ top_arrow_index ];
          top_arrow->set_trans_tlx( tx );
          top_arrow->set_trans_tlz( tz );
          auto ty = tz*tga;
          top_arrow->set_trans_tly( ty );
          //top_arrow->set_alpha( alpha );
          //parrow2->set_trans_rtx( ax );
          if (ag>90)
          {
               ag = 90;
          }
          //parrow2->set_trans_rtz( -ag );
          //parrow2->set_trans_rty( ag );
          auto prev_idx = prev_arrow_index( top_arrow_index );
          auto& prev_arrow = parrows[ prev_idx ];
          auto ab_z1 = aw_z1 + linear_zdelta;
          auto ab_y1 = ab_z1*tga;
          prev_arrow->set_trans_tlx( 0 );
          prev_arrow->set_trans_tlz( ab_z1 );
          prev_arrow->set_trans_tly( ab_y1 );
          prev_arrow->set_alpha( 1.f );

          auto last_idx = prev_arrow_index( prev_idx );
          auto& last_arrow = parrows[ last_idx ];
          auto ab_z0 = aw_z0 + linear_zdelta;
          auto ab_y0 = ab_z0*tga;
          last_arrow->set_trans_tlx( 0 );
          last_arrow->set_trans_tlz( ab_z0 );
          last_arrow->set_trans_tly( ab_y0 );
          last_arrow->set_alpha( 1.f );

          auto hide_idx = prev_arrow_index( last_idx );
          auto& hide_arrow = parrows[ hide_idx ];
          auto ab_zh = aw_zh + linear_zdelta;
          auto ab_yh = ab_zh*tga;
          hide_arrow->set_trans_tlx( 0 );
          hide_arrow->set_trans_tlz( ab_zh );
          hide_arrow->set_trans_tly( ab_yh );
          hide_arrow->set_alpha( 1.f );
         
     }
#else
     if( be_a_moving )
     {
          //g_acm_c.handle();
     }
#endif
}
void register_adas_cmd_handl()
{
      g_image_show= (ft_image_file*)get_aliase_ui_control("show_image");
      pfcw_p=(ft_material_3d*)get_aliase_ui_control("show_fcw");
      pfcw_v=(ft_material_3d*)get_aliase_ui_control("show_fcw_v");
      psacc=(ft_material_3d*)get_aliase_ui_control("show_sacc");
      parrow0=(ft_material_3d*)get_aliase_ui_control("show_arrow3d0");
      parrow1=(ft_material_3d*)get_aliase_ui_control("show_arrow3d1");
      parrow2=(ft_material_3d*)get_aliase_ui_control("show_arrow3d2");
	 /** parrows[ 0 ] = (ft_material_3d*)get_aliase_ui_control("show_arrowh");
      parrows[ 1 ] = parrow0;
      parrows[ 2 ] = parrow1;
      parrows[ 3 ] = parrow2;
     
      g_acm_c.add_arrow( parrows[ 0 ] );
      g_acm_c.add_arrow( parrows[ 1 ] );
      g_acm_c.add_arrow( parrows[ 2 ] );
      g_acm_c.add_arrow( parrows[ 3 ] );
      */
      pturn_left=(ft_image_play*)get_aliase_ui_control("txt_left_wing");
      pturn_right=(ft_image_play*)get_aliase_ui_control("txt_right_wing");


	g_msg_host.attach_monitor("show_image", [&](u8* pbuff,int len){
		pbuff++;
		u8 pic_id=*pbuff;
		sprintf(str_show,"/run/media/sda1/%d.png",pic_id);
		g_image_show->set_visible(true);
		bool beshow=g_image_show->load_image_file(str_show);
		//u8 cmd[4]
		//string cmd={0x09,0x1a,1,beshow};
		//CommonMessageInterface(cmd);
	//cmd.resize(4);
	});
	 g_msg_host.attach_monitor("factory", [&](u8* pbuff,int len){
		u8 fcatory=*pbuff;
	      
		g_image_show->set_visible(fcatory!=0);
		
	//cmd.resize(4);
	});
	const auto& imtl = g_material_list.find("road");
	assert(imtl != g_material_list.end());
	mtl_lane=imtl->second;
	//mtl_lane->set_value("lc2", &lc2, 1);
	//mtl_lane->set_value("rc2", &lc2, 1);
	const auto& imtl_sacc=g_material_list.find("sacc");
	assert(imtl_sacc!=g_material_list.end());
	mtl_sacc=imtl_sacc->second;
	/*
	g_msg_host.attach_monitor("navi req",[&](u8*pbuff,int len){
		enum 
		{
			navi_inactive,
			navi_off,
			navi_on,
			navi_reserved,
		};
		if(*pbuff==navi_on)
		{
		}
		else
		{
		}
	});	
	*/
     reg_trans_handle( "play_left_turn_round0", [&]( int from, int to )
     {
          be_visble = false;
          set_property_aliase_value( "show_left_turn_round0", &be_visble );
          restore_trans_value( "show_left_turn_round0", 0 );
          be_visble = true;
          set_property_aliase_value( "show_left_turn_round1", &be_visble );
          play_tran( "play_left_turn_round1", 0, 1 );
     } );
     reg_trans_handle( "play_left_turn_round1", [&]( int from, int to )
     {
          if( pre_turn_count > 2 )
          {

               play_tran( "play_left_turn_round1", 0, 1 );
               pre_turn_count--;
          }
          else
          {
               be_visble = false;
               set_property_aliase_value( "show_left_turn_round1", &be_visble );
               restore_trans_value( "show_left_turn_round1", 0 );
               be_visble = true;
               set_property_aliase_value( "show_left_turn_round2", &be_visble );
               play_tran( "play_left_turn_round2", 0, 1 );
          }
     } );
     reg_trans_handle( "play_left_turn_round2", [&]( int from, int to )
     {
          pre_turn_count = 0;
          be_visble = false;
          set_property_aliase_value( "show_left_turn_round2", &be_visble );
          restore_trans_value( "show_left_turn_round2", 0 );
          be_visble = true;
          set_property_aliase_value( "show_arrows", &be_visble );

     } );

	g_msg_host.attach_monitor("navi message",[&](u8*pbuff,int len){
		enum en_navi_direction
		{
			inactive,
			off,
			staight_ahead,
			front_left_ahead,
			turning_left,
			left_rear_backwards,
			left_turning_round,
			right_rear_backwards,
			turn_right,
			front_right_ahead,
			right_turning_round,
			turning_left_by_anticlockwise_roundabout,
			turning_right_by_anticlockwise_roundabout,
			straight_ahead_by_anticlockwise_roundabout,
			turning_round_by_anticlockwise_roundabout,
			turning_left_by_clockwise_roundabout,
			turning_right_by_clockwise_roundabout,
			straight_ahead_by_clockwise_roundabout,
			turning_round_by_clockwise_roundabout,
			clockwise_entry_roundabout,
			clockwise_exit_roundabout,
			anticlockwise_entry_roundabout,
			anticlockwise_exit_roundabout,
			direction_motion,
			via_point_arrived,
			service_area_arrived,
			tool_station_arrived,
			destination_arrived,
			into_the_tunnel,
			reserved,
		};
		int navi_direction_txt[reserved-2]=
		{
			en_NaviIcon_StraightLine_png,//staight_ahead,
			en_NaviIcon_LeftFront_png,//front_left_ahead,
			en_NaviIcon_LeftTurn_png,//turning_left,
			en_NaviIcon_LeftRear_png,//left_rear_backwards,
			en_NaviIcon_TurnDown_png,//left_turning_round,
			en_NaviIcon_RightRear_png,//right_rear_backwards,
			en_NaviIcon_RightTurn_png,//turn_right,
			en_NaviIcon_RightFront_png,//front_right_ahead,
			en_NaviIcon_RightTurningRound_png,//right_turning_round,
			en_NaviIcon_TurnAround_LeftTurn_png,//turning_left_by_anticlockwise_roundabout,
			en_NaviIcon_Roundabout_png,//turning_right_by_anticlockwise_roundabout,
			en_NaviIcon_TurnAround_StraightLine_png,//straight_ahead_by_anticlockwise_roundabout,
			en_NaviIcon_TurningRoundAnticlockwiseRoundaout_png,//turning_round_by_anticlockwise_roundabout,
			en_NaviIcon_TurningLeftClockwiseRoundabout_png,//turning_left_by_clockwise_roundabout,
			en_NaviIcon_TurningRightClockwiseRoundabout_png,//turning_right_by_clockwise_roundabout,
			en_NaviIcon_StraightAheadClockwiseRoundabout_png,//straight_ahead_by_clockwise_roundabout,
			en_NaviIcon_TurningRoundClockwiseRoundabout_png,//turning_round_by_clockwise_roundabout,
			en_NaviIcon_ClockwiseEntryRoundabout_png,//clockwise_entry_roundabout,
			en_NaviIconClockExitRoundabout_png,//clockwise_exit_roundabout,
			en_NaviIcon_TurnAround_RightUp_png,//anticlockwise_entry_roundabout,
			en_NaviIcon_TurnAround_StraightLine_png,//anticlockwise_exit_roundabout,
			en_NaviIcon_AlongTheLine_png,//direction_motion,
			en_NaviIcon_WayPoint_2_png,//via_point_arrived,
			en_NaviIcon_ServiceStation_png,//service_area_arrived,
			en_NaviIcon_TollGate_png,//tool_station_arrived,
			en_NaviIcon_End_png,//destination_arrived,
			en_NaviIcon_Tunnel_png,//into_the_tunnel,
			
		};
		int navi_roundabout[2][10]=
		{
			{
			en_NaviIcon_TurnAround_1_png,
			en_NaviIcon_TurnAround_2_png,
			en_NaviIcon_TurnAround_3_png,
			en_NaviIcon_TurnAround_4_png,
			en_NaviIcon_TurnAround_5_png,
			en_NaviIcon_TurnAround_6_png,
			en_NaviIcon_TurnAround_7_png,
			en_NaviIcon_TurnAround_8_png,
			en_NaviIcon_TurnAround_9_png,
			en_NaviIcon_TurnAround_10_png
			},
			{
			en_NaviIcon_TurnAroundleft_1_png,
			en_NaviIcon_TurnAroundleft_2_png,
			en_NaviIcon_TurnAroundleft_3_png,
			en_NaviIcon_TurnAroundleft_4_png,
			en_NaviIcon_TurnAroundleft_5_png,
			en_NaviIcon_TurnAroundleft_6_png,
			en_NaviIcon_TurnAroundleft_7_png,
			en_NaviIcon_TurnAroundleft_8_png,
			en_NaviIcon_TurnAroundleft_9_png,
			en_NaviIcon_TurnAroundleft_10_png
			}
		};
		enum navi_range
		{
			navi_inactive,
			less_than_1000,
			less_than_10000,
			greater_than_or_equal_to_10000,
			range_cnt
		};
		struct GNU_DEF struct_can_msg
		{
			u8 HU_HUD_NaviDisplayDirection:5;
			u8 HU_HUD_NextCrossDistanceRange:2;
			u8 reserved:1;
			u8 HU_HUD_NaviRoundaboutExitCross:5;
			u8 reserved2:3;
			u16 HU_HUD_NextCrossDistance:14;
			u16 reserved3:2;
			u32 reserved4;
		};
		u8 signal_valid=*pbuff++;
		struct_can_msg* pcan=(struct_can_msg*)pbuff;
		bool beshow;
		#if 0
		printf("signal_valid=%d,HU_HUD_NextCrossDistance=%x,HU_HUD_NaviDisplayDirection=%d\n",\
			signal_valid,pcan->HU_HUD_NextCrossDistance,pcan->HU_HUD_NaviDisplayDirection);
		#endif
		if(signal_valid==signal_lost
			||pcan->HU_HUD_NextCrossDistance==0x3ffe
			||pcan->HU_HUD_NaviDisplayDirection<staight_ahead)
		{
		       beshow=false;
			set_property_aliase_value("show_navi_dir", &beshow);
			set_property_aliase_value("show_remain_dis", &beshow);
		}
		else
		if(pcan->HU_HUD_NaviDisplayDirection<reserved)
		{
		
			beshow=true;
			set_property_aliase_value("show_navi_dir", &beshow);
			set_property_aliase_value("show_remain_dis", &beshow);
			u8 txtindex=pcan->HU_HUD_NaviDisplayDirection-staight_ahead;
			int txt_id=navi_direction_txt[txtindex];
			set_property_aliase_value("control_navi_dir", &txt_id);
			
		}
		u8 cross_rg=pcan->HU_HUD_NextCrossDistanceRange;
		if(cross_rg&&cross_rg<range_cnt)
		{
			const struct
			{
				float reslution;
				const char* unit;
			}range_st[range_cnt]=
			{
				{0,""},
				{10.f,"m"},
				{0.1f,"km"},
				{1.f,"km"},
			};
			
			int reslution=range_st[cross_rg].reslution;
			int next_cross_dis=pcan->HU_HUD_NextCrossDistance*reslution;
			sprintf(str_show,"%d",next_cross_dis);
			auto tlen=strlen(str_show);
			strcpy(str_show+tlen,range_st[cross_rg].unit);
			set_property_aliase_value("text_remain_dis", str_show);
		}
		
	});	
	 g_msg_host.attach_monitor("turning curvature", [&](u8* pbuff,int len){
		u8 DMS_VehicleTurningCurvature=*pbuff;
	  });
	 g_msg_host.attach_monitor("steering info", [&](u8* pbuff,int len){
	 	enum en_wheel_sign
	 	{
	 		en_w_left,
			en_w_right,
	 	};
		enum en_SAS_St
		{
			sas_angle_and_speed_correct,
			sas_not_calibrated,
			intermittent_error_detected,
			permanent_error_detected,
		};
		struct GNU_DEF struct_can_msg
		{
			u8 check_sum;
			u16 SteeringWheelAngle:15;//res:0.04375 u:o
			u16 SteeringWheelAngleSign:1;// 0 left,1 right
			u16 SteeringWheelSpeed:15;//res:0.04375 u:o/s
			u16 SteeringWheelSpeedSign:1;// 0 left,1 right
			u16 Reserved;
			u8 SAS_St:2;
			u8 Reserved2:2;
			u8 LiveCounter_EPS_1_SAS_1:4;
		};
	  });
	 g_msg_host.attach_monitor("lka", [&](u8* pbuff,int len){
		u8 HAD_Intervention_LKA=*pbuff;
		pbuff++;
		u8 HAD_WorkingSt_LKA=*pbuff;
	  });
	 tmid_speed=g_timer.register_timer_ex([&](int tid){
		bool be_show=tid%2;
		set_property_aliase_value("show_acc_speed", &be_show);
	 });
	 //HAD_WorkingSt_ACC_L12
	 g_msg_host.attach_monitor("acc", [&](u8* pbuff,int len){
              enum en_acc_state
		{
			en_off,
			en_stand_by,
			en_active,
			en_override,
			en_shutOff,
			en_available,
			en_reserved,
			en_reserved2,
			en_acc_cnt,
		};
		int acc_control[2]={en_ACC_b_png,en_ACC_png};
		u8 acc_state=*pbuff;
		g_be_acc=true;
		bool be_show=true;
		bool be_show_acc_speed=false;
		switch(acc_state)
		{
			case en_available:
				set_property_aliase_value("control_acc",  &acc_control[0]);
				break;
			case en_active:
				set_property_aliase_value("control_acc",  &acc_control[1]);
				be_show_acc_speed=true;
				break;
			case en_override://flash the speed
				be_show_acc_speed=true;
				if(!g_timer.is_actived_timer_ex(tmid_speed))
				{
					g_timer.active_timer_ex(tmid_speed,500);
					set_property_aliase_value("show_acc_speed", &be_show_acc_speed);
				}
				break;
			/*case en_stand_by:
				g_be_acc=false;
				be_show=false;
				break;*/
			default:
				g_be_acc=false;
				be_show=false;
				break;
		}
		if(acc_state!=en_override)
		{
			g_timer.deactive_time_ex(tmid_speed);
			set_property_aliase_value("show_acc_speed", &be_show_acc_speed);
		}
		set_property_aliase_value("show_acc", &be_show);
		obj_sacc_handle();
		
	  });
	 //HAD_WorkingSt_LCA
	 g_msg_host.attach_monitor("lca", [&](u8* pbuff,int len){
		enum
		{
			off,
			stand_by,
			availiable,
			active,
			exit,
			recoverable_laneline_disapearing,
			recoverable_steering_override,
			reserved,
		};
		u8 HAD_WorkingSt_LCA=*pbuff;
		g_be_lca=HAD_WorkingSt_LCA>off&&HAD_WorkingSt_LCA<recoverable_laneline_disapearing;
		obj_sacc_handle();
		
	 });
	 tmid_lane_ldw=g_timer.register_timer_ex([&](int tid){
		int ihide=tid%2;
	 	if(lane_ldw_status==LeftWarning)
	 	{
			mtl_lane->set_value("lhide",&ihide,1);
		}
		else if(lane_ldw_status==RightWarning)
		{
			mtl_lane->set_value("rhide",&ihide,1);
		}
		else if(lane_ldw_status==ldw_inactive)
		{
			if(tid>4)
			{
				ihide=0;
				mtl_lane->set_value("lhide",&ihide,1);
				mtl_lane->set_value("rhide",&ihide,1);
				g_timer.deactive_time_ex(tmid_lane_ldw);
			}
		}
		
	 });
	 g_msg_host.attach_monitor("had_10", [&](u8* pbuff,int len){
	 	enum en_HAD_SysTextinfo
	 	{
	 		no_display,
			condition_not_satisfied,
			radar_error,
			stop_go_clue_step_gas_pedal_or_resume_key,
			acc_cancelled_for_ESC_interven,
			driver_cancel_operation,
			bad_weather_system_can_not_work,
			system_initial,
			camera_is_polluted,
			camera_is_not_calibration,
			speed_is_less_than_10_unable_to_acctive_acc,
			acc_cancelled_autoly,
			lca_cancelled_autoly,
			sacc_cancelled_autoly,
			radar_not_calibration,
			radar_is_covered,
	 	};
		enum en_HAD_LDWModeInd
		{
			ldw_only,
			ldw_Vibration,
			ldw_LKA,
			Reserved,
		};
		enum en_HAD_TakeOverReq
		{
			Inactive,
			Request,
		};
		enum en_HAD_WorkingSt_LDW
		{
			ldw_off,
			ldw_standby,
			ldw_active,
			ldw_reserved,
		};
		
		enum en_HAD_LDWLaneDetectSt
		{
			neither_side_lane_mark_detected,
			left_side_lane_mark_detected,
			right_side_lane_mark_detected,
			both_side_lane_marks_detected,
		};
		enum en_HAD_WarningCmd_FCW
		{
			no_alert,
			imminent_alert,
			caution_alert,
			fcw_reserved,
		};
		struct GNU_DEF struct_can_msg
		{
			u8 HAD_CruiseSpeedSetting_ACC;//1km/h
			u8 HAD_TimeGapSetting:3;//TimeGap1....4
			u8 HAD_ObjectDetection:1;//0 no object,1 target detected
			u8 HAD_SysTextinfo:4;//
			u8 HAD_LDWModeInd : 2;
			u8 Reserved : 1;
			u8 HAD_TakeOverReq : 1;
			u8 Reserved2:4;
			u16 HAD_WorkingSt_LDW:2;
			u16 HAD_WarningCmd_LDW:2;
			u16 HAD_LDWLaneDetectSt:2;
			u16 Reserved3:5;
			u16 HAD_WarningCmd_FCW:2;
			u32 Reserved4:27;
		};
		u8 signal_status=*pbuff;
		pbuff++;
		struct_can_msg* pcan=(struct_can_msg*)pbuff;
		HAD_TimeGapSetting=pcan->HAD_TimeGapSetting;
		u8 acc_tar_speed=pcan->HAD_CruiseSpeedSetting_ACC;
		if(acc_tar_speed>=0xFE)
		{
			str_show[0]='\0';
			set_property_aliase_value("text_acc_speed", str_show);
	       }
		else
		{
			sprintf(str_show,"%d",acc_tar_speed);
			set_property_aliase_value("text_acc_speed", str_show);
			//bool be_show=true;
			//set_property_aliase_value("show_acc_speed", &be_show);
			
		}
		switch(pcan->HAD_LDWLaneDetectSt)
		{
			case neither_side_lane_mark_detected:
				llane_status_id=en_lane_off;
				rlane_status_id=en_lane_off;
				break;
			case left_side_lane_mark_detected:
				llane_status_id=en_lane_on;
				rlane_status_id=en_lane_off;
				break;
			case right_side_lane_mark_detected:
				llane_status_id=en_lane_off;
				rlane_status_id=en_lane_on;
				break;
			case both_side_lane_marks_detected:
				llane_status_id=en_lane_on;
				rlane_status_id=en_lane_on;
				break;
		}
		int ihide=0;
		lane_ldw_status=pcan->HAD_WarningCmd_LDW;
		if(pcan->HAD_WorkingSt_LDW==ldw_off)
		{
			lane_ldw_status=ldw_inactive;
		}
		else
		{
			switch(lane_ldw_status)
			{
				case ldw_inactive:				
					mtl_lane->set_value("rhide",&ihide,1);
					mtl_lane->set_value("lhide",&ihide,1);
					break;
				case LeftWarning:
					llane_status_id=en_lane_ldw;
					mtl_lane->set_value("rhide",&ihide,1);
					g_timer.active_timer_ex(tmid_lane_ldw,200);
					break;
				case RightWarning:
					rlane_status_id=en_lane_ldw;
					mtl_lane->set_value("lhide",&ihide,1);
					g_timer.active_timer_ex(tmid_lane_ldw,200);
					break;
			}
		}

		set_mp_text_uf(lane_mt, "llane", lane_txt_name[theme_id][llane_status_id]);
		set_mp_text_uf(lane_mt, "rlane", lane_txt_name[theme_id][rlane_status_id]);
		if(pcan->HAD_ObjectDetection)
		{
			u8 fcw_level=pcan->HAD_WarningCmd_FCW;
			obj_fcw_handle(fcw_level);
		}
		
	  });
	 tmid_tsr=g_timer.register_timer_ex([&](int tid){
		int ihide=tid%2;
	 	bool be_show=false;
		if(HAD_WarningCmd_TSR==tsr_warning2)
		{
			be_show=ihide==0||tid>=10;
		}
		set_property_aliase_value("show_tsr",&be_show);
		if(HAD_WarningCmd_TSR==tsr_inactive)
		{
			be_show=false;
			set_property_aliase_value("show_tsr",&be_show);
			g_timer.deactive_time_ex(tmid_tsr);
		}
		
	 });
	 g_msg_host.attach_monitor("tsr", [&](u8* pbuff,int len){
		u8 HAD_LimitedVehicleSpeed_TSR=*pbuff++;//km/h
		if(HAD_LimitedVehicleSpeed_TSR>0)
		{
			sprintf(str_show,"%d",HAD_LimitedVehicleSpeed_TSR);
			set_property_aliase_value("text_tsr_speed", str_show);
		}
		 HAD_WarningCmd_TSR=*pbuff;
		/*if(HAD_WarningCmd_TSR==tsr_inactive)
		{
		       //bool be_show=false;
			//set_property_aliase_value("show_tsr",&be_show);
		}
		else*/
		
		if(HAD_WarningCmd_TSR==tsr_warning1)
		{
		       int tvalue=5000;
			bool be_show=true;
		       set_property_aliase_value("show_tsr",&be_show);
			if(!g_timer.is_actived_timer_ex(tmid_tsr))
			{
				play_tran("tsr_rg", 0,1);
			}
			g_timer.active_timer_ex(tmid_tsr,tvalue);
		}
		else
		if(HAD_WarningCmd_TSR==tsr_warning2)
		{
			 int tvalue=500;
			 if(!g_timer.is_actived_timer_ex(tmid_tsr))
			 {
				 bool be_show=true;
				set_property_aliase_value("show_tsr",&be_show);
				play_tran("tsr_rg", 0,1);
			 	g_timer.active_timer_ex(tmid_tsr,tvalue);
			 }
			 else
			 {
				 int prev_tvalue=g_timer.get_tvalue_ex(tmid_tsr);
				 if(prev_tvalue!=tvalue)
				 {
					bool be_show=true;
					set_property_aliase_value("show_tsr",&be_show);
			 		g_timer.active_timer_ex(tmid_tsr,tvalue);
				 }
			 }
		}
		
	  });
	 g_msg_host.attach_monitor("aeb", [&](u8* pbuff,int len){
	 	enum en_Intervention_AEB
	 	{
	 		no_intervention,
			intervention,
	 	};
		u8 Intervention_AEB=*pbuff;
		if(Intervention_AEB)
		{
			obj_AEB_handle();
		}
		else
		{
			obj_fcw_hide();
		}
	  });
	 
	 /*
	tmid_cave_lnae=g_timer.register_timer_ex([&](int idx){
		g_c2-=c2u;
		//printf("fps:%f\n",ImGui::GetIO().Framerate);
	      mtl_lane->set_value("lc2", &g_c2, 1);
	      mtl_lane->set_value("rc2", &g_c2, 1);
		if(g_c2<rc2||g_c2>lc2)
		{
			c2u=-c2u;
		}
		if(g_debug_stm)
		{
			auto tm=idx%10;
			if(tm==0)
			{
				play_tran_playlist("navi_direct",0);
			}
			if(tm==5)
			{
				play_tran("tsr_rg", 0,1);
			}
		}
	});
	g_timer.active_timer_ex(tmid_cave_lnae,300);*/
}

void register_private_cmd()
{


	/*g_mh_pc.attach_monitor("lane_line_l_1", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum_IFC__LaneLine_L_1;
			u16 b_1_2_IFC_Timestamp_LaneLine_L_1;
			u8 b3_IFC_LaneLineType_L : 3;
			u8 b3_IFC_LaneLineColor_L : 2;
			u8 b3_Reserved : 2;
			u8 b3_IFC_LaneLineValidity_L : 1;
			u8 b4_IFC_LaneLineConfidence_L : 7;
			u8 b4_Reserved : 1;
			u32 b_4_7_Reserved : 20;
			u32 b7_LiveCounter_IFC__LaneLine_L_1 : 4;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
	});*/
	#if 1
	g_mh_pc.attach_monitor("lane_line_ll_2", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum_IFC__LaneLine_2;
			u16 b_1_2_IFC_Timestamp_LaneLine_2;
			u32 b_3_5_IFC_LaneLineShap_C2 : 18;
			u32 b_5_7_IFC_LaneLineShap_C1_a0 : 14;//′?′|×?????è?Dèòaa0oía1??DD??????μ?3?
			u8 b_5_7_IFC_LaneLineShap_C1_a1 : 2;
			u8 b7_Reserved : 2;
			u8 b7_LiveCounter_IFC__LaneLine_2 : 4;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
		u16 sc1= (u16)(can_msg->b_5_7_IFC_LaneLineShap_C1_a1 << 14 | can_msg->b_5_7_IFC_LaneLineShap_C1_a0);
		float llc1=sc1*0.0001-2.f;
		float llc2=can_msg->b_3_5_IFC_LaneLineShap_C2*0.000001f-0.125f;

	});
	g_mh_pc.attach_monitor("lane_line_ll_3", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum_IFC_LaneLine_3;
			u16 b_1_2_IFC_Timestamp_LaneLine_3;
			s16 b_3_4_FC_LaneLineShap_C0;
			s32 b_5_7_IFC_LaneLineShap_C3 : 20;
			u32 b7_LiveCounter_IFC__LaneLine_3 : 4;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
		float llc0=can_msg->b_3_4_FC_LaneLineShap_C0-30000;
		
	});
	
	g_mh_pc.attach_monitor("lane_line_l_2", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum_IFC__LaneLine_2;
			u16 b_1_2_IFC_Timestamp_LaneLine_2;
			u32 b_3_5_IFC_LaneLineShap_C2 : 18;
			u32 b_5_7_IFC_LaneLineShap_C1_a0 : 14;//′?′|×?????è?Dèòaa0oía1??DD??????μ?3?
			u8 b_5_7_IFC_LaneLineShap_C1_a1 : 2;
			u8 b7_Reserved : 2;
			u8 b7_LiveCounter_IFC__LaneLine_2 : 4;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
		u16 sc1= (u16)(can_msg->b_5_7_IFC_LaneLineShap_C1_a1 << 14 | can_msg->b_5_7_IFC_LaneLineShap_C1_a0);
		lc1=sc1*0.0001-2.f;
		lc2=can_msg->b_3_5_IFC_LaneLineShap_C2*0.000001f-0.125f;
		/*static int dlcnt=0;
		dlcnt++;
		if(dlcnt>50)
		{
			dlcnt=0;
			printf("lc1=%f lc2=%f\n",lc1,lc2);
		}*/
		mtl_lane->set_value("lc1", &lc1, 1);
		mtl_lane->set_value("lc2", &lc2, 1);
		lanec[0]=true;
		calcu_arrows_transx();
	});
	g_mh_pc.attach_monitor("lane_line_l_3", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum_IFC_LaneLine_3;
			u16 b_1_2_IFC_Timestamp_LaneLine_3;
			s16 b_3_4_FC_LaneLineShap_C0;
			s32 b_5_7_IFC_LaneLineShap_C3 : 20;
			u32 b7_LiveCounter_IFC__LaneLine_3 : 4;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
		lc0=can_msg->b_3_4_FC_LaneLineShap_C0-30000;
		/*static int dlcnt=0;
		dlcnt++;
		if(dlcnt>50)
		{
			dlcnt=0;
			printf("lc0=%f\n",lc0);
		}*/
		mtl_lane->set_value("lc0", &lc0, 1);
		lanec[1]=true;
		calcu_arrows_transx();
	});
	g_mh_pc.attach_monitor("lane_line_l_4", [&](u8* pbuff,int len){
		
	});
	g_mh_pc.attach_monitor("lane_line_r_1", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum_IFC__LaneLine_R_1;
			u16 b_1_2_IFC_Timestamp_LaneLine_R_1;
			u8 b3_IFC_LaneLineType_R : 3;
			u8 b3_IFC_LaneLineColor_R : 2;
			u8 b3_Reserved : 2;
			u8 b3_IFC_LaneLineValidity_R : 1;
			u8 b4_IFC_LaneLineConfidence_R : 7;
			u8 b4_Reserved : 1;
			u32 b_4_7_Reserved : 20;
			u32  b7_LiveCounter_IFC__LaneLine_R_1 : 4;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
	});
	g_mh_pc.attach_monitor("lane_line_r_2", [&](u8* pbuff,int len){
              struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum_IFC__LaneLine_2;
			u16 b_1_2_IFC_Timestamp_LaneLine_2;
			u32 b_3_5_IFC_LaneLineShap_C2 : 18;
			u32 b_5_7_IFC_LaneLineShap_C1_a0 : 14;//′?′|×?????è?Dèòaa0oía1??DD??????μ?3?
			u8 b_5_7_IFC_LaneLineShap_C1_a1 : 2;
			u8 b7_Reserved : 2;
			u8 b7_LiveCounter_IFC__LaneLine_2 : 4;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
		u16 sc1= (u16)(can_msg->b_5_7_IFC_LaneLineShap_C1_a1 << 14 | can_msg->b_5_7_IFC_LaneLineShap_C1_a0);
		rc1=sc1*0.0001-2.f;
		rc2=can_msg->b_3_5_IFC_LaneLineShap_C2*0.000001f-0.125f;
		/*static int dlcnt=0;
		dlcnt++;
		if(dlcnt>50)
		{
			dlcnt=0;
			printf("rc1=%f,rc2=%f\n",rc1,rc2);
		}*/		
		mtl_lane->set_value("rc1", &rc1, 1);
		mtl_lane->set_value("rc2", &rc2, 1);
		lanec[2]=true;
		calcu_arrows_transx();
	});
	g_mh_pc.attach_monitor("lane_line_r_3", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum_IFC_LaneLine_3;
			u16 b_1_2_IFC_Timestamp_LaneLine_3;
			s16 b_3_4_FC_LaneLineShap_C0;
			s32 b_5_7_IFC_LaneLineShap_C3 : 20;
			u32 b7_LiveCounter_IFC__LaneLine_3 : 4;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
		rc0=can_msg->b_3_4_FC_LaneLineShap_C0-30000;
		/*static int dlcnt=0;
		dlcnt++;
		if(dlcnt>50)
		{
			dlcnt=0;
			printf("rc0=%f\n",rc0);
		}*/
		mtl_lane->set_value("rc0", &rc0, 1);
		lanec[3]=true;
		calcu_arrows_transx();
	});
	g_mh_pc.attach_monitor("lane_line_r_4", [&](u8* pbuff,int len){
		
	});	
	g_mh_pc.attach_monitor("lane_line_rr_2", [&](u8* pbuff,int len){
		 struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum_IFC__LaneLine_2;
			u16 b_1_2_IFC_Timestamp_LaneLine_2;
			u32 b_3_5_IFC_LaneLineShap_C2 : 18;
			u32 b_5_7_IFC_LaneLineShap_C1_a0 : 14;//′?′|×?????è?Dèòaa0oía1??DD??????μ?3?
			u8 b_5_7_IFC_LaneLineShap_C1_a1 : 2;
			u8 b7_Reserved : 2;
			u8 b7_LiveCounter_IFC__LaneLine_2 : 4;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
		u16 sc1= (u16)(can_msg->b_5_7_IFC_LaneLineShap_C1_a1 << 14 | can_msg->b_5_7_IFC_LaneLineShap_C1_a0);
		float rrc1=sc1*0.0001-2.f;
		float rrc2=can_msg->b_3_5_IFC_LaneLineShap_C2*0.000001f-0.125f;

	});
	g_mh_pc.attach_monitor("lane_line_rr_3", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum_IFC_LaneLine_3;
			u16 b_1_2_IFC_Timestamp_LaneLine_3;
			s16 b_3_4_FC_LaneLineShap_C0;
			s32 b_5_7_IFC_LaneLineShap_C3 : 20;
			u32 b7_LiveCounter_IFC__LaneLine_3 : 4;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
		float rrc0=can_msg->b_3_4_FC_LaneLineShap_C0-30000;
		
	});
	g_mh_pc.attach_monitor("obj", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum_IFC_Obj;
			u16 b_1_2_IFC_Timestamp_Obj;
			u8 b3_IFC_Obj_CIPV_ID;
			u8 b4_IFC_Obj_VD_Count : 5;//vechicle count
			u8 b4_Reserved : 3;

			u32 b5_IFC_Obj_VRU_Count : 5;//pedestrain count
			u32 b_5_7_Reserved : 15;
			u32 b7_LiveCounter_IFC_Obj : 4;

		};
		struct_can_msg* pcan_msg = (struct_can_msg*)pbuff;
		obj_vechicle_cnt=pcan_msg->b4_IFC_Obj_VD_Count;
		obj_pedestrain_cnt=pcan_msg->b5_IFC_Obj_VRU_Count;
		obj_cipv_id=pcan_msg->b3_IFC_Obj_CIPV_ID;
	});
	
	g_mh_pc.reg_batching_cmd(0, [&](u8* pbuff,int len)->bool{

		const u16 obj0_cmd=0x422;//IFC_Obj1_1
		u16* pcmd=(u16*)pbuff;
		if(*pcmd<obj0_cmd)
		{
			return false;
		}
		u16 cmd_offset=*pcmd-obj0_cmd;
		u16 obj_index=cmd_offset/4;
		u16 sub_cmd=cmd_offset%4;
		pbuff+=2;
		len-=2;
		msg_handle obj_cmd[4]={
		[&](u8* pbuff,int len){
				struct GNU_DEF struct_can_msg
				{
					u8 b0_Checksumj;
					u16 b_1_2_IFC_Timestamp1;
					u8 b3_IFC_ID;
					u8 b4_IFC_MotionStatus : 3;
					u8 b4_Reserved : 1;

					u8 b4_IFC_MotionPattern : 4;
					u32 b5_IFC_Type : 4;
					u32 b_5_6_IFC_TTC : 11;
					u32 b_6_7_Reserved : 5;
					u32 b7_LiveCounter_IFC : 4;
				};
				struct_can_msg* pcan_msg = (struct_can_msg*)pbuff;
				g_obj_tbs[obj_index].obj_id=pcan_msg->b3_IFC_ID;
				g_obj_tbs[obj_index].obj_motion_status=pcan_msg->b4_IFC_MotionStatus;
				g_obj_tbs[obj_index].obj_motion_pattern=pcan_msg->b4_IFC_MotionPattern;
			       g_obj_tbs[obj_index].obj_type=pcan_msg->b5_IFC_Type;
				g_obj_tbs[obj_index].obj_TTC=pcan_msg->b_5_6_IFC_TTC*0.01-8;
			},
		[&](u8* pbuff,int len){
				struct GNU_DEF struct_can_msg
				{
					u8 b0_Checksum;
					u16 b_1_2_IFC_Timestamp;
					u32 b_3_4_IFC_XPos_Near : 14;
					u32 b_4_6_IFC_YPos_Near : 12;
					u32 b_6_7_IFC_Width : 9;

					u8 b7_Reserved : 1;
					u8 b7_LiveCounter_IFC : 4;

				};
				struct_can_msg* pcan_msg = (struct_can_msg*)pbuff;
				g_obj_tbs[obj_index].xpos_near=pcan_msg->b_3_4_IFC_XPos_Near*0.05-200.f;
				g_obj_tbs[obj_index].ypos_near=pcan_msg->b_4_6_IFC_YPos_Near*0.05-100.f;
				g_obj_tbs[obj_index].width=pcan_msg->b_6_7_IFC_Width*0.01+0.4;
				
			},
		[&](u8* pbuff,int len){
				struct GNU_DEF struct_can_msg
				{
					u8 b0_Checksum;
					u16 b_1_2_IFC_Timestamp;
					u32 b_3_4_IFC_XSpeedRelative : 13;
					u32 b_4_5_IFC_YSpeedRelative : 11;
					u32 b_6_7_IFC_Orientation : 10;
					u8 b7_Reserved : 2;
					u8 b7_LiveCounter : 4;

				};
				struct_can_msg* pcan_msg = (struct_can_msg*)pbuff;
				g_obj_tbs[obj_index].xspeed=pcan_msg->b_3_4_IFC_XSpeedRelative*0.05-120;
				g_obj_tbs[obj_index].yspeed=pcan_msg->b_4_5_IFC_YSpeedRelative*0.05-50;
				
				
			},
		[&](u8* pbuff,int len){
				struct GNU_DEF struct_can_msg
				{
					u8 b0_Checksum;
					u16 b_1_2_IFC_Timestamp;
					u32 b_3_4_IFC_Length : 12;
					u32 b_4_5_IFC_height : 9;
					u32 b5_Reserved : 3;

					u32 b6_IFC_Confidence : 7;
					u32 b_6_7_Reserved: 5;
					u8 b7_LiveCounter_IFC : 4;

				};
				struct_can_msg* pcan_msg = (struct_can_msg*)pbuff;
				g_obj_tbs[obj_index].length=pcan_msg->b_3_4_IFC_Length*0.01;
				g_obj_tbs[obj_index].height=pcan_msg->b_4_5_IFC_height*0.01;
			}
		};
		obj_cmd[sub_cmd](pbuff,len);
		return true;
	});
	#else
	g_mh_pc.attach_monitor("obj1_1", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksumj;
			u16 b_1_2_IFC_Timestamp1;
			u8 b3_IFC_ID;
			u8 b4_IFC_MotionStatus : 3;
			u8 b4_Reserved : 1;

			u8 b4_IFC_MotionPattern : 4;
			u32 b5_IFC_Type : 4;
			u32 b_5_6_IFC_TTC : 11;
			u32 b_6_7_Reserved : 5;
			u32 b7_LiveCounter_IFC : 4;
		};
		struct_can_msg* pcan_msg = (struct_can_msg*)pbuff;
		g_obj_tbs[0].obj_motion_status=pcan_msg->b4_IFC_MotionStatus;
		g_obj_tbs[0].obj_motion_pattern=pcan_msg->b4_IFC_MotionPattern;
	       g_obj_tbs[0].obj_type=pcan_msg->b5_IFC_Type;
		g_obj_tbs[0].obj_TTC=pcan_msg->b_5_6_IFC_TTC*0.01-8;
	});
	g_mh_pc.attach_monitor("obj1_2", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum;
			u16 b_1_2_IFC_Timestamp;
			u32 b_3_4_IFC_XPos_Near : 14;
			u32 b_4_6_IFC_YPos_Near : 12;
			u32 b_6_7_IFC_Width : 9;

			u8 b7_Reserved : 1;
			u8 b7_LiveCounter_IFC : 4;

		};
		struct_can_msg* pcan_msg = (struct_can_msg*)pbuff;
		g_obj_tbs[0].xpos_near=pcan_msg->b_3_4_IFC_XPos_Near*0.05-200.f;
		g_obj_tbs[0].ypos_near=pcan_msg->b_4_6_IFC_YPos_Near*0.05-100.f;
		
	});
	g_mh_pc.attach_monitor("obj1_3", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum;
			u16 b_1_2_IFC_Timestamp;
			u32 b_3_4_IFC_XSpeedRelative : 13;
			u32 b_4_5_IFC_YSpeedRelative : 11;
			u32 b_6_7_IFC_Orientation : 10;
			u8 b7_Reserved : 2;
			u8 b7_LiveCounter : 4;

		};
		struct_can_msg* pcan_msg = (struct_can_msg*)pbuff;
		g_obj_tbs[0].xspeed=pcan_msg->b_3_4_IFC_XSpeedRelative*0.05-120;
		g_obj_tbs[0].yspeed=pcan_msg->b_4_5_IFC_YSpeedRelative*0.05-50;
		
		
	});	
	g_mh_pc.attach_monitor("obj1_4", [&](u8* pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 b0_Checksum;
			u16 b_1_2_IFC_Timestamp;
			u32 b_3_4_IFC_Length : 12;
			u32 b_4_5_IFC_height : 9;
			u32 b5_Reserved : 3;

			u32 b6_IFC_Confidence : 7;
			u32 b_6_7_Reserved: 5;
			u8 b7_LiveCounter_IFC : 4;

		};
		struct_can_msg* pcan_msg = (struct_can_msg*)pbuff;
		g_obj_tbs[0].length=pcan_msg->b_3_4_IFC_Length*0.01;
		g_obj_tbs[0].height=pcan_msg->b_4_5_IFC_height*0.01;
	});	


	
#endif
	
}
