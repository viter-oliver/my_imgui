#include "afg.h"
#include "af_bind.h"
#include "near_scenario.h"
#include "msg_host_n.h"
#include "af_timer.h"
#include "enum_txt_name0.h"
#include "af_state_manager.h"
#include <fstream>
#include <chrono>
using namespace auto_future_utilities;
using namespace auto_future;
extern void print_buff(u8* pbuff, int len);
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
extern u8 theme_id;
extern void thame_change();
bool be_turn_left = false;
bool be_turn_right = false;
//#define MAX_CONTENT_LEN 0x100
extern char str_show[MAX_CONTENT_LEN];


enum en_safty_belt
{
	en_not_cofigured,
	en_cmd_fault,
	en_not_bulked,
	en_bulked,
	en_invalid,
};
using namespace chrono;
//static const char* mtl_name="mt_sp";


static int cs_tm_sversion_id=0;
enum {
	en_auto,
	en_eco,
	en_comfort,
	en_sport,
	en_snow,
	en_off_road,
	en_individual,
	en_EV,
	en_sand,
	en_mud_ruts,
	en_extrme,
	en_model_cnt,
	};
       static char model_ch[15][en_model_cnt]=
       	{
       	{0x20,0},//自动
		{0xe7,0xbb,0x8f,0xe6,0xb5,0x8e,0},//eco 经济
		{0xe8,0x88,0x92,0xe9,0x80,0x82,0},//comfort 舒适
		{0xe8,0xbf,0x90,0xe5,0x8a,0xa8,0},//sports运动
		{0xe9,0x9b,0xaa,0xe5,0x9c,0xb0,0},//snow 雪地
       	{0xe8,0xbd,0xa6,0xe8,0xbe,0x99,0},// off road 车辙
       	{0xe8,0x87,0xaa,0xe5,0xae,0x9a,0xe4,0xb9,0x89,0},//individual 自定义
       	{0x20,0},//EV
       	{0x20,0},//sand
       	{0x20,0},//mud ruts
       	{0xe6,0x9e,0x81,0xe8,0x87,0xb4,0},//极致
	      };
       static char model_en[15][en_model_cnt]=
       	{
       	{0x20,0},//自动
		{"Eco"},//eco 经济
		{"Comfort"},//comfort 舒适
		{"Sports"},//sports运动
		{"Snow"},//snow 雪地
       	{"Off road"},// off road 车辙
       	{"Individual"},//individual 自定义
       	{0x20,0},//EV
       	{0x20,0},//sand
       	{0x20,0},//mud ruts
       	{"Extreme"},//极致
	      };
 typedef char( *pmodel_str )[ en_model_cnt ];
pmodel_str model_show[ 2 ] = { model_ch, model_en };
static u8 language_id=0,mod_id=0;
int tmid_phone=0;
bool g_be_phoning=false;

//fcta,fctb
int tmid_left_fctb=0;
int tmid_right_fctb=0;


void register_near_cmd_handl()
{

	g_msg_host.attach_monitor("left turn",[&](u8*pbuff,int len){
		be_turn_left=*pbuff!=0;
		/*if(be_turn_left)
		{
			play_tran("turn_left", 0,1);
		}*/
		
	});
	g_msg_host.attach_monitor("right turn",[&](u8*pbuff,int len){
		be_turn_right=*pbuff!=0;
		/*if(be_turn_right)
		{
			play_tran("turn_right", 0,1);
		}*/
		
	});
	g_msg_host.attach_monitor("speed",[&](u8*pbuff,int len){
		//printf("speed:");
		//print_buff(pbuff,len);
		struct GNU_DEF speed_block
		{
			u8 _data_valid;
			u8 _warning;
			u8 _speed_value_h;
			u8 _speed_value_l;
			
		};
		speed_block* pdata=(speed_block*)pbuff;
		if(pdata->_data_valid){
			u16 speed_value=pdata->_speed_value_h<<8|pdata->_speed_value_l;
			sprintf(str_show,"%d",speed_value);
			set_property_aliase_value("speed", str_show);
		}
		
	});
	g_msg_host.attach_monitor("gear",[&](u8*pbuff,int len){
		enum  {
			en_invalid,
			en_P,
			en_R,
			en_N,
			en_D,
			en_M,
			en_S,
			en_g_cnt
		};
		const char* gear_show[en_g_cnt]={"--","P","R","N","D","M","S"};
		u8 gvalue=*pbuff;
		if(gvalue<en_g_cnt){
			/*auto ilen=strlen(gear_show[gvalue]);
			memcpy(str_show,gear_show[gvalue],ilen);
			str_show[ilen]=0;*/
			strcpy(str_show,gear_show[gvalue]);
			set_property_aliase_value("gear", str_show);
		}
	});
	g_msg_host.attach_monitor("driving model",[&](u8*pbuff,int len){
		
		     u8 drm=*pbuff;
		     if(drm<en_model_cnt)
		     {
		     		mod_id=drm;
		       //auto ilen=strlen(model_sh[drm]);
				strcpy(str_show,model_show[language_id][mod_id]);
			   	set_property_aliase_value("driving_model", str_show);
		     }
	});
	tmid_phone=g_timer.register_timer_ex([&](int tid){
		if(tid==1||tid==3)
		{
			play_tran("phone_ring", 0,1);
		}
		else
		if(tid==4)
		{
			bool beshow=false;
			set_property_aliase_value("show_phone_ring", &beshow);
			g_timer.deactive_time_ex(tmid_phone);
		}
	 });
       g_msg_host.attach_monitor("phone call",[&](u8*pbuff,int len){
		if(*pbuff==1)
		{
			if(!g_be_phoning)
			{
				g_be_phoning=true;
				set_property_aliase_value("show_phone_ring", &g_be_phoning);
				play_tran("phone_ring", 0,1);
				g_timer.active_timer_ex(tmid_phone,1000);
			}
			
		}
		else
		{
			bool beshow=false;
			g_be_phoning=false;
			set_property_aliase_value("show_phone_ring", &beshow);
		}
		
	});	
	g_msg_host.attach_monitor("language",[&](u8*pbuff,int len){
		language_id=*pbuff!=0;
		strcpy(str_show,model_show[language_id][mod_id]);
	       set_property_aliase_value("driving_model", str_show);
		
	});	
	g_msg_host.attach_monitor("color model",[&](u8*pbuff,int len){
		static char* md_nm[3]=
		{
			"font1_clr",
			"font2_clr",
			"font3_clr",
		};
		u8 clr_md=*pbuff;
		if(clr_md<3&&theme_id!=clr_md)
		{
		       theme_id=clr_md;
			printf("theme_id=%d\n",theme_id);
			cover_common_value(md_nm[clr_md]);
			thame_change();
		}
	});
	tmid_left_fctb=g_timer.register_timer_ex([&](int tid){
		int ihide=tid%2;
	 	bool be_show=ihide;
		set_property_aliase_value("show_left_fctb",&be_show);
	 });
	tmid_right_fctb=g_timer.register_timer_ex([&](int tid){
		int ihide=tid%2;
	 	bool be_show=ihide;
		set_property_aliase_value("show_right_fctb",&be_show);
	 });
	g_msg_host.attach_monitor("FMradar_1",[&](u8*pbuff,int len){
		struct GNU_DEF struct_can_msg
		{
			u8 Checksum_FMradar_1;
			u16 FMradar_FCTA_status:2;
			u16 FMradar_FCTB_status:2;
			u16 FMradar_FCTA_Warning_Left:2;
			u16 FMradar_FCTB_BrakeReq:1;
			u16 Reserved:4;
			u16 FMradar_FCTA_Warning_Right:2;
			u16 Reserved2:3;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
		bool be_show=false;
		set_property_aliase_value("show_left_fctb",&be_show);
		set_property_aliase_value("show_right_fctb",&be_show);
		set_property_aliase_value("show_left_fcta1",&be_show);
		set_property_aliase_value("show_left_fcta2",&be_show);
		set_property_aliase_value("show_right_fcta1",&be_show);
		set_property_aliase_value("show_right_fcta2",&be_show);		
		be_show=true;
		if(can_msg->FMradar_FCTB_BrakeReq)
		{
			if(can_msg->FMradar_FCTA_Warning_Left==1||can_msg->FMradar_FCTA_Warning_Left==2)
			{
				set_property_aliase_value("show_left_fctb",&be_show);
				g_timer.active_timer_ex(tmid_left_fctb,200);
			}
			else
			{
				g_timer.deactive_time_ex(tmid_left_fctb);
			}
			if(can_msg->FMradar_FCTA_Warning_Right==1||can_msg->FMradar_FCTA_Warning_Right==2)
			{
				set_property_aliase_value("show_right_fctb",&be_show);
				g_timer.active_timer_ex(tmid_right_fctb,200);
			}
			else
			{
				g_timer.deactive_time_ex(tmid_right_fctb);
			}
		}
		else
		{	
			if(can_msg->FMradar_FCTA_Warning_Left==1)
			{
				set_property_aliase_value("show_left_fcta1",&be_show);
				play_tran("left_fcta1",0,1);
			}
			else
			if(can_msg->FMradar_FCTA_Warning_Left==2)
			{
				set_property_aliase_value("show_left_fcta2",&be_show);
				play_tran("left_fcta2",0,1);
			}
			if(can_msg->FMradar_FCTA_Warning_Right==1)
			{
				set_property_aliase_value("show_right_fcta1",&be_show);
				play_tran("right_fcta1",0,1);
			}
			else
			if(can_msg->FMradar_FCTA_Warning_Right==2)
			{
				set_property_aliase_value("show_right_fcta2",&be_show);
				play_tran("right_fcta2",0,1);
			}
		}
	});
	g_msg_host.attach_monitor("LCDA",[&](u8*pbuff,int len){
		enum
		{
		in_active,
		warning_leve1,
		warning_leve2,
		};
		struct GNU_DEF struct_can_msg
		{
			u8 LCDA_WarningCmd_LCDA_Left:2;
			u8 LCDA_WarningCmd_LCDA_Right:2;
			u8 Reserved:4;
		};
		struct_can_msg* can_msg = (struct_can_msg*)pbuff;
		if(can_msg->LCDA_WarningCmd_LCDA_Left==warning_leve2)
		{
			play_tran("turn_left", 0,1);
		}
		if(can_msg->LCDA_WarningCmd_LCDA_Right==warning_leve2)
		{
			play_tran("turn_right", 0,1);
		}
		
	});
	 
}
