#pragma once
#include "application.h"

class my_application :
	public auto_future::application
{
	base_ui_component* _xedge = {NULL};
	ft_slider* _water_temp = { NULL };//69-29
	ft_slider* _fuel_cons = { NULL };//1702-1742
	base_ui_component* _left_tunnel = { NULL };
	base_ui_component* _left_scale_group = { NULL };
	ft_slider_thumb* _left_pointer_sl = { NULL };
	ft_image_play* _left_pointer_play = { NULL };
	ft_image_play* _edge_left_play = { NULL };
	ft_image_play* _edge_left_up_play = { NULL };
	ft_slider_thumb* _edge_left_up_sl1 = { NULL };
	ft_slider_thumb* _edge_left_up_sl2 = { NULL };
	base_ui_component* _adas = { NULL };
	base_ui_component* _trip_group = { NULL };
	base_ui_component* _right_tunnel = { NULL };
	base_ui_component* _right_scale_group = { NULL };
	ft_slider_thumb* _right_pointer_sl = { NULL };
	ft_image_play* _right_pointer_play = { NULL };
	ft_image_play* _edge_right_play = { NULL };
	ft_image_play* _edge_right_up_play = { NULL };
	ft_slider_thumb* _edge_right_up_sl1 = { NULL };
	ft_slider_thumb* _edge_right_up_sl2 = { NULL };
	base_ui_component* _odo_group = { NULL };
	ft_image* _w120 = { NULL };
	ft_textblock* _time = { NULL };
	ft_image* _icon_turn = { NULL };
	ft_textblock* _distance = { NULL };
	//base_ui_component* _gear;
	ft_image* _right_decorate_line = { NULL };//17-77
	ft_image* _left_decorate_line = { NULL };//17-77
	ft_textblock* _gear_value = { NULL };
	ft_textblock* _state = { NULL };
	ft_textblock* _speed = { NULL };
	ft_textblock* _trip = { NULL };
	ft_textblock* _odo = { NULL };
	base_ui_component* _main_menu = { NULL };
	base_ui_component* _signal_light = { NULL };
	base_ui_component* _popup_dlg = { NULL };
	bool _being_logo_animation = { true };
public:
	my_application(int argc, char **argv);
	void init_ui_component(base_ui_component* pobj);
	void resLoaded();
	void onUpdate();
	virtual ~my_application();
};

