
#include "mobileye_adas.h"
#include "navi_info.h"
#include "msg_host_n.h"
#include "af_bind.h"
#include "af_primitive_object.h"
#include "af_material.h"
#include "enum_txt_name0.h"
#include "afg.h"
#include<chrono>
#include <zklz_hud.h>
#include "coordinate_change.h"
#include "af_timer.h"
#include "uart.h"
#include <thread>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<errno.h>
#include <mutex>
#include <map>
#include <vector>
#include <math.h>
using namespace chrono;
using namespace auto_future_utilities;
using namespace std;
extern u8 navi_direct_state;
extern u8 g_gear;
#define NO_REAR g_gear!=7
extern bool navi_straight_go;
extern msg_host_n g_msg_host;
extern const char* show_lane;
extern bool be_turn_left;
extern bool be_turn_right;
extern int cs_tar;
extern af_timer g_timer;
static int restore_lane_timer;
static int restore_fcw_timer;
static int flash_failsafe_timer;
static bool show_failsafe=false;
u8 fcw_idx=0,fcw_value=0;

const char* road_primitice = "roadb.FBX0";
float ob_delay=0.3f;
bool be_hud_proj;
u8 dg_lane=0,dg_obs=0,dg_fw=0,cp_lane=1;
#define hud_proj

DefStructCoordinateChange g_from_xie;
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

//eye_2_mobile_eyex=0.36m
//eye_2_mobile_eyey=0.0658m
//eye_2_mobile_eyez=-0.538m
mutex mtx_debug_prj;
class cmutex_lock{
	mutex& _tar;
	public:
	cmutex_lock(mutex& tar)
	:_tar(tar)
	{
		_tar.lock();
	}
	~cmutex_lock()
	{
		_tar.unlock();
	}
};


volatile bool navi_state = false;
volatile u8 lane_state=en_lane_off;
volatile u8 pre_lane_state=en_lane_normal;
const char* lane_state_txt_name[2][en_lane_num]=
{
	{"road.png","ldw_left.png","ldw_right.png","trans.png"},
	{"road_navi.png", "ldw_left_navi.png", "ldw_right_navi.png","road_navi_s.png" },
};
const char* road_navi_s_txt = "road_navi_s.png";

base_ui_component* obstacles_group=nullptr;
static int obstacles_cnt = 0;

ps_mtl mtl_lane;
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
bool prohibit_lane_swith(u8 lane_type)
{
	bool ret= lane_type==en_lane_solid||lane_type==en_lane_road_edge||lane_type==en_lane_double_lane_mark||lane_type==en_lane_invalid;
	return ret;
}
enum
{
	en_model_error,
	en_linear_model,
	en_parabolic_model,
	en_3rd_degree_model,
};
u8 left_lane_type=0,right_lane_type=0;
static u8 left_lane_model_degree=0,right_lane_model_degree=0,lane_mdl=3;
static u16 left_curvature_stm = 0, left_lane_heading_stm = 0;
static u16 left_curv_dt_stm=0,right_curv_dt_stm=0;
static u16 right_curvature_stm = 0, right_lane_heading_stm = 0;
static float left_position=-1.5f,right_position=1.5f;
enum 
{
en_lt_vx,
en_lm_vx,
en_lm0_vx,
en_lm1_vx,
en_md_vx,
en_rm1_vx,
en_rm0_vx,
en_rm_vx,
en_rt_vx,
en_vx_col_num,
};
struct lane_vertex_dis
{
	 vector<int> vtx[en_vx_col_num];
	 float zdis;
	 //vector<int> lmt_vert;
	 //vector<int> rmt_vert;
	 //vector<int> rt_vert;
};
//typedef vector<int>[en_vx_col_num] lane_vertex_dis;
typedef map<int,lane_vertex_dis> mp_vert;
mp_vert s_dic_vertex;
#ifdef hud_proj
ft_textblock* pleft_debug;
ft_textblock* pright_debug;

const char* mtl_name = "mt_base";
const int numb_of_vertex = 1008;
const int len_of_verterx = 8;
static float base_road_vertexs[numb_of_vertex][len_of_verterx];
static float road_vertexs[numb_of_vertex][len_of_verterx];
//static bool road_veters_left[numb_of_vertex];
static u8 road_vx_pos[numb_of_vertex];
static float road_vx_dis[numb_of_vertex];
static float dis_unit=2.2f;
void init_raod_vertex()
{
	const float base_y=-350.f;
	const float unit_y=33.3333f;
	for (int ix = 0; ix < numb_of_vertex;ix++)
	{
		auto& vertex_u = base_road_vertexs[ix];
		//road_vx_pos[ix] = vertex_u[3] < 0;
		float vdelta=150.f-vertex_u[3];
		if(vdelta<0.00001)
		{
			road_vx_pos[ix]=en_rt_vx;
		}
		else if(vdelta<76.f)
		{ 
			road_vx_pos[ix]=en_rm_vx;
		}
		else if(vdelta<101.f)
		{ 
			road_vx_pos[ix]=en_rm0_vx;
		}		
		else if(vdelta<126.f)
		{ 
			road_vx_pos[ix]=en_rm1_vx;
		}		
		else if(vdelta<151.f)
		{
			road_vx_pos[ix]=en_md_vx;
		}
		else if(vdelta<176.f)
		{
			road_vx_pos[ix]=en_lm1_vx;
		}
		else if(vdelta<201.f)
		{
			road_vx_pos[ix]=en_lm0_vx;
		}
		else if(vdelta<226.f)
		{
			road_vx_pos[ix]=en_lm_vx;
		}
		else
		{
			road_vx_pos[ix]=en_lt_vx;
		}
		float dis_rf=vertex_u[4]-base_y;
		int numb_unit=dis_rf/unit_y;
		road_vx_dis[ix]=numb_unit*10.f;
		const auto& idic=s_dic_vertex.find(numb_unit);
		if(idic!=s_dic_vertex.end())
		{
			auto& lvertx=idic->second;
			lvertx.vtx[road_vx_pos[ix]].push_back(ix);
		}
		else
		{
			lane_vertex_dis nvtx;
			nvtx.vtx[road_vx_pos[ix]].push_back(ix);
			nvtx.zdis=13.f+numb_unit*dis_unit+numb_unit*numb_unit*0.05f;
			s_dic_vertex[numb_unit]=nvtx;
		}
		//s_dic_vertex[idis].push_pack(ix);
	}
}
vector<POINT3D> i_points;
#define PMDEF
bool calcu_vertex_ex(float curv_lt_def, float curvature_left, float lane_heading_left,float left_base, float curv_rt_def,float curvature_right, float lane_heading_right,float right_base)
{

	 vector<POINT> o_points;

	i_points.resize(numb_of_vertex);
	//o_points.resize(numb_of_vertex);
	float rc0,rc1,rc2,rc3,z;
	float lc0,lc1,lc2,lc3;
	
       float car_speed=(float)cs_tar*0.2778;
	int icnt=0;
	static int  imax=0;
	auto imx_c=s_dic_vertex.size()-1;
	imax++;
	imax%=imx_c;
	#ifdef PMDEF
	rc0=right_base;
	rc1=lane_heading_right;
	rc2=curvature_right;
	rc3=0.f;
	lc0=left_base;
	lc1=lane_heading_left;
	lc2=curvature_left;
	lc3=0.f;
	
	#else
	function<void(float, float&,float&,float&,float&)> f_clt_rlane_cf;
	if(lane_mdl<5)
	{
		right_lane_model_degree=left_lane_model_degree=lane_mdl;
	}
	if(right_lane_model_degree==en_linear_model)
	{
		f_clt_rlane_cf=[&](float z,float& c0,float& c1,float& c2,float& c3){
			c0=right_base;
			c1=3*curv_rt_def*z*z+2*curvature_right*z+lane_heading_right;
			c2=0;
			c3=0;
		};
	}
	else if(right_lane_model_degree==en_parabolic_model)
	{
		f_clt_rlane_cf=[&](float z,float& c0,float& c1,float& c2,float& c3){
			c0=right_base;
			c1=3*curv_rt_def*z*z+2*curvature_right*z+lane_heading_right;
			c2=6*curv_rt_def*z+2*curvature_right;
			c3=0;
		};
	}
	else if(right_lane_model_degree==en_3rd_degree_model)
	{
		f_clt_rlane_cf=[&](float z,float& c0,float& c1,float& c2,float& c3){
			c0=right_base;
			c1=3*curv_rt_def*z*z+2*curvature_right*z+lane_heading_right;
			c2=6*curv_rt_def*z+2*curvature_right;
			c3=6*curv_rt_def;
		};
	}
	else
	{
		f_clt_rlane_cf=[&](float z,float& c0,float& c1,float& c2,float& c3){
			c0=right_base;
			c1=c2=c3=0.f;
		};
	}
	function<void(float, float&,float&,float&,float&)> f_clt_llane_cf;
	if(left_lane_model_degree==en_linear_model)
	{
		f_clt_llane_cf=[&](float z,float& c0,float& c1,float& c2,float& c3){
			c0=left_base;
			c1=3*curv_lt_def*z*z+2*curvature_left*z+lane_heading_left;
			c2=0;
			c3=0;
		};
	}
	else if(left_lane_model_degree==en_parabolic_model)
	{
		f_clt_llane_cf=[&](float z,float& c0,float& c1,float& c2,float& c3){
			c0=left_base;
			c1=3*curv_lt_def*z*z+2*curvature_left*z+lane_heading_left;
			c2=6*curv_lt_def*z+2*curvature_left;
			c3=0;
		};
	}
	else if(left_lane_model_degree==en_3rd_degree_model)
	{
		f_clt_llane_cf=[&](float z,float& c0,float& c1,float& c2,float& c3){
			c0=left_base;
			c1=3*curv_lt_def*z*z+2*curvature_left*z+lane_heading_left;
			c2=6*curv_lt_def*z+2*curvature_left;
			c3=6*curv_lt_def;
		};
	}
	else
	{
		f_clt_llane_cf=[&](float z,float& c0,float& c1,float& c2,float& c3){
			c0=left_base;
			c1=c2=c3=0.f;
		};
	}
	#endif
	//float ddz=0;
	for(auto& dic_u:s_dic_vertex)
	{
		//auto& numb_unit=dic_u.first;
		auto& ivtx=dic_u.second;
		z=ivtx.zdis;
		#ifdef PMDEF

		float rx=rc2*z*z+rc1*z+rc0;
		float lx=lc2*z*z+lc1*z+lc0;
		#else
		f_clt_rlane_cf(z,rc0,rc1,rc2,rc3);		
		float rx=rc3*z*z*z+rc2*z*z+rc1*z+rc0;
		f_clt_llane_cf(z,lc0,lc1,lc2,lc3);
		float lx=lc3*z*z*z+lc2*z*z+lc1*z+lc0;
		
		#endif
		if(cp_lane)
		{
 			float carspeed=car_speed*0.6f;
			float ragl=atan(rc1);
			float rvx=carspeed*sin(ragl);
			float rvz=carspeed*cos(ragl);	
			float lagl=atan(lc1);
			float lvx=carspeed*sin(lagl);
			float lvz=carspeed*cos(lagl);
			float dz=(lvz+rvz)*0.5f*ob_delay;
			float drx=rvx*ob_delay;
			float dlx=lvx*ob_delay;
			z=z+dz;
			rx=rx+drx;
			lx=lx+dlx;
		}	
		if( lx>=rx)
		{ 
			printf("lx=%f,rx=%f\n",lx,rx);
			return false;
		}
		//float hdis=lx-rx;
		rx=-rx;
		lx=-lx;
		float hdis=rx-lx;
		/*if(icnt==imax)
		{
			printf("hdis=%f\n",hdis);
		}*/
		const float max_hdis=-3.75f;
		if(hdis<max_hdis)
		{
			hdis=max_hdis;
			rx=lx+max_hdis;
		}
		for(auto& rt_vtx:ivtx.vtx[en_rt_vx])
		{
		       i_points[rt_vtx].z=z;
		       i_points[rt_vtx].y=0.f;
		       i_points[rt_vtx].x=rx;
		}
		
		for(auto& rt_vtx:ivtx.vtx[en_lt_vx])
		{
		       i_points[rt_vtx].z=z;
		       i_points[rt_vtx].y=0.f;
		       i_points[rt_vtx].x=lx;
		}

		float hunit=hdis*0.25f;
		float lm=lx+hunit;
		float rm=rx-hunit;
		float munit=(rm-lm)/6;
		float lm0=lm+munit;
		float lm1=lm0+munit;
		float md=lm1+munit;
		float rm1=md+munit;
		float rm0=rm1+munit;
 
		for(auto& rt_vtx:ivtx.vtx[en_lm_vx])
		{
		       int ix=rt_vtx;
			i_points[ix].y=0;
			i_points[ix].z=z;
		       i_points[ix].x=lm;
		}
		
		for(auto& rt_vtx:ivtx.vtx[en_lm0_vx])
		{
		       int ix=rt_vtx;
			i_points[ix].y=0;
			i_points[ix].z=z;
		       i_points[ix].x=lm0;
		}
		
		for(auto& rt_vtx:ivtx.vtx[en_lm1_vx])
		{
		       int ix=rt_vtx;
			i_points[ix].y=0;
			i_points[ix].z=z;
		       i_points[ix].x=lm1;
		}
		
		for(auto& rt_vtx:ivtx.vtx[en_md_vx])
		{
		       int ix=rt_vtx;
			i_points[ix].y=0;
			i_points[ix].z=z;
		       i_points[ix].x=md;
		}
		for(auto& rt_vtx:ivtx.vtx[en_rm1_vx])
		{
		       int ix=rt_vtx;
			i_points[ix].y=0;
			i_points[ix].z=z;
		       i_points[ix].x=rm1;
		}		
		for(auto& rt_vtx:ivtx.vtx[en_rm0_vx])
		{
		       int ix=rt_vtx;
			i_points[ix].y=0;
			i_points[ix].z=z;
		       i_points[ix].x=rm0;
		}		
		for(auto& rt_vtx:ivtx.vtx[en_rm_vx])
		{
		       int ix=rt_vtx;
			i_points[ix].y=0;
			i_points[ix].z=z;
		       i_points[ix].x=rm;
		}
	}

	mtx_debug_prj.lock();
	HUD_ProjectPoints3d(i_points,o_points);
	mtx_debug_prj.unlock();
	//static int sss=0;
	for(int ix=0;ix<numb_of_vertex;ix++)
	{
		road_vertexs[ix][3]=o_points[ix].x;//*hscale;
		road_vertexs[ix][4]=-o_points[ix].y;//*vscale;//*3.8f;
	}
	return true;
}


static float bk_left_position,bk_right_position;
static u16 bk_left_curv_dt_stm,bk_left_curvature_stm,bk_left_lane_heading_stm;
static u16 bk_right_curv_dt_stm,bk_right_curvature_stm,bk_right_lane_heading_stm;
void backup_lane_params()
{
	 bk_left_curv_dt_stm=left_curv_dt_stm;bk_left_curvature_stm=left_curvature_stm;bk_left_lane_heading_stm=left_lane_heading_stm;bk_left_position=left_position;
	 bk_right_curv_dt_stm=right_curv_dt_stm;bk_right_curvature_stm=right_curvature_stm;bk_right_lane_heading_stm=right_lane_heading_stm;bk_right_position=right_position;
	 
}
void restore_lane_params()
{
	 left_curv_dt_stm=bk_left_curv_dt_stm;left_curvature_stm=bk_left_curvature_stm;left_lane_heading_stm=bk_left_lane_heading_stm;left_position=bk_left_position;
	 right_curv_dt_stm=bk_right_curv_dt_stm;right_curvature_stm=bk_right_curvature_stm;right_lane_heading_stm=bk_right_lane_heading_stm;right_position=bk_right_position;
	// printf("left_lane(%x,%x,%x,%f) right_lane(%x,%x,%x,%f)\n",left_curv_dt_stm,left_curvature_stm,left_lane_heading_stm,left_position,right_curv_dt_stm,right_curvature_stm,right_lane_heading_stm,right_position);

}
void init_lane_params()
{
	left_curv_dt_stm=left_curvature_stm=left_lane_heading_stm=right_curv_dt_stm=right_curvature_stm=right_lane_heading_stm=0x7fff;
	left_position=-1.5f;
	right_position=1.5f;
}
void calcu_left_right_lanes()
{
	if (left_curvature_stm != 0 && left_lane_heading_stm != 0 && right_curvature_stm != 0 && right_lane_heading_stm!=0)
	{
	       float cuv_lt_df=(left_curv_dt_stm-0x7fff)/(1<<28);
		float cuv_lt = (left_curvature_stm - 0x7fff) / 1024000.0;
		float lh_lt = (left_lane_heading_stm - 0x7fff) / 1024.0;
		
	       float cuv_rt_df=(right_curv_dt_stm-0x7fff)/(1<<28);
		float cuv_rt = (right_curvature_stm - 0x7fff) / 1024000.0;
		float lh_rt = (right_lane_heading_stm - 0x7fff) / 1024.f;		
		float left_base=left_position;
		float right_base=right_position;
		bool be_valid=calcu_vertex_ex(cuv_lt_df,cuv_lt, lh_lt, left_base,cuv_rt_df,cuv_rt,lh_rt,right_base);
		if(be_valid)
		{
			backup_lane_params();
			if(dg_lane)
			   printf("llane(%x,%x,%x,%x,%f) rlane(%x,%x,%x,%x,%f)\n",left_lane_model_degree,left_curv_dt_stm,left_curvature_stm,left_lane_heading_stm,left_position,\
			   right_lane_model_degree,right_curv_dt_stm,right_curvature_stm,right_lane_heading_stm,right_position);
			update_prm_vbo(road_primitice, road_vertexs[0], numb_of_vertex * len_of_verterx);
		}
		else
		{
			printf("abadoned\n");
		}
		//set_property_aliase_value(show_lane, &be_valid);
		left_curvature_stm = left_lane_heading_stm = right_curvature_stm = right_lane_heading_stm = 0;
	}
}
#else
const char* mtl_name = "mt_sp";
void calcu_left_right_lanes()
{
	if (left_curvature_stm != 0 && left_lane_heading_stm != 0 && right_curvature_stm != 0 && right_lane_heading_stm!=0)
	{
		float cuv_lt = (left_curvature_stm - 0x7fff) / 1024000.0;
		float lh_lt = (left_lane_heading_stm - 0x7fff) / 1024.0;
		float cuv_rt = (right_curvature_stm - 0x7fff) / 1024000.0;
		float lh_rt = (right_lane_heading_stm - 0x7fff) / 1024.f;
		float left_base=-left_position*100.f;
		float right_base=-right_position*100.f;
		float of_eye=30.f;
		bool be_cross=cuv_lt>0&&cuv_rt<0;
		bool be_departure=cuv_lt<0&&cuv_rt>0;
		//if (1)//(cuv_lt > 0.0 && cuv_rt > 0.0 || cuv_lt < 0 && cuv_rt < 0)
		{
			printf("cuv_lt=%f,lh_lt=%f,ofs_l=%f,cuv_rt=%f,lh_rt=%f,ofs_r=%f\n", cuv_lt, lh_lt,left_position, cuv_rt, lh_rt,right_position);
			
			mtl_lane->set_value("curv_l", &cuv_lt, 1);
			mtl_lane->set_value("head_l", &lh_lt, 1);
			mtl_lane->set_value("curv_r", &cuv_rt, 1);
			mtl_lane->set_value("head_r", &lh_rt, 1);
			mtl_lane->set_value("left_base", &left_base, 1);
			mtl_lane->set_value("right_base", &right_base, 1);
			mtl_lane->set_value("of_eye", &of_eye, 1);
			left_curvature_stm = left_lane_heading_stm = right_curvature_stm = right_lane_heading_stm = 0;
		}
	}
}
#endif
//static const char* mtl_name = "mt_sp";
enum en_obstacle_type
{
	en_vehichle,
	en_truck,
	en_bike,
	en_ped,
	en_bicycle,
	en_unsued,
	en_unused1,
	en_unused2,
	en_obstacle_type_num
};
enum en_obstacle_status
{
	en_ob_undefined,
	en_ob_standing,
	en_ob_stopped,
	en_ob_moving,
	en_ob_oncoming,
	en_ob_parked,
	en_ob_unused,
};
#define IS_MOVING(x) (x==en_ob_moving||x==en_ob_oncoming)

int obstacle_icon[en_obstacle_type_num]=
{
en_obstacle_v_png,
en_obstacle_v_png,
en_obstacle_v_png,
en_obstacle_p_png,
en_obstacle_v_png,
en_obstacle_v_png,
en_obstacle_v_png,
en_obstacle_v_png,
};
enum en_obstacle_state
{
	en_ob_invisible,
	en_ob_visible,
	en_ob_left_lost,
	en_ob_right_lost,
};
struct st_obstacle_state
{
	u8 _obs_tp;
	en_obstacle_state _obs_st;
};
#define MAX_OBS_CNT 15
st_obstacle_state obstacle_states[MAX_OBS_CNT];
s16 obstacle_accel_x[MAX_OBS_CNT]={0};
void notify_hide_obstacle()
{
	bool bvalue=false;
	set_property_aliase_value("show_left_ob", &bvalue);
	set_property_aliase_value("show_right_ob", &bvalue);
	for(int ix=0;ix<obstacles_cnt;ix++)
	{
		auto& obj_tar=obstacle_states[ix];
		if(obj_tar._obs_st==en_ob_left_lost)
		{
			set_property_aliase_value("control_left_ob",&obstacle_icon[obj_tar._obs_tp]);
			bvalue=true;
			set_property_aliase_value("show_left_ob", &bvalue);
		}
		else
		if(obj_tar._obs_st==en_ob_right_lost)
		{
			set_property_aliase_value("control_right_ob",&obstacle_icon[obj_tar._obs_tp]);
			bvalue=true;
			set_property_aliase_value("show_right_ob", &bvalue);

		}
	}
}
bool handle_obstacle_state(int obj_id,u8 obj_type, float x,float y,float sz,bool hided)
{
	 assert(obj_id<MAX_OBS_CNT);
	auto& obj_tar=obstacle_states[obj_id];
	obj_tar._obs_tp=obj_type;
	//printf("_obs_tp=%d\n",obj_type);
	if(hided)
	{
		obj_tar._obs_st= en_ob_invisible;
	}
	else if(x>-400.f&&(x+sz)<400.f&&y>-133.5f&&(y+sz)<133.5f)
	{
		obj_tar._obs_st= en_ob_visible;
	}
	else
	{
		if(x<0)
			obj_tar._obs_st=  en_ob_left_lost;
		else
			obj_tar._obs_st=  en_ob_right_lost;
	}	
	notify_hide_obstacle();
	return obj_tar._obs_st== en_ob_visible;
}
void hide_obsacle(int obj_id)
{
       if(obj_id==-1)
       {
		for(int ix=0;ix<MAX_OBS_CNT;ix++)
		{
			obstacle_states[ix]._obs_st=en_ob_invisible;
		}
	}
	else
	{
	 	assert(obj_id<MAX_OBS_CNT);
		obstacle_states[obj_id]._obs_st=en_ob_invisible;
	}
	notify_hide_obstacle();
}

const float max_pre_ct=20.f;
struct obstacle_unit
{
	steady_clock::time_point _tp;
	bool _recording{false};
	float _prv_x{0.f}, _prv_y{0.f};
	float _prv_x_draw{0};
	float _pre_collision_time{max_pre_ct};
};
const int max_obstacle_cnt=15;
obstacle_unit obs_list[max_obstacle_cnt];
float min_obstacle_collision_time()
{
	if(obstacles_cnt==0)
		return max_pre_ct;
	float min_oc=max_pre_ct;
	for(int ix=0;ix<obstacles_cnt;ix++)
	{
		if(obs_list[ix]._pre_collision_time<min_oc)
		{
			min_oc=obs_list[ix]._pre_collision_time;
		}
	}
	return min_oc;
}
const char* fifo_path="/tmp/fifo_fy";
int fd_fifo=0;
bool recieve_cmd=false;

float eula_x=-2.8f,eula_y=0.0f,eula_z=0;
float ect_x=0.51f,ect_y=1.3f,ect_z=-1.1f;
bool update_lane_caculate=false;

base_ui_component* uilocation=nullptr;
float ui_xpos=-300.f,ui_ypos=24.f;
bool update_loc_pos=false;
static bool be_adj_fov = false;

void set_eula_x_ect_y(float eulax,float ecty)
{
	eula_x=eulax;
	ect_y=ecty;
	cmutex_lock mtk(mtx_debug_prj);
	HUD_SetEular(eula_x,eula_y,eula_z);
	HUD_SetEyeCenter(ect_x,ect_y,ect_z);
	update_lane_caculate=true;
}
void adas_cmd_update()
{
	if(update_lane_caculate)
	{
		update_lane_caculate=false;
		restore_lane_params();
		printf("refrash lanes\n");
		calcu_left_right_lanes();
	}
	if(update_loc_pos)
	{
		update_loc_pos=false;
		set_property_aliase_value("show_location", &be_adj_fov);
		uilocation->set_base_pos(ui_xpos-16,ui_ypos-16);
		printf("loc posx=%f, posy=%f\n",ui_xpos,ui_ypos);
	}
}


void register_adas_cmd_handl()
{

	uilocation=get_aliase_ui_control("show_location");

	HUD_SetOutputSize(800,267);
	HUD_SetEular(eula_x,eula_y,eula_z);//(-2.6,-0.5,0);//(-2.6f,-2.0f,0);//HUD_SetEular(-2.6,0,0);// HUD_SetEular(0,0,0);
	HUD_SetEyeCenter(ect_x,ect_y,ect_z);//HUD_SetEyeCenter(-0.34, 1.333, 0.0);//HUD_SetEyeCenter(0.0, 1.0, 0.0)

	Coordinate_Change_Init(&g_from_xie);
	int ret=-1;
	if(access(fifo_path,F_OK))
	{
		ret=mkfifo(fifo_path,0777);
		if(ret==-1)
		{
			printf("fail to create fifo:%s\n",fifo_path);
		}
	}
	else
	{
		ret=1;
	}
	#if 1
	if(ret!=-1)
	{
		thread th_fifo([&](){
			#define FF_BUFF_LEN 0x100
			char f_buff[FF_BUFF_LEN];
			 fd_fifo=open(fifo_path,O_RDONLY);
			if(fd_fifo==-1)
			{
				printf("fail to open fifo:%s\n",fifo_path);
				return;
				
			}
			recieve_cmd=true;
			while(recieve_cmd)
			{
				wait_fd_read_eable(fd_fifo);
				int n=read(fd_fifo,f_buff,FF_BUFF_LEN);
				if(n>0)
				{
					f_buff[n]='\0';
					printf("recieve cmd:%s\n",f_buff);
					if(strncmp(f_buff,"eula_x=",strlen("eula_x="))==0)
					{
						eula_x=atof(f_buff+strlen("eula_x="));
						printf("eula_x=%f\n",eula_x);
						cmutex_lock mtk(mtx_debug_prj);
						HUD_SetEular(eula_x,eula_y,eula_z);
						update_lane_caculate=true;
					}
					else
					if(strncmp(f_buff,"eula_y=",strlen("eula_y="))==0)
					{
						eula_y=atof(f_buff+strlen("eula_y="));
						cmutex_lock mtk(mtx_debug_prj);
						HUD_SetEular(eula_x,eula_y,eula_z);
						update_lane_caculate=true;
					}
					else
					if(strncmp(f_buff,"eula_z=",strlen("eula_z="))==0)
					{
						eula_z=atof(f_buff+strlen("eula_z="));
						cmutex_lock mtk(mtx_debug_prj);
						HUD_SetEular(eula_x,eula_y,eula_z);
						update_lane_caculate=true;
					}
					else
					if(strncmp(f_buff,"ect_x=",strlen("ect_x="))==0)
					{
						ect_x=atof(f_buff+strlen("ect_x="));
						cmutex_lock mtk(mtx_debug_prj);
						HUD_SetEyeCenter(ect_x,ect_y,ect_z);
						update_lane_caculate=true;
					}
					else
					if(strncmp(f_buff,"ect_y=",strlen("ect_y="))==0)
					{
						ect_y=atof(f_buff+strlen("ect_x="));
						cmutex_lock mtk(mtx_debug_prj);
						HUD_SetEyeCenter(ect_x,ect_y,ect_z);
						update_lane_caculate=true;
					}
					else
					if(strncmp(f_buff,"ect_z=",strlen("ect_z="))==0)
					{
						ect_z=atof(f_buff+strlen("ect_z="));
						cmutex_lock mtk(mtx_debug_prj);
						HUD_SetEyeCenter(ect_x,ect_y,ect_z);
						update_lane_caculate=true;
					}
					else
					if(strncmp(f_buff,"show_loc=",strlen("show_loc="))==0)
					{
						int show_loc=atoi(f_buff+strlen("show_loc="));
						be_adj_fov=show_loc!=0;
						update_loc_pos=true;
					}
					else
					if(strncmp(f_buff,"loc_xadd=",strlen("loc_xadd="))==0)
					{
						float xloc=atof(f_buff+strlen("loc_xadd="));
						ui_xpos+=xloc;
						update_loc_pos=true;
					}
					else
					if(strncmp(f_buff,"loc_yadd=",strlen("loc_yadd="))==0)
					{
						float yloc=atof(f_buff+strlen("loc_yadd="));
						ui_ypos+=yloc;
						update_loc_pos=true;
					}
					else
					if(strncmp(f_buff,"ob_delay=",strlen("ob_delay="))==0)
					{
						ob_delay=atof(f_buff+strlen("ob_delay="));
					}
					else
					if(strncmp(f_buff,"dg_lane=",strlen("dg_lane="))==0)
					{
						dg_lane=atoi(f_buff+strlen("dg_lane="));
					}
					else
					if(strncmp(f_buff,"dg_obs=",strlen("dg_obs="))==0)
					{
						dg_obs=atoi(f_buff+strlen("dg_obs="));
					}
					else
					if(strncmp(f_buff,"dg_fw=",strlen("dg_fw="))==0)
					{
						dg_fw=atoi(f_buff+strlen("dg_fw="));
					}
					else
					if(strncmp(f_buff,"cp_lane=",strlen("cp_lane="))==0)
					{
						cp_lane=atoi(f_buff+strlen("cp_lane="));
					}
					else
					if(strncmp(f_buff,"lane_mdl=",strlen("lane_mdl="))==0)
					{
						lane_mdl=atoi(f_buff+strlen("lane_mdl="));
					}				
				}
			}
			close(fd_fifo);
		});
		th_fifo.detach();
	}
      
	printf("go on..........\n");
	#endif
	const auto& imtl = g_material_list.find("mt_sp");
	assert(imtl != g_material_list.end());
	mtl_lane=imtl->second;
	//uf_value = 350.f;
	//mtl_lane->set_value("base_y", &uf_value, 1);
	#ifdef hud_proj
	if (get_prm_data(road_primitice, &road_vertexs[0][0], numb_of_vertex * len_of_verterx))
	{
		memcpy(&base_road_vertexs[0][0], road_vertexs, numb_of_vertex * len_of_verterx * sizeof(float));
		init_raod_vertex();
		init_lane_params();
		calcu_left_right_lanes();
		/*auto pvalue_gear=get_aliase_ui_control("value_gear");
		pleft_debug=(ft_textblock*)pvalue_gear->get_child(0);
		pright_debug=(ft_textblock*)pvalue_gear->get_child(1);
		*/
	}
	be_hud_proj=true;
	#else
	be_hud_proj=false;
	#endif
	set_property_aliase_value("show_scene_base", &be_hud_proj);
	obstacles_group = get_aliase_ui_control("show_obstacles");

	restore_lane_timer= g_timer.register_timer([&](int tick){
		if (tick==0)
		{
			pre_lane_state=en_lane_normal;
			set_mp_text_uf(mtl_name, "text", lane_state_txt_name[navi_state][pre_lane_state]);
			g_timer.deactive_time(restore_lane_timer);
		}

	});
	restore_fcw_timer=g_timer.register_timer([&](int tick){
				bool be_show=false;
				char seg_name[50] = "";
				sprintf(seg_name, "show_distance%d", fcw_idx);
				set_property_aliase_value(seg_name, &be_show);
				if(fcw_idx==fcw_value)
				{
					g_timer.deactive_time(restore_fcw_timer);
				}
				fcw_idx++;
				
	});
	flash_failsafe_timer=g_timer.register_timer([&](int tick){
				show_failsafe=!show_failsafe;
				set_property_aliase_value("show_failsafe", &show_failsafe);
	});
	g_msg_host.attach_monitor("can message", [&](u8* pbuff, int len){
		//printf("can message:\n");
		//print_buff(pbuff, len);
		pbuff += 2;
		enum
		{
			en_silent,
			en_ldw_left,
			en_ldw_right,
			en_hw1,
			en_tsr,
			en_ufcw,
			en_fcw_pcw,
		};
		struct GNU_DEF can_msg
		{
			u8 b0_sound_type : 3;
			u8 b0_time_indicator : 2;
			u8 b0_undocumented : 3;

			u8 b1_0 : 1;
			u8 b1_reserved_n : 4;
			u8 b1_zero_spped : 1;
			u8 b1_reserved : 2;

			u8 b2_headway_valid : 1;
			u8 b2_headway_measurement : 7;

			u8 b3_error : 1;
			u8 b3_error_code : 7;

			u8 b4_ldw_off : 1;
			u8 b4_left_ldw_on : 1;
			u8 b4_right_ldw_on : 1;
			u8 b4_fcw_on : 1;
			u8 b4_undocumented : 2;
			u8 b4_maintenance : 1;
			u8 b4_failsafe : 1;

			u8 b5_zero : 1;
			u8 b5_peds_fcw : 1;
			u8 b5_peds_in_dz : 1;
			u8 b5_reserved2 : 2;
			u8 b5_tamper_alert : 1;
			u8 b5_reseved : 1;
			u8 b5_tsr_enabled : 1;

			u8 b6_tsr_warning_level : 3;
			u8 b6_reserved : 5;

			u8 b7_headway_warning_level : 2;
			u8 b7_hw_repeatable_enabled : 1;
			u8 b7_reserved : 5;
		};
		can_msg* pcan_msg = (can_msg*)pbuff;
		if(pcan_msg->b4_failsafe)
		{
			printf("fail safe....\n");
			if(!g_timer.is_actived_timer(flash_failsafe_timer))
			{
				show_failsafe=true;
				set_property_aliase_value("show_failsafe", &show_failsafe);
			}
			g_timer.active_timer(flash_failsafe_timer, 74, 37);
		}
		else
		{
			show_failsafe=false;
			set_property_aliase_value("show_failsafe", &show_failsafe);
			g_timer.deactive_time(flash_failsafe_timer);
		}
		if(pcan_msg->b5_tamper_alert)
		{
			printf("tamper alert\n");
		}
		bool be_show;
		u8 head_way_mm = pcan_msg->b2_headway_measurement;
		if(dg_fw)
		{
			printf("sound_type:%d,headway_mm:%d,headway_valid=%d fcw=%d hw_w_l=%d\n",pcan_msg->b0_sound_type,\
				head_way_mm,pcan_msg->b2_headway_valid,pcan_msg->b4_fcw_on,pcan_msg->b7_headway_warning_level);
		}
		float min_os=min_obstacle_collision_time();
		//min_os=min_os*10;
		//if (pcan_msg->b2_headway_valid&&head_way_mm<60&&min_os<6.f&&NO_REAR)
		//sound type==en_ufcw or en_fcw_pcw
		if (head_way_mm<60&&min_os<6.f&&NO_REAR)
		{
			int dist_id = 0;
			if(head_way_mm==0)
			{
				head_way_mm=min_os*10;
			}
			static u8 segs[] = { 24, 20, 16, 12, 8, 4 };
			
			for (; dist_id < sizeof(segs); dist_id++)
			{
				if (head_way_mm > segs[dist_id])
				{
					break;
				}
			}
			//dist_id--;
			if(dist_id>5)
				dist_id=5;
			if(dg_fw)
			printf("head_way_mm=%d,dist_id=%d\n",head_way_mm,dist_id);
			for (int ix = 0; ix < 6;ix++)
			{
				be_show = ix<=dist_id;
				char seg_name[50] = "";
				sprintf(seg_name, "show_distance%d", ix);
				set_property_aliase_value(seg_name, &be_show);
			}
			fcw_idx=0;
			fcw_value=dist_id;
			g_timer.active_timer(restore_fcw_timer, 52, 7);
			
		}
		else
		{
			for (int ix = 0; ix < 6; ix++)
			{
				be_show =false;
				char seg_name[50] = "";
				sprintf(seg_name, "show_distance%d", ix);
				set_property_aliase_value(seg_name, &be_show);
			}
		}
		if (navi_direct_state==en_navi_no_direction)
		{
			//lane_state=en_lane_normal;
			if (pcan_msg->b4_ldw_off)
			{
				pre_lane_state=en_lane_normal;
			}
			else
			{
				if ((pcan_msg->b0_sound_type == 1 || pcan_msg->b4_left_ldw_on)
					&& pcan_msg->b4_right_ldw_on == 0 && !be_turn_left)
				{
				       pre_lane_state=en_lane_ldw_left;
					 g_timer.active_timer(restore_lane_timer, 42, 1);
				}
				else
				if ((pcan_msg->b0_sound_type == 2 || pcan_msg->b4_right_ldw_on)
					&& pcan_msg->b4_left_ldw_on == 0 && !be_turn_right)
				{
				      pre_lane_state=en_lane_ldw_right;
					g_timer.active_timer(restore_lane_timer, 42, 1);
				}
			}
			set_mp_text_uf(mtl_name, "text", lane_state_txt_name[navi_state][pre_lane_state]);
			//pre_lane_state=lane_state;
		}
		else
		{
			//printf("navi_direct_state=%d\n",navi_direct_state);
		}
		if(pcan_msg->b4_fcw_on==0||pcan_msg->b0_sound_type!=en_ufcw&&pcan_msg->b0_sound_type!=en_fcw_pcw)
		{
			hide_obsacle(-1);
		}
		
		/*be_show = pcan_msg->b5_peds_in_dz;
		if (pcan_msg->b5_peds_in_dz)
		{
			be_show = true;
			if (pcan_msg->b5_peds_fcw)
			{
				set_property_aliase_value("peds_war2", &be_show);
			}
			else
			{
				set_property_aliase_value("peds_war1", &be_show);
			}
		}
		else
		{
			be_show = false;
			set_property_aliase_value("peds_war1", &be_show);
			set_property_aliase_value("peds_war2", &be_show);
		}*/
		//set_property_aliase_value("")
	});
       g_msg_host.attach_monitor("car info",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF car_info
		{
			u8 b0_brakes:1;
			u8 b0_left_signal:1;
			u8 b0_right_signal:1;
			u8 b0_wipers:1;
			u8 b0_low_beam:1;
			u8 b0_high_beam:1;
			u8 b0_resvered1 : 1;
			u8 b0_reserved:1;
			
			u8 b1_reserved2:3;
			u8 b1_wipers_available:1;
			u8 b1_low_beam_available:1;
			u8 b1_high_beam_available:1;
			u8 b1_reserved : 1;
			u8 b1_speed_available:1;

			u8 b2_speed;
			u8 b3_reserved;
			u8 b4_reserved;
			u8 b5_reserved;
			u8 b6_reserved;
			u8 b7_reserved;
		};
		car_info* pcar_info=(car_info*)pbuff;
	});	
    g_msg_host.attach_monitor("tsr message",[&](u8* pbuff,int len){
	   	u16* ptsr_id=(u16*)pbuff;
		pbuff+=2;
		struct GNU_DEF tsr_message
		{
			u8 b0_vision_only_sign_type;
			u8 b1_vision_only_supplementary_sign_type;
			u8 b2_sign_position_x;
			u8 b3_sign_position_y:7;
			u8 b3_na:1;
			u8 b4_position_z:6;
			u8 b4_na:2;
			u8 b5_filter_type;
			u8 b6_na;
			u8 b7_na;
		};
		tsr_message* ptsr_message=(tsr_message*)pbuff;
	});	
    g_msg_host.attach_monitor("tsr display",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF tsr_display
		{
			u8 b0_vision_only_sign_type_display1;
			u8 b1_vision_only_supplementary_sign_type_display1;
			u8 b2_vision_only_sign_type_display2;
			u8 b3_vision_only_supplementary_sign_type_display2;
			u8 b4_vision_only_sign_type_display3;
			u8 b5_vision_only_supplementary_sign_type_display3;
			u8 b6_vision_only_sign_type_display4;
			u8 b7_vision_only_supplementary_sign_type_display4;
			
		};
		tsr_display* ptsr_display=(tsr_display*)pbuff;
	});
    g_msg_host.attach_monitor("details of lane",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF details_of_lane
		{
			u8 b0_lane_confidence_left:2;
			u8 b0_ldw_available_left:1;
			u8 b0_undocumented : 1;
			u8 b0_lane_type_left:4;
			
			u8 b1_reserved : 4;
			u8 b1_distance_to_left_lane_lsb:4;
			
			u8 b2_distance_to_left_lane_msb:7;
			u8 b2_distance_to_left_lane_sg : 1;
			
			u8 b3_resreved;
			u8 b4_reserved;
			
			u8 b5_lane_confidence_right:2;
			u8 b5_ldw_available_right:1;
			u8 b5_undocumented:1;
			u8 b5_lane_type_right:4;
			
			u8 b6_reserved : 4;
			u8 b6_distance_to_right_lane_lsb:4;
			
			u8 b7_distance_to_right_lane_msb:7;
			u8 b7_distance_to_right_lane_sg : 1;
			
		};
		details_of_lane* pdata=(details_of_lane*)pbuff;
		left_lane_type = pdata->b0_lane_type_left;
		right_lane_type = pdata->b5_lane_type_right;
		s16 sleft_lane_dis = pdata->b2_distance_to_left_lane_msb * 16 + pdata->b1_distance_to_left_lane_lsb;
		if (pdata->b2_distance_to_left_lane_sg)
		{
			sleft_lane_dis = -sleft_lane_dis;
		}
		s16 sright_lane_dis = pdata->b7_distance_to_right_lane_msb * 16 + pdata->b6_distance_to_right_lane_lsb;
		if (pdata->b7_distance_to_right_lane_sg)
		{
			sright_lane_dis = -sright_lane_dis;
		}
		//printf("sleft_lane_dis=%d,sright_lane_dis=%d\n", sleft_lane_dis, sright_lane_dis);

	});
    g_msg_host.attach_monitor("lka left lane a",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF lka_left_lane_a
		{
			u8 b0_lane_type : 4;
			u8 b0_quality:2;
			u8 b0_model_degree:2;

			s16 b12_position_c0;

			
			u8 b3_curvature_c2_byte0;

			u8 b4_curvature_c2_byte1;

			u8 b5_derivative_c3_byte0;

			u8 b6_derivative_c3_byte1;

			u8 b7_width_left_marking;			
		};
		
		lka_left_lane_a* pdetails_lane = (lka_left_lane_a*)pbuff;
		//printf("left lane curve quality=%d lane_type=%d\n",pdetails_lane->b0_quality,pdetails_lane->b0_lane_type);
		bool bvalue=navi_direct_state==en_navi_no_direction;
		if(bvalue)
		{
			bool navi_turn=!bvalue;
			set_property_aliase_value("show_left_bend", &navi_turn);			
			set_property_aliase_value("show_navi_rt2lt", &navi_turn);	
			set_property_aliase_value("show_right_bend", &navi_turn);
			set_property_aliase_value("show_navi_lt2rt", &navi_turn);
		}
		bool low_quality=pdetails_lane->b0_quality<2;
		if(!navi_state)
		{
			bool bvalue=!low_quality;
			set_property_aliase_value(show_lane, &bvalue);
		}
		#if 1
		if (low_quality)//||pdetails_lane->b0_lane_type == en_lane_invalid)
		{
			lane_state=en_lane_off;
			init_lane_params();
			calcu_left_right_lanes();
			set_mp_text_uf(mtl_name, "text", lane_state_txt_name[navi_state][lane_state]);
			return;
		}
		else
		{
			lane_state=pre_lane_state;
		}
		set_mp_text_uf(mtl_name, "text", lane_state_txt_name[navi_state][lane_state]);
		#else
		set_mp_text_uf(mtl_name, "text", lane_state_txt_name[navi_state][lane_state]);
		if(low_quality)
		{
			if(pdetails_lane->b0_lane_type == en_lane_invalid)
			{
				init_lane_params();
				calcu_left_right_lanes();
				set_mp_text_uf(mtl_name, "text", lane_state_txt_name[navi_state][en_lane_off]);
			}
			return;
		}
		#endif
		left_lane_model_degree=pdetails_lane->b0_model_degree;
		s16* plpos=(s16*)(pbuff+1);
		left_position= *plpos/256.f;
		//printf("lane leftpos=%f\n",left_position);
		left_curvature_stm = pdetails_lane->b3_curvature_c2_byte0  + pdetails_lane->b4_curvature_c2_byte1* 0x100;
		left_curv_dt_stm= pdetails_lane->b5_derivative_c3_byte0  + pdetails_lane->b6_derivative_c3_byte1* 0x100;
		calcu_left_right_lanes();
	});

	g_msg_host.attach_monitor("lka left lane b", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF lka_left_lane_b
		{
			u8 b0_heading_angle_byte0;

			u8 b1_heading_angle_byte1;

			u8 b2_view_range_lsb;

			u8 b3_view_range_msb : 7;
			u8 b3_view_range_availability:1;

			u8 b4_reserve;
			u8 b5_reserve;
			u8 b6_reserve;
			u8 b7_reserve;			
		};
		lka_left_lane_b* pdetails_lane = (lka_left_lane_b*)pbuff;
		u16* pheading = (u16*)pbuff;
		left_lane_heading_stm = *pheading;
		calcu_left_right_lanes();
	});
	g_msg_host.attach_monitor("lka right lane a", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF lka_right_lane_a
		{
			u8 b0_lane_type : 4;
			u8 b0_quality : 2;
			u8 b0_model_degree : 2;

			u8 b1_position_c0_byte0;

			u8 b2_position_c0_byte1;

			u8 b3_curvature_c2_byte0;

			u8 b4_curvature_c2_byte1;

			u8 b5_derivative_c3_byte0;

			u8 b6_derivative_c3_byte1;

			u8 b7_width_left_marking;
		};
		lka_right_lane_a* pdetails_lane = (lka_right_lane_a*)pbuff;
		//printf("right lane curve quality=%d lane_type=%d\n",pdetails_lane->b0_quality,pdetails_lane->b0_lane_type);
		bool low_quality=pdetails_lane->b0_quality<2;
		if(!navi_state)
		{
			bool bvalue=!low_quality;
			set_property_aliase_value(show_lane, &bvalue);
		}
		#if 1
		if (low_quality)//||pdetails_lane->b0_lane_type == en_lane_invalid)
		{
			lane_state=en_lane_off;
			init_lane_params();
			calcu_left_right_lanes();
			set_mp_text_uf(mtl_name, "text", lane_state_txt_name[navi_state][lane_state]);
			return;
		}
		else
		{
			lane_state=pre_lane_state;
		}
		set_mp_text_uf(mtl_name, "text", lane_state_txt_name[navi_state][lane_state]);
		#else
		set_mp_text_uf(mtl_name, "text", lane_state_txt_name[navi_state][lane_state]);
		if(low_quality)
		{
			if(pdetails_lane->b0_lane_type == en_lane_invalid)
			{
				init_lane_params();
				calcu_left_right_lanes();
				set_mp_text_uf(mtl_name, "text", lane_state_txt_name[navi_state][en_lane_off]);
			}
			return;
		}
		#endif
		right_lane_model_degree=pdetails_lane->b0_model_degree;
		s16* plpos=(s16*)(pbuff+1);
		right_position= *plpos/256.f;
		//printf("lane rightpos=%f\n",right_position);
		right_curvature_stm = pdetails_lane->b3_curvature_c2_byte0  + pdetails_lane->b4_curvature_c2_byte1* 0x100;
		right_curv_dt_stm= pdetails_lane->b5_derivative_c3_byte0  + pdetails_lane->b6_derivative_c3_byte1* 0x100;

		calcu_left_right_lanes();
	});

	g_msg_host.attach_monitor("lka right lane b", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF lka_right_lane_b
		{
			u8 b0_heading_angle_byte0;

			u8 b1_heading_angle_byte1;

			u8 b2_view_range_lsb;

			u8 b3_view_range_msb : 7;
			u8 b3_view_range_availability : 1;

			u8 b4_reserve;
			u8 b5_reserve;
			u8 b6_reserve;
			u8 b7_reserve;
		};
		lka_right_lane_b* pdetails_lane = (lka_right_lane_b*)pbuff;
		u16* pheading = (u16*)pbuff;
		right_lane_heading_stm = *pheading;
		calcu_left_right_lanes();
	});

	g_msg_host.attach_monitor("details - lane", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF details_lane
		{
			u8 b0_lane_cuevature_lsb;
			u8 b1_lane_cuevature_msb : 7;
			u8 b1_lane_cuevature_msb_sg:1;

			u8 b2_lane_heading_lsb;

			u8 b3_lane_heading_msb : 4;
			u8 b3_ca : 1;
			u8 b3_right_ldw_availability : 1;
			u8 b3_left_ldw_availability : 1;
			u8 b3_na : 1;

			u8 b4_yaw_agle_lsb;
			u8 b5_yaw_agle_msb;

			u8 b6_pitch_angel_lsb;
			u8 b7_pitch_angel_msb;


		};
		details_lane* pdetails_lane = (details_lane*)pbuff;
	});
    g_msg_host.attach_monitor("details - obstacle status",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF details_obstacle_status
		{
			u8 b0_num_obstacles;
			
			u8 b1_timestamp;
			
			u8 b2_appliaction_vesion;
			
			u8 b3_avtive_version_number_section:2;
			u8 b3_left_close_range_cut_in:1;
			u8 b3_right_close_range_cut_in : 1;
			u8 b3_go:4;
			
			u8 b4_protocol_version;
			
			u8 b5_reserved:3;
			u8 b5_failsafe:4;
			u8 b5_close_car:1;			
		};
		details_obstacle_status* pdata=(details_obstacle_status*)pbuff;
		obstacles_cnt = pdata->b0_num_obstacles;
		if (obstacles_cnt>15)
		{
			obstacles_cnt = 15;
		}
		if(dg_obs)
		{
			printf("obstacles_cnt=%d\n",obstacles_cnt);
		}
		for (int ix = obstacles_cnt; ix < 15;ix++)
		{
			base_ui_component* pobj = obstacles_group->get_child(ix);
			pobj->set_visible(false);
			obs_list[ix]._recording=false;
			obs_list[ix]._prv_x_draw=0;
			obs_list[ix]._prv_x=0;
			obs_list[ix]._prv_y=0;
			obstacle_states[ix]._obs_st=en_ob_invisible;
			obs_list[ix]._pre_collision_time=max_pre_ct;
			//obs_list[ix]._prv_y=0.f;
		}
		if (obstacles_cnt==0)
		{
			bool be_show = false;
			for (int ix = 0; ix < 6; ix++)
			{
				char seg_name[50] = "";
				sprintf(seg_name, "show_distance%d", ix);
				set_property_aliase_value(seg_name, &be_show);
			}
			for (int ix = 0; ix < 15; ix++)
			{
				base_ui_component* pobj = obstacles_group->get_child(ix);
				pobj->set_visible(false);
				obstacle_states[ix]._obs_st=en_ob_invisible;
			}
		}
		notify_hide_obstacle();
	});
       g_msg_host.attach_monitor("details - obstacle data a",[&](u8* pbuff,int len){
		u16* pmessg = (u16*)pbuff;
		int obstacle_id = (*pmessg - 0x739) / 3;
		if(obstacle_id>14) return;
		pbuff+=2;
		struct GNU_DEF details_obstacle_data_a
		{
			u8 b0_obstacle_id;
			
			u16 b1_obstacle_pos_x:12;
			
			u16 b2_null:4;
			
			s16 b34_obstacle_pos_y : 10;
			u8 b4_blinker_info:3;
			u8 b4_cut_in_and_out : 3;
			
			s16 b5_obstacle_rel_vel_x:12;//rel vx
			u8 b6_obstacle_type : 3;
			u8 b6_reserved:1;
			
			u8 b7_obstacle_status:3;
			u8 b7_obstacle_brake_lights:1;
			u8 b7_reserved:2;
			u8 b7_obstacle_valid:2;
		
		};
		details_obstacle_data_a* pdata=(details_obstacle_data_a*)pbuff;
		ft_image* pimg_obj = (ft_image*)obstacles_group->get_child(obstacle_id);
		if (obstacle_id < obstacles_cnt)
		{
			pimg_obj->set_visible(true);
		}
		else
		{
			pimg_obj->set_visible(false);
			hide_obsacle(obstacle_id);
			return;
		}

		u16 posx = pdata->b1_obstacle_pos_x;
		float x_v_rel=pdata->b5_obstacle_rel_vel_x*0.0625f;
		//printf("obstacle_id=0x%x posx=0x%x\n", pdata->b0_obstacle_id, posx);
		if (posx==0xfff)
		{
			//printf("abandoned invalid xvalue\n");
			return;
		}
		if (posx>0xfa0)
		{
			posx = 0xfa0;
		}
		if (posx>0x200)
		{
			pimg_obj->set_texture_id(en_people_war0_png);
		}
		else
		if (posx>0x100)
		{
			pimg_obj->set_texture_id(en_people_war1_png);
		}
		else
		{
			pimg_obj->set_texture_id(en_people_war2_png);
		}

		if (pdata->b34_obstacle_pos_y >= 0x200)
		{
			//printf("abandoned invalid yvalue\n");
			return;
		}         
		float mz=posx*0.0625f;
		float mx= pdata->b34_obstacle_pos_y*0.0625;
		float my=0.5;///1.f;
		bool hided=false;
		if(dg_obs)
		{
			printf("id=%d valid=%d status=%d,mx=%f,mz=%f\n",obstacle_id,pdata->b7_obstacle_valid,pdata->b7_obstacle_status,mx,mz);
		}
#define cp_before_prj
		#ifdef cp_before_prj
		hided=/*mz>=15.f||*/mx>=3.f||mx<=-3.f;
		float pmx=mx;
		float pmz=mz;
		if(obs_list[obstacle_id]._recording&&IS_MOVING(pdata->b7_obstacle_status))
		{
			
			float x_offset=mx-obs_list[obstacle_id]._prv_x;
			float y_offset=mz-obs_list[obstacle_id]._prv_y;
			auto currentTime = steady_clock::now();
			int  tmills = chrono::duration_cast<chrono::duration<int, std::milli>>(currentTime - obs_list[obstacle_id]._tp).count();
			float tsecs=tmills*0.001f;
        		float ax=obstacle_accel_x[obstacle_id]*0.03f;
			const float at=ob_delay*ob_delay*0.5f;
			float deltav=x_v_rel*ob_delay+ax*at;
			
			if(mx>2&&mx<-2)
			{
				obs_list[obstacle_id]._pre_collision_time=max_pre_ct;
			}
			else 
			{
				float vrel=x_v_rel+ax*ob_delay;
				if(vrel>0.00001f)
				obs_list[obstacle_id]._pre_collision_time=mz/vrel;
			}
			
			//printf("mx=%f,_prv_x=%f,x_offset=%f,tsecs=%f\n",mx,obs_list[obstacle_id]._prv_x,x_offset,tsecs);
			float hv=x_offset/tsecs;
			float deltah=hv*ob_delay;

			mz=mz+deltav;
			
			float mx_draw=mx+deltah;
			float d_x_off=mx_draw-obs_list[obstacle_id]._prv_x_draw;
			float d_sign=d_x_off*x_offset;
			if(d_sign<=0)
			{
				mx=obs_list[obstacle_id]._prv_x_draw;
			}
			else
			{
				mx=mx_draw;
			}	
		}
		obs_list[obstacle_id]._recording=true;
		obs_list[obstacle_id]._prv_x_draw=mx;
		obs_list[obstacle_id]._tp=steady_clock::now();
		obs_list[obstacle_id]._prv_x=pmx;
		obs_list[obstacle_id]._prv_y=pmz;
		#endif
		//obs_list[obstacle_id]._prv_x_draw=mx;
           
		
		#if 0
		g_from_xie.obj_input.x=mz;
		g_from_xie.obj_input.y=mx;
		g_from_xie.obj_input.z= -g_from_xie.ADAS_to_ground;
		if(Coordinate_Change(&g_from_xie) == OKAY)
		{
			pimg_obj->set_base_pos(g_from_xie.hud_obj_pixel_x,	g_from_xie.hud_obj_pixel_y);
			printf("output x=%f,y=%f\n",g_from_xie.hud_obj_pixel_x,g_from_xie.hud_obj_pixel_y);
			return;
		}
		return;
		#endif
		/*printf("mx=%f,my=%f,mz=%f\n",mx,my,mz);
		ft_textblock* ptext=(ft_textblock*)pimg_obj->get_child(0);
		char str_buff[0x100]={0};
		sprintf(str_buff,"(%.2f,%.2f,%.2f)",mx,my,mz);
		ptext->set_content(str_buff);*/
		if(dg_obs)
		{
			printf("after cp mx=%f,mz=%f\n",mx,mz);
		}
		vector<POINT3D> in_points;
		POINT3D pt3d={mx,my,mz};
		in_points.push_back(pt3d);
		vector<POINT> proj_points;
		HUD_ProjectPoints3d(in_points, proj_points);
		/*if(proj_points.size()>0)
		{
			
			//printf("px=%f,py=%f\n",proj_points[0].x,proj_points[0].y);
		}*/
		
		//float vposx = -0.15*posx;
		///float vposx=(proj_points[0].y-630)/2;
		float vpos=proj_points[0].y;//*0.593333;
		float szu = 69 - posx * 0.05;
		//s16 uposy = pdata->b34_obstacle_pos_y; // (((u16)pdata->b4_obstacle_pos_y_msb )<<8 | pdata->b3_obstacle_pos_y_lsb)<<6;
	    
		//printf("uposy=0x%x\n", uposy);

		
		
		//printf("sposy=%d\n", sposy);
		//sposy /= 64;
              float sscale=400.f/675.f;
		float hpos =  proj_points[0].x;//*sscale;
		//hpos -= 110;
		//printf("obstacle_id=%d,hscale=%f,hpos=%f,vposx=%f\n", obstacle_id, hscale, hpos, vposx);
		
		#ifdef cp_after_prj
		if(obs_list[obstacle_id]._recording)
		{
			auto currentTime = steady_clock::now();
			int delta = chrono::duration_cast<chrono::duration<int, std::milli>>(currentTime - obs_list[obstacle_id]._tp).count();
			float fdelta=(float)delta/7.f;
			float x_offset=hpos-obs_list[obstacle_id]._prv_x;
			float y_offset=vpos-obs_list[obstacle_id]._prv_y;
			float x_ac=x_offset/fdelta;
			float y_ac=y_offset/fdelta;
			hpos+=x_ac;
			vpos+=y_ac;
			//printf("x_ac=%f,y_ac=%f\n",x_ac,y_ac);
		}
		obs_list[obstacle_id]._recording=true;
		obs_list[obstacle_id]._tp=steady_clock::now();
		obs_list[obstacle_id]._prv_x=hpos;
		obs_list[obstacle_id]._prv_y=vpos;
		#endif
		
		auto bevisible=handle_obstacle_state(obstacle_id,pdata->b6_obstacle_type,hpos,vpos,szu,hided);
		pimg_obj->set_visible(bevisible);
		pimg_obj->set_size(ImVec2(szu, szu));
		float xpos=hpos-szu/2;
		float ypos=vpos-szu/2;
		pimg_obj->set_base_pos(xpos, ypos);
	      
	});
       g_msg_host.attach_monitor("details - obstacle data b",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF details_obstacle_data_b
		{
			u8 b0_obstacle_length;
			
			u8 b1_obstacle_width;
			
			u8 b2_obstacle_age;
			
			u8 b3_obstacle_lane:2;
			u8 b3_cipv_flag:1;
			u8 b3_reserved : 1;
			u8 b3_rader_pos_x_lsb:4;
			
			u8 b4_rader_pos_x_msb;
			
			u8 b5_radar_vel_x_lsb;
			
			u8 b6_radar_vel_x_msb:4;
			u8 b6_radar_match_confidence : 3;
			u8 b6_reserved:1;
			
			u8 b7_matched_radar_id;
			u8 b7_reserved:1;
		
		};
		details_obstacle_data_b* pdetails_obstacle_data=(details_obstacle_data_b*)pbuff;
	});
       g_msg_host.attach_monitor("details - obstacle data c",[&](u8* pbuff,int len){
		struct GNU_DEF details_obstacle_data_c
		{
			u8 b0_obstacle_angle_rate_lsb;
			
			u8 b1_obstacle_angle_rate_msb;
						
			u8 b2_obstacle_scale_change_lsb;
			
			u8 b3_obstacle_scale_change_msb;
						
			s16 b4_object_accel_x:10;			
			u8 b5_null:2;
			u8 b5_obstacle_replaced : 1;
			u8 b5_reserved:3;
						
			u8 b6_obstacle_angle_lsb;
			
			u8 b7_obstacle_angle_msb;
		
		};
		u16* pmessg = (u16*)pbuff;
		int obstacle_id = (*pmessg - 0x73b) / 3;
		if(obstacle_id>14) return;
		pbuff+=2;	
		details_obstacle_data_c* pdetails_obstacle_data=(details_obstacle_data_c*)pbuff;
		obstacle_accel_x[obstacle_id]=pdetails_obstacle_data->b4_object_accel_x;
		
	});   
}
