#include "afg.h"
#include "af_bind.h"
#include "car_info.h"
#include "msg_host_n.h"
#include "af_timer.h"
#include "enum_txt_name0.h"
#include <fstream>
#include <chrono>
using namespace auto_future_utilities;
using namespace auto_future;

bool get_system_version(string& str_version)
{
const char* version_file="/version/build.pro";
const char* version_key="ro.build.fub.version=";
	ifstream fin;
	fin.open(version_file);
	if(fin.is_open())
	{
		#define buff_len 0x100
		char cbuff[buff_len];
		while(fin.getline(cbuff,buff_len))
		{
			string str_line=cbuff;
			//printf("strline:%s\n",cbuff);
			auto ios=str_line.find(version_key);
			if(ios!= string::npos)
			{
				str_version=str_line.substr(ios+strlen(version_key));
				fin.close();
				return true;
			}
		}
	}
	fin.close();
	return false;
}

extern msg_host_n g_msg_host;
extern af_timer g_timer;
extern bool be_hud_proj;
bool be_turn_left = false;
bool be_turn_right = false;
#define MAX_CONTENT_LEN 0x100
extern char str_show[MAX_CONTENT_LEN];
static bool bvalue = false;
u8 g_gear;
struct signal_light_control 
{
	bool _visible;
	bool _be_preempted;
	enum_txt_name0_h txt_id;
};
using scu = signal_light_control;
enum
{
	en_no_seatbelt,
	en_tire_abnormal,
	en_fdring,
	en_signal0_cnt
};
scu signal0[en_signal0_cnt] = {
	{false,false,en_vinfo_no_seatbelt_png},
	{ false, false, en_vinfo_abnormal_tire_pressure_png },
	{ false, false, en_vinfo_fatigue_driving_png },
};
enum
{
	en_low_beam,
	en_full_beam,
	en_signal1_cnt
};
scu signal1[en_signal1_cnt] = {
	{ false, false, en_vinfo_dipped_headlight_png },
	{ false, false, en_vinfo_fullbeam_headlight_png },
};

enum 
{
	en_right_turn,
	en_doors_open,
	en_signal2_cnt,
};
scu signal2[en_signal2_cnt] = {
	{ false, false, en_vinfo_right_turn_png },
	{ false, false, en_vinfo_doors_open_png },
};
enum 
{
	en_left_turn,
	en_low_fuel,
	en_hot_water,
	en_signal3_cnt,
};
scu signal3[en_signal3_cnt] = {
	{ false, false, en_vinfo_left_turn_png },
	{ false, false, en_vinfo_low_fuel_png },
	{ false, false, en_vinfo_hot_water_png },
};
enum
{
	en_aeb0,
	en_aeb1,
	en_signal4_cnt,
};
scu signal4[en_signal4_cnt] = {
	{ false, false, en_vinfo_AEB_fault_png },
	{ false, false, en_vinfo_AEB_fault1_png },
};
enum
{
	en_acc_off,
	en_acc_control,
	en_acc_fault,
	en_trafic_acc_jam_off,
	en_trafic_acc_jam_on,
	en_signal5_cnt,
};
scu signal5[en_signal5_cnt] = {
	{ false, false, en_vinfo_cruise_control_png },
	{ false, false, en_vinfo_cruise_control1_png },
	{ false, false, en_vinfo_acc_fault_png },
	{ false, false, en_vinfo_traffic_jam_acc1_png },
	{ false, false, en_vinfo_traffic_jam_acc_png },
};
void set_signal_status(scu* pgrp, int grp_cnt, int sid, bool bon, const char* ctl_name, const char* v_name)
{
       if (sid >= grp_cnt)	{		return;	}
	assert(sid < grp_cnt);
	if (bon)
	{
		for (int ix = sid - 1; ix >= 0; ix--)
		{
			if (pgrp[ix]._visible || pgrp[ix]._be_preempted)
			{
				pgrp[sid]._be_preempted = true;
				pgrp[sid]._visible = false;
				return;
			}
		}
		pgrp[sid]._be_preempted = false;
		pgrp[sid]._visible = true;
		set_property_aliase_value(ctl_name, &pgrp[sid].txt_id);
		set_property_aliase_value(v_name, &pgrp[sid]._visible);
	}
	else{
		pgrp[sid]._visible = false;
		if (pgrp[sid]._be_preempted)
		{
			pgrp[sid]._be_preempted = false;
		}
		else
		{
			for (int ix = sid + 1; ix < grp_cnt;ix++)
			{
				if (pgrp[ix]._be_preempted)
				{
					pgrp[ix]._be_preempted = false;
					pgrp[ix]._visible = true;
					set_property_aliase_value(ctl_name, &pgrp[ix].txt_id);
					set_property_aliase_value(v_name, &pgrp[ix]._visible);
					return;
				}
			}
			for (int ix = 0; ix < grp_cnt;ix++)
			{
				if (pgrp[ix]._visible)
				{
					return;
				}
			}
			set_property_aliase_value(ctl_name, &pgrp[sid].txt_id);
			set_property_aliase_value(v_name, &pgrp[sid]._visible);
		}
	}
}
#define LIGHT0_SET(id,vl) set_signal_status(signal0,en_signal0_cnt,id,vl,\
"control_nseatbelt_abtire_fdring","show_nseatbelt_abtire_fdring")
#define LIGHT1_SET(id,vl) set_signal_status(signal1,en_signal1_cnt,id,vl,"control_beam_low_high","show_beam_low_high")
#define LIGHT2_SET(id,vl) set_signal_status(signal2,en_signal2_cnt,id,vl,\
"select_right_turn_doors_open","show_right_turn_doors_open")
#define LIGHT3_SET(id,vl) set_signal_status(signal3,en_signal3_cnt,id,vl,\
"select_left_turn_low_fuel_hot_water","show_left_turn_low_fuel_hot_water")
#define LIGHT4_SET(id,vl) set_signal_status(signal4,en_signal4_cnt,id,vl,"aeb_control","aeb_show")
#define LIGHT5_SET(id,vl) set_signal_status(signal5,en_signal5_cnt,id,vl,"acc_control","acc_show")
extern bool prohibit_lane_swith(u8 lane_type);
extern u8 left_lane_type,right_lane_type;


enum en_safty_belt
{
	en_not_cofigured,
	en_cmd_fault,
	en_not_bulked,
	en_bulked,
	en_invalid,
};
using namespace chrono;
extern const char* mtl_name ;
//static const char* mtl_name="mt_sp";
static const char* unf_name="dy";
class lane_ani_mg
{
	ps_mtl mtl_lane;
	float lane_step={-0.0025f};
	float lane_delta={0.00f};
	float lane_delta_max={0.29f};////{0.175f};
	float speed_unit={10.f/36.f};//m/s
	float lane_circle_mileage={0.9f};//m
	float lane_circle={2.f};//s
	float lane_time_delta={0.f};
	float lane_speed={0.f};//m/s
	steady_clock::time_point  lastTime;
public:
	void init_time()
	{
		lastTime = steady_clock::now();
		lane_delta = lane_delta_max;
	}
	void set_car_speed(u16 csp)
	{
		if (csp<20)		
		{			
			lane_step = 0.005f;		
		}		
		else		
		if (csp>=20&& csp<90)
		{			
			lane_step = 0.01;		
		}		
		else		
		{			
			lane_step = 0.02;
		}
		lane_speed=csp*speed_unit;
		lane_circle=lane_circle_mileage/lane_speed;
		lane_time_delta=lane_circle/24;
		//printf("lane_time_delta=%f\n",lane_time_delta);
	}
	void set_mtl(ps_mtl pmtl)
	{
		mtl_lane=pmtl;
	}
	void set_lane_step(float sp)
	{
		lane_step=sp;
	}
	void loop()
	{
		auto currentTime = steady_clock::now();
		int delta = chrono::duration_cast<chrono::duration<int, std::milli>>(currentTime - lastTime).count();
		float delta_s=delta/1000.f;
		if(delta_s<lane_time_delta)
		{
			//printf("delta=%d delta_s=%f\n",delta,delta_s);
			return;
		}
		lastTime=currentTime;

		lane_delta+=lane_step;
		if(lane_delta<=0)
			lane_delta = lane_delta_max;
		if(lane_delta>lane_delta_max)
			lane_delta =0;
		//printf("lane_delta=%f\n",lane_delta);
		mtl_lane->set_value(unf_name,&lane_delta,1);
		
	}
};
static lane_ani_mg s_lane_mg;

void lane_moving_animation()
{
	s_lane_mg.loop();
	
	
}
//rotate speed
static int rs_timer_id=-1;
static float rs_cur = 0.f;
static float rs_tar = 0.f;
static float rs_unit = 0.f;
//car speed
static int cs_timer_id=-1;
static int cs_cur = 0;
int cs_tar = 0;
static int cs_unit = 0;

static int cs_tm_sversion_id=0;
void register_car_cmd_handl()
{
	const auto& imtl=g_material_list.find(mtl_name);
	assert(imtl!=g_material_list.end());
	s_lane_mg.set_mtl(imtl->second);
	s_lane_mg.init_time();
	if(be_hud_proj)
	{
		s_lane_mg.set_lane_step(0.0025f);
	}
	s_lane_mg.set_car_speed(80);
	cs_timer_id = g_timer.register_timer([&](int tick){
		cs_cur += cs_unit;

		s_lane_mg.set_car_speed(cs_cur);
		snprintf(str_show, MAX_CONTENT_LEN, "%d", cs_cur);
		set_property_aliase_value("value_car_speed", str_show);
		float fprg = (float)(cs_cur) / 240.f;
		set_property_aliase_value("car_speed_prog", &fprg);
		
		if (tick==0)
		{
			s_lane_mg.set_car_speed(cs_tar);
			snprintf(str_show, MAX_CONTENT_LEN, "%d", cs_tar);
			set_property_aliase_value("value_car_speed", str_show);
			float fprg = (float)(cs_tar) / 240.f;
			set_property_aliase_value("car_speed_prog", &fprg);
			g_timer.deactive_time(cs_timer_id);
		}
	});
	cs_tm_sversion_id=g_timer.register_timer([&](int tick){
		if(tick==0)
		{
			bool beshow=false;
			set_property_aliase_value("show_popdlg", &beshow);
			g_timer.deactive_time(cs_tm_sversion_id);
		}
		
	});
	g_msg_host.attach_monitor("car speed",[&](u8*pbuff,int len){
		/*struct _car_speed
		{
		u16 ig_swithc : 1;
		u16 char_speed:
		};*/
		#define MAX_SPPED 240.f
		u16* pcar_speed=(u16*)pbuff;
		cs_tar = *pcar_speed;
		if (cs_tar>MAX_SPPED)
		{
			cs_tar = MAX_SPPED;
		}
		int cs_delta = cs_tar - cs_cur;
		if (cs_delta<25&&cs_delta>-25)
		{
			cs_cur = cs_tar;
			s_lane_mg.set_car_speed(cs_cur);
			snprintf(str_show, MAX_CONTENT_LEN, "%d", cs_cur);
			set_property_aliase_value("value_car_speed", str_show);
			float fprg = (float)(cs_cur) / MAX_SPPED;
			set_property_aliase_value("car_speed_prog", &fprg);
			return;
		}
		cs_unit = cs_delta / 24;
		cs_cur += cs_unit;
		s_lane_mg.set_car_speed(cs_cur);
		snprintf(str_show, MAX_CONTENT_LEN, "%d", cs_cur);
		set_property_aliase_value("value_car_speed", str_show);
		float fprg = (float)(cs_cur) / MAX_SPPED;
		set_property_aliase_value("car_speed_prog", &fprg);
		g_timer.active_timer(cs_timer_id, 24, 1);
		/*pbuff+=2;
		u8 speed_unit=*pbuff;
		if (speed_unit>4)
		{
			speed_unit = 0;
		}
		static const char* speed_unit_name[4]={"L/100km","mpg(GB)","mpg(US)","km/L"};
		sprintf_s(str_show, MAX_CONTENT_LEN, "%s", speed_unit_name[speed_unit]);
		set_property_aliase_value("car_speed_unit", str_show);
		*/
	});
	g_msg_host.attach_monitor("gear",[&](u8*pbuff,int len){
		g_gear=*pbuff;
		static const char* gear_name[0x10] =\
		{"P", " ", " ", " ", "1", "D", "N", "R", " ","2","3","4","5","6"," ","!" };
		if (g_gear>0x0f)
		{
			g_gear = 0x0f;
		}
		if(g_gear==7)
		{
			bool be_show =false;
			for (int ix = 0; ix < 6; ix++)
			{
				char seg_name[50] = "";
				sprintf(seg_name, "show_distance%d", ix);
				set_property_aliase_value(seg_name, &be_show);
			}
		}
		snprintf(str_show, MAX_CONTENT_LEN, "%s", gear_name[g_gear]);
		set_property_aliase_value("value_gear", str_show);
	});
	g_msg_host.attach_monitor("driver side safety belt",[&](u8*pbuff,int len){
		
		u8 belt_status=*pbuff;
		bool lgt_on = belt_status ==en_not_bulked;
		LIGHT0_SET(en_no_seatbelt, lgt_on);
		pbuff++;
		u16* pcar_spped=(u16*)pbuff;
	});
	g_msg_host.attach_monitor("front passenger side seat belt",[&](u8*pbuff,int len){
		u8 belt_status=*pbuff;
		bool lgt_on = belt_status == en_not_bulked;
		LIGHT0_SET(en_no_seatbelt, lgt_on);
		pbuff++;
		u16* pcar_spped=(u16*)pbuff;
	});
	g_msg_host.attach_monitor("ignition",[&](u8*pbuff,int len){
		u8 on_status=*pbuff;
		pbuff++;
		u16* pcar_spped=(u16*)pbuff;
		pbuff+=2;
		u8 speed_unit=*pbuff;
	});
	g_msg_host.attach_monitor("engine coolant temp",[&](u8*pbuff,int len){
		struct st_ct
		{
			u8 value:7;
			u8 signal:1;
		};
		//printf("b0=0x%x b1=0x%x\n",pbuff[0],pbuff[1]);
		u8 low_byte=*pbuff;
		pbuff++;
		st_ct* psc=(st_ct*)pbuff;
		
		s16 engine_water_temp=psc->value*0x100+low_byte;
		if(psc->signal)
			engine_water_temp=-engine_water_temp;
		//printf("engine coolant temp :%d\n",engine_water_temp);
		//s16* engine_water_temp=(s16*)pbuff;
		pbuff++;
		u8 water_temp_status=*pbuff;
		if (water_temp_status==0)
		{
			bvalue = engine_water_temp > 120;
			//printf("bvalue=%d\n",bvalue);
			LIGHT3_SET(en_hot_water, bvalue);
		}

	});
	g_msg_host.attach_monitor("fuel level low",[&](u8*pbuff,int len){
		u8 fuel_level_low=*pbuff;
		LIGHT3_SET(en_low_fuel, fuel_level_low != 0);
	});
	g_msg_host.attach_monitor("tpms",[&](u8*pbuff,int len){
		u8 system=*pbuff++;
		u8 tpms_status=*pbuff;
		LIGHT0_SET(en_tire_abnormal, tpms_status != 0);
	});
	g_msg_host.attach_monitor("door ajar",[&](u8*pbuff,int len){
		u8 door_status=*pbuff;
		LIGHT2_SET(en_doors_open, door_status != 0);
	});
	g_msg_host.attach_monitor("dms",[&](u8*pbuff,int len){
		u8 driver_fatigue_level=*pbuff;
		LIGHT0_SET(en_fdring, driver_fatigue_level != 0);
	});
	g_msg_host.attach_monitor("turnin",[&](u8*pbuff,int len){
		u8 turnin_light=*pbuff;
		enum trn_light_state
		{
			en_turn_both_off,
			en_turn_left,
			en_turn_right,
			en_turn_both_on,
		};
		switch (turnin_light)
		{
			case en_turn_both_off:
			{
				LIGHT2_SET(en_right_turn, false);
				LIGHT3_SET(en_left_turn, false);
				//bvalue=false;
				//set_property_aliase_value("prohibit_left_lane_change", &bvalue);
				//set_property_aliase_value("prohibit_right_lane_change", &bvalue);	
				be_turn_left = false;
				be_turn_right = false;
			}
			break;
		case en_turn_left:
			{
				LIGHT2_SET(en_right_turn, false);
				LIGHT3_SET(en_left_turn, true);
				//bvalue=false;
				//set_property_aliase_value("prohibit_right_lane_change", &bvalue);	
				be_turn_left = true;
				/*if(prohibit_lane_swith(left_lane_type))
				{
					bvalue=true;
					set_property_aliase_value("prohibit_left_lane_change", &bvalue);
				}*/
			}
			break;
		case en_turn_right:
			{
				LIGHT3_SET(en_left_turn, false);
				LIGHT2_SET(en_right_turn, true);
				//bvalue=false;
				//set_property_aliase_value("prohibit_left_lane_change", &bvalue);	
				be_turn_right = true;
				/*if(prohibit_lane_swith(right_lane_type))
				{
					bvalue=true;
					set_property_aliase_value("prohibit_right_lane_change", &bvalue);
				}*/				
			}
			break;
		case en_turn_both_on:
			{
				LIGHT2_SET(en_right_turn, true);
				LIGHT3_SET(en_left_turn, true);
				//bvalue=true;
				be_turn_left = true;
				be_turn_right = true;
/*
				if(prohibit_lane_swith(left_lane_type))
				{
					set_property_aliase_value("prohibit_left_lane_change", &bvalue);
				}
				if(prohibit_lane_swith(right_lane_type))
				{
					set_property_aliase_value("prohibit_right_lane_change", &bvalue);
				}		
*/
			}
			break;
		default:
			break;
		}

	});	
	rs_timer_id=g_timer.register_timer([&](int tick){
		rs_cur += rs_unit;
		float frt_sp = rs_cur / 1000.f;
		snprintf(str_show, MAX_CONTENT_LEN, "%.1f", frt_sp);
		//printf("rotating speed:%d\n",*protate_speed);
		set_property_aliase_value("value_rotate_speed", str_show);
		float fpg = rs_cur / 8000;
		set_property_aliase_value("rotate_speed_prog", &fpg);
		if (tick==0)
		{
			float frt_sp = rs_tar / 1000.f;
			snprintf(str_show, MAX_CONTENT_LEN, "%.1f", frt_sp);
			//printf("rotating speed:%d\n",*protate_speed);
			set_property_aliase_value("value_rotate_speed", str_show);
			float fpg = rs_tar / 8000;
			set_property_aliase_value("rotate_speed_prog", &fpg);
			g_timer.deactive_time(rs_timer_id);
		}
	});
	g_msg_host.attach_monitor("rotate speed",[&](u8*pbuff,int len){
		u16* protate_speed=(u16*)pbuff;
		//*protate_speed += 100;
		rs_tar = (float)(*protate_speed);
		if (rs_tar>8000.0f)
		{
			rs_tar = 8000.f;
		}
		rs_unit = (rs_tar - rs_cur)/12.f;
		rs_cur += rs_unit;
		float frt_sp = rs_cur / 1000.f;
		snprintf(str_show, MAX_CONTENT_LEN, "%.1f", frt_sp);
		set_property_aliase_value("value_rotate_speed", str_show);
		float fpg = rs_cur / 8000;
		set_property_aliase_value("rotate_speed_prog", &fpg);
		g_timer.active_timer(rs_timer_id, 24, 2);
	});
	g_msg_host.attach_monitor("high beam",[&](u8*pbuff,int len){
		struct GNU_DEF st_beam
		{
			u8 full_beam : 1;
			u8 low_beam : 1;
			u8 reserved : 6;
		};
		st_beam* high_beam = (st_beam*)pbuff;
		LIGHT1_SET(en_low_beam, high_beam->low_beam);
		LIGHT1_SET(en_full_beam, high_beam->full_beam);
	});
	g_msg_host.attach_monitor("compass", [&](u8*pbuff, int len){
		u8 degree = *pbuff;
		const float a_degree = 0.0175f;
		float value_degree = degree * 2.f * a_degree;
		set_property_aliase_value("compass_angle", &value_degree);
	});


	
	g_msg_host.attach_monitor("height adjustment",[&](u8*pbuff,int len){
		u8 height_value=*pbuff++;
	});
	g_msg_host.attach_monitor("height value",[&](u8*pbuff,int len){
		s8 ht_i=*pbuff++;
		u8 ht_d=*pbuff;
		float ht_value_f;
		if(ht_i>=0)
			ht_value_f=ht_i+ht_d*0.01;
		else
			ht_value_f=ht_i-ht_d*0.01;
		//printf("ht_value_f=%f\n",ht_value_f);
			
	});
	g_msg_host.attach_monitor("brightness adjustment",[&](u8*pbuff,int len){
		u8 brightness_value=*pbuff++;
	});
	g_msg_host.attach_monitor("mpu version",[&](u8*pbuff,int len){
		string mpu_version;
		printf("requst mpu version\n");
		if(get_system_version(mpu_version))
		{
		      // printf("system:%s\n",mpu_version.c_str());
			snprintf(str_show, MAX_CONTENT_LEN, "Mpu system version:%s", mpu_version.c_str());
			set_property_aliase_value("warning_content", str_show);
			bool beshow=true;
			set_property_aliase_value("show_popdlg", &beshow);
			
			g_timer.active_timer(cs_tm_sversion_id,1480,1480);
		}
	});
	 
}
