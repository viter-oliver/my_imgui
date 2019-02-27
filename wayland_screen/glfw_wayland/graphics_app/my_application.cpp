#include "my_application.h"
#include "main.h"
#include "msg_host_n.h"
#include <chrono>
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)
#pragma message(VAR_NAME_VALUE(__cplusplus))
#define _STR(x) #x
#define STR(x) _STR(x)
#define BASE_UC_REF(x) _##x=pmain_show->get_child(idx);_##x->set_visible(false);idx++
#define IMG_REF(x) _##x=static_cast<ft_image*> (pmain_show->get_child(idx));_##x->set_visible(false);idx++
#define SLD_REF(x) _##x=static_cast<ft_slider*> (pmain_show->get_child(idx));_##x->set_visible(false);idx++
#define SLD_THUMB_REF(x)  _##x=static_cast<ft_slider_thumb*> (pmain_show->get_child(idx));_##x->set_visible(false);idx++
#define IMG_PLAY_REF(x)  _##x=static_cast<ft_image_play*> (pmain_show->get_child(idx));_##x->set_visible(false);idx++
#define TXT_REF(x)  _##x=static_cast<ft_textblock*> (pmain_show->get_child(idx));_##x->set_visible(false);idx++
//#define BASE_UC_REF(x) if (!_##x&&object_name==STR(x)) {_##x=pobj;_##x->set_visible(false); return;}
//#define SLD_REF(x) if (!_##x&&object_name==STR(x)) {_##x=static_cast<ft_slider*> (pobj);_##x->set_visible(false);return;}
//#define SLD_THUMB_REF(x) if (!_##x&&object_name==STR(x)) {_##x=static_cast<ft_slider_thumb*> (pobj);_##x->set_visible(false);return;}
//#define IMG_PLAY_REF(x) if (!_##x&&object_name==STR(x)) {_##x=static_cast<ft_image_play*> (pobj);_##x->set_visible(false);return;}
//#define IMG_REF(x) if (!_##x&&object_name==STR(x)) {_##x=static_cast<ft_image*> (pobj);_##x->set_visible(false);return;}
//#define TXT_REF(x) if (!_##x&&object_name==STR(x)) {_##x=static_cast<ft_textblock*> (pobj);_##x->set_visible(false);return;}
int key_time_pointer[7] = 
{
	5, 
	7, 
	23,//15,20
	54,
	72
};
int time_line = 0;
int delta_limit = 30;


const ImVec2 popup_img_sel_pos[]=
{
	{82,175},
	{ 210, 175 },
};
using namespace auto_future_utilities;
msg_host_n g_msg_host;
//const char* protocol_path = "communication_protocol_n.json";
const char* filter_by_priority_path = "filter_by_priority_protocol.json";
//#define RCF_COMM
#ifdef RCF_COMM
#include "idata_collect.h"
idata_collect g_rcf_server;
#endif
#ifdef _SCOMMM
#include "scomm.h"
scomm s_scm;
#endif
my_application::my_application(int argc, char **argv)
	:application(argc,argv)
{
}


void my_application::init_ui_component()
{
	base_ui_component* pmain_show = _proot->get_child(0);
	int idx = 2;
	//printf("get ui componet:%s\n", object_name.c_str());
	BASE_UC_REF(xedge);
	SLD_REF(water_temp);
	SLD_REF(fuel_cons);
	BASE_UC_REF(left_tunnel);
	BASE_UC_REF(left_scale_group);
	SLD_THUMB_REF(left_pointer_sl);
	IMG_PLAY_REF(left_pointer_play);
	IMG_PLAY_REF(edge_left_play);
	IMG_PLAY_REF(edge_left_up_play);
	SLD_THUMB_REF(edge_left_up_sl1);
	SLD_THUMB_REF(edge_left_up_sl2);
	BASE_UC_REF(adas);
	BASE_UC_REF(trip_group);
	BASE_UC_REF(right_tunnel);
	BASE_UC_REF(right_scale_group);
	SLD_THUMB_REF(right_pointer_sl);
	IMG_PLAY_REF(right_pointer_play);
	IMG_PLAY_REF(edge_right_play);
	IMG_PLAY_REF(edge_right_up_play);
	SLD_THUMB_REF(edge_right_up_sl1);
	SLD_THUMB_REF(edge_right_up_sl2);
	BASE_UC_REF(odo_group);
	IMG_REF(w120);
	TXT_REF(time);
	IMG_REF(icon_turn);
	_distance = static_cast<ft_textblock*>(_icon_turn->get_child(1));
	_distance->set_visible(false);
	base_ui_component* _gear;
	BASE_UC_REF(gear);
	_gear->set_visible(true);
	_right_decorate_line = static_cast<ft_image*>(_gear->get_child(0));
	_right_decorate_line->set_visible(false);
	_left_decorate_line = static_cast<ft_image*>(_gear->get_child(1));
	_left_decorate_line->set_visible(false);
	_gear_value = static_cast<ft_textblock*>(_gear->get_child(2));
	_gear_value->set_visible(false);
	_state=static_cast<ft_textblock*>(_gear->get_child(3));
	_state->set_visible(false);
	TXT_REF(speed);
	TXT_REF(trip);
	TXT_REF(odo);
	_main_menu = _proot->get_child(1);
	_main_menu->set_visible(false);
	_signal_light = _proot->get_child(2);
	//_signal_light->set_visible(false);
	_popup_dlg = _proot->get_child(3);
	_popup_dlg->set_visible(false);
	//printf("is %s missed?\n", object_name.c_str());
}
void my_application::register_command_handle()
{
	//total mileage
	g_msg_host.attach_monitor("total mileage", [this](unsigned char* pbuff, int len){
		unsigned char DataValid = *pbuff++;
		int mileage_value = *((int*)pbuff);
		char odo_show[50] = { 0 };
		if (DataValid==1)
		{
			sprintf(odo_show, "ODO:%dkm", mileage_value);
		}
		else
		{
			strcpy(odo_show, "ODO:--");
		}
		_odo->set_content(odo_show);
		
	});
	g_msg_host.attach_monitor("subtotal mileage", [this](unsigned char* pbuff, int len){
		unsigned char DataValid = *pbuff++;
		*(pbuff+3) = 0;
		int sub_mileage_value = *((int*)pbuff);
		char trip_show[50] = { 0 };
		if (DataValid == 1)
		{
			sprintf(trip_show, "TRIP:%dkm", sub_mileage_value);
		}
		else
		{
			strcpy(trip_show,"TRIP:--");
		}
		_trip->set_content(trip_show);
	});
	g_msg_host.attach_monitor("auto speed", [this](unsigned char* pbuff, int len){
		unsigned char DataValid = *pbuff++;     // Byte first
		unsigned char alarm = *pbuff++;         // Byte Second
		short auto_speed = *((short*)pbuff);
		char auto_speed_show[50] = { 0 };
		if (DataValid == 1)
		{
			sprintf(auto_speed_show, "%d", auto_speed);
			float moto_speed_rate = (float)auto_speed / 3.f;
			_right_pointer_sl->set_progress_value(moto_speed_rate);
			if (_being_logo_animation)
			{
				_right_pointer_sl->calcu_thumb_pos();
				ImVec2 bsize = _right_pointer_sl->thumb_size();
				_right_pointer_play->base_pos() = _right_pointer_sl->thumb_base_pos();
				_right_pointer_play->set_size(bsize);
			}
		}
		else
		{
			strcpy(auto_speed_show, "--");
		}
		_speed->set_content(auto_speed_show);
	});

	g_msg_host.attach_monitor("motor speed", [this](unsigned char* pbuff, int len){
		unsigned char DataValid = *pbuff++;     // Byte first
		unsigned char alarm = *pbuff++;         // Byte Second
		short motor_speed = *((short*)pbuff);
		if (DataValid == 1)
		{
			/*char motor_speed_show[50] = { 0 };
			sprintf(motor_speed_show, "ROTATE:%r/m", motor_speed);
			_trip->set_content(motor_speed_show);*/
			float moto_speed_rate = (float)motor_speed  / 10.f;
			_left_pointer_sl->set_progress_value(moto_speed_rate);
			if (_being_logo_animation)
			{
				_left_pointer_sl->calcu_thumb_pos();
				ImVec2 bsize = _left_pointer_sl->thumb_size();
				_left_pointer_play->base_pos() = _left_pointer_sl->thumb_base_pos();
				_left_pointer_play->set_size(bsize);
			}
		}
	});

	// Fuel
	g_msg_host.attach_monitor("fuel", [this](unsigned char* pbuff, int len){
		unsigned char DataValid = *pbuff++;     // Byte first
		unsigned char alarm = *pbuff++;         // Byte Second
		short fuel_cons_value = *((short*)pbuff);   // Byte third
		//printf("****0x%x,0x%x,0x%x\n", *pbuff, *(pbuff + 1), fuel_cons_value);
		fuel_cons_value *= 0.1;
		float fuel_cons_rate = fuel_cons_value / 50.f;
		if (DataValid == 1)
		{
			_fuel_cons->set_progress(fuel_cons_rate);
		}
	});
	// Fuel
	g_msg_host.attach_monitor("water temperature", [this](unsigned char* pbuff, int len){
		unsigned char DataValid = *pbuff++;     // Byte first
		unsigned char alarm = *pbuff++;         // Byte Second
		short water_temperature_value = *((short*)pbuff);   // Byte third
		water_temperature_value -= 1000;
		water_temperature_value *= 0.1;
		float water_temperature_rate = water_temperature_value / 100;
		if (DataValid == 1)
		{
			_water_temp->set_progress(water_temperature_rate);
		}
	});
	// Gear
	g_msg_host.attach_monitor("gearbox", [this](unsigned char* pbuff, int len){
		unsigned char DataValid = *pbuff++;  // Byte first
		unsigned char GearPosition = *pbuff++;   // 变速档档位
		unsigned char TargerGear = *pbuff++; // 目标档位
		unsigned char GearBoxMode = *pbuff++; //变速箱模式：0标准模式，1经济模式，2动力模式，3雪地模式，4手动模式
		unsigned char ECO_Grade = *pbuff;    // ECO等级
		static char* gear_show[] = { "P", "R", "N", "D", "M", "S" };
		const unsigned char gear_position_cnt = sizeof(gear_show) / sizeof(char*);
		assert(GearPosition < gear_position_cnt);
		_gear_value->set_content(gear_show[GearPosition]);

	});
}
//double lastTime = 0.f;
std::chrono::high_resolution_clock::time_point lastTime;
void my_application::resLoaded()
{
	init_ui_component();
	_left_pointer_sl->set_progress_value(40);
	_left_pointer_sl->calcu_thumb_pos();
	ImVec2 bsize = _left_pointer_sl->thumb_size();
	_left_pointer_play->base_pos() = _left_pointer_sl->thumb_base_pos();
	_left_pointer_play->set_size(bsize);
	_edge_left_play->set_frame_index(0);

	_right_pointer_sl->set_progress_value(8);
	_right_pointer_sl->calcu_thumb_pos();
	bsize = _right_pointer_sl->thumb_size();
	_right_pointer_play->base_pos() = _right_pointer_sl->thumb_base_pos();
	_right_pointer_play->set_size(bsize);
	_edge_right_play->set_frame_index(0);
	_edge_right_up_sl1->set_progress_value(0.f);
	_edge_right_up_sl2->set_progress_value(0.f);
	_water_temp->set_progress(0.4f);
	_water_temp->base_pos().x = 69.f;//69-29
	_fuel_cons->set_progress(0.7f);
	_fuel_cons->base_pos().x = 1702.f;//1702-1742
	_right_decorate_line->set_size(ImVec2(17, 3));//17-77
	_left_decorate_line->set_size(ImVec2(17, 3));//17-77
	//lastTime = glfwGetTime();
	lastTime = std::chrono::high_resolution_clock::now();
	g_msg_host.load_protocol_from_config(_arg_list[0].c_str());
	register_command_handle();
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
void my_application::finish_animation()
{
	_trip_group->set_visible(true);
	_odo_group->set_visible(true);
	_speed->set_visible(true);
	_gear_value->set_visible(true);
	_trip->set_visible(true);
	_odo->set_visible(true);
	_time->set_visible(true);
	_pmain_menu = make_shared<main_menu_controller>(_main_menu);
	_psignal_light = make_shared<signal_light_controller>(_signal_light);
	_ppop_up_dlg = make_shared<pop_up_dlg_controller>(_popup_dlg);
}
bool play_image(ft_image_play* pimgp)
{
	auto last_fm_id=pimgp->get_frames_count()-1;
	auto cur_fm_id = pimgp->get_cur_frame_index();
	if (cur_fm_id<last_fm_id)
	{
		pimgp->set_frame_index(cur_fm_id + 1);
		return true;
	}
	else
	{
		return false;
	}
}
bool play_slider_thumb(ft_slider_thumb* psl, float delta=10.f)
{
	auto cur_pr_value = psl->get_progress_value();
	if (cur_pr_value>=100.f)
	{
		return false;
	}
	psl->set_progress_value(delta+cur_pr_value);
	return true;
}
void play_group(base_ui_component* pobj)
{
	for (size_t ix = 1; ix < pobj->get_child_count(); ix++)
	{
		ft_slider_thumb* psl = static_cast<ft_slider_thumb*>(pobj->get_child(ix));
		play_slider_thumb(psl);
	}
}
void my_application::onUpdate()
{
	g_msg_host.execute_data_handle_cmd();
	
	auto currentTime = std::chrono::high_resolution_clock::now();
	int delta = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(currentTime - lastTime).count();
	if (delta<delta_limit)
	{
		//printf("too fast....%d\n",delta);
		return;
	}
	lastTime = currentTime;
	if (!_being_logo_animation)
	{
		return;
	}
	time_line++;
	if (time_line == key_time_pointer[0])
	{
		_xedge->set_visible(true);
		return;
	}
	if (time_line>=key_time_pointer[1]&&time_line<key_time_pointer[2])
	{
		if (time_line == key_time_pointer[1])
		{
			_edge_left_play->set_visible(true);
			_edge_right_play->set_visible(true);
		}
		bool b_end = play_image(_edge_left_play);
		b_end = play_image(_edge_right_play);
		//printf("play_edge...\n");
		return;
	}
	if (time_line >= key_time_pointer[2] && time_line<key_time_pointer[3])
	{
		if (time_line == key_time_pointer[2])
		{
			_left_pointer_play->set_visible(true);
			_right_pointer_play->set_visible(true);
			_edge_left_up_play->set_visible(true);
			_edge_right_up_play->set_visible(true);

			//delta_limit = 45;
		}
		bool b_end = play_image(_left_pointer_play);
		b_end = play_image(_right_pointer_play);
		b_end = play_image(_edge_left_up_play);
		b_end = play_image(_edge_right_up_play);

		if (time_line==key_time_pointer[2]+5)
		{
			_water_temp->set_visible(true);
			_fuel_cons->set_visible(true);
			_right_decorate_line->set_visible(true);
			_left_decorate_line->set_visible(true);
		}
		if (time_line> key_time_pointer[2]+5)
		{
			if (_water_temp->base_pos().x>29.f)
			{
				_water_temp->base_pos().x -= 5.f;
			}
			if (_fuel_cons->base_pos().x<1742.f)
			{
				_fuel_cons->base_pos().x += 5.f;
			}
			else
			{
				_state->set_visible(true);
			}
			ImVec2 bsz = _right_decorate_line->get_size();
			if (bsz.x<77.f)
			{
				bsz.x += 7.5;
				_right_decorate_line->set_size(bsz);
			}
			bsz = _left_decorate_line->get_size();
			if (bsz.x < 77.f)
			{
				bsz.x += 7.5;
				_left_decorate_line->set_size(bsz);
			}
		}
		if (time_line==key_time_pointer[2]+13)
		{
			_left_scale_group->set_visible(true);
			_right_scale_group->set_visible(true);
		}
		if (time_line==key_time_pointer[2]+18)
		{
			_edge_left_up_sl1->set_visible(true);
			_edge_left_up_sl2->set_visible(true);
			_edge_right_up_sl1->set_visible(true);
			_edge_right_up_sl2->set_visible(true);
			_left_tunnel->set_visible(true);
			_right_tunnel->set_visible(true);
		}
		if (time_line > key_time_pointer[2] + 18)
		{
			play_slider_thumb(_edge_left_up_sl1);
			play_slider_thumb(_edge_left_up_sl2);
			play_slider_thumb(_edge_right_up_sl1);
			play_slider_thumb(_edge_right_up_sl2);
			play_group(_left_scale_group);
			play_group(_right_scale_group);
			
		}
		return;
	}
	if (time_line >= key_time_pointer[3] && time_line<key_time_pointer[4])
	{
		if (time_line == key_time_pointer[3])
		{
			_left_pointer_play->set_visible(false);
			_right_pointer_play->set_visible(false);
			_left_pointer_sl->set_visible(true);
			_right_pointer_sl->set_visible(true);
		}
		play_slider_thumb(_edge_left_up_sl1);
		play_slider_thumb(_edge_left_up_sl2);
		play_slider_thumb(_edge_right_up_sl1);
		play_slider_thumb(_edge_right_up_sl2);
		play_group(_left_scale_group);
		play_group(_right_scale_group);
		return;
	}
	if (time_line == key_time_pointer[4])
	{
		finish_animation();
		_being_logo_animation = false;
	}

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


