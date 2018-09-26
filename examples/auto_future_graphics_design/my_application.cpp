#include "my_application.h"
#include "main.h"
#include "msg_host_n.h"
#include "idata_collect.h"
#include <chrono>
//#include <GLFW/glfw3.h>
#define _STR(x) #x
#define STR(x) _STR(x)
#define BASE_UC_REF(x) if (!_##x&&object_name==STR(x)) {_##x=pobj;_##x->set_visible(false); return;}
#define SLD_REF(x) if (!_##x&&object_name==STR(x)) {_##x=static_cast<ft_slider*> (pobj);_##x->set_visible(false);return;}
#define SLD_THUMB_REF(x) if (!_##x&&object_name==STR(x)) {_##x=static_cast<ft_slider_thumb*> (pobj);_##x->set_visible(false);return;}
#define IMG_PLAY_REF(x) if (!_##x&&object_name==STR(x)) {_##x=static_cast<ft_image_play*> (pobj);_##x->set_visible(false);return;}
#define IMG_REF(x) if (!_##x&&object_name==STR(x)) {_##x=static_cast<ft_image*> (pobj);_##x->set_visible(false);return;}
#define TXT_REF(x) if (!_##x&&object_name==STR(x)) {_##x=static_cast<ft_textblock*> (pobj);_##x->set_visible(false);return;}
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
const char* protocol_path = "communication_protocol_n.json";
const char* filter_by_priority_path = "filter_by_priority_protocol.json";
idata_collect g_rcf_server;
my_application::my_application(int argc, char **argv)
	:application(argc,argv)
{
}


void my_application::init_ui_component(base_ui_component* pobj)
{
	string object_name = pobj->get_name();
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
	TXT_REF(distance);
	//BASE_UC_REF(gear);
	IMG_REF(right_decorate_line);
	IMG_REF(left_decorate_line);
	TXT_REF(gear_value);
	TXT_REF(state);
	TXT_REF(speed);
	TXT_REF(trip);
	TXT_REF(odo);
	BASE_UC_REF(main_menu);
	BASE_UC_REF(signal_light);
	BASE_UC_REF(popup_dlg);
	//printf("is %s missed?\n", object_name.c_str());
}
//double lastTime = 0.f;
std::chrono::high_resolution_clock::time_point lastTime;
void my_application::resLoaded()
{
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
	g_msg_host.load_protocol_from_config(protocol_path);
	
	g_rcf_server.set_cmd_handle(std::bind(&msg_host_n::pick_valid_data, &g_msg_host, std::placeholders::_1, std::placeholders::_2));
	g_rcf_server.link();
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
	if (!_being_logo_animation)
	{
		return;
	}
	auto currentTime = std::chrono::high_resolution_clock::now();
	int delta = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(currentTime - lastTime).count();;
	if (delta<delta_limit)
	{
		//printf("too fast....%d\n",delta);
		return;
	}
	lastTime = currentTime;
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
		printf("play_edge...\n");
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
		_trip_group->set_visible(true);
		_odo_group->set_visible(true);
		_speed->set_visible(true);
		_gear_value->set_visible(true);
		_trip->set_visible(true);
		_odo->set_visible(true);
		_time->set_visible(true);
		_being_logo_animation = false;
	}

}

my_application::~my_application()
{
}
AFGUI_APP(my_application)