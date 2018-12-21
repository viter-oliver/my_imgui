#pragma once
#include "afg.h"
class main_menu_controller
{
	base_ui_component* _root;
	ft_image* _menu_bg;
	ft_image* _arrow_btn_left;
	ft_image* _arrow_btn_right;
	ft_image* _img_selected;
	base_ui_component* _info_icon_list;
	base_ui_component* _navi_group;
	base_ui_component* _setup_group;
	base_ui_component* _media_group;
	base_ui_component* _trip_group;


	int _first_level_menu_index;
	int _menu_level;
public:
	main_menu_controller(base_ui_component* root);
	~main_menu_controller(){}
	void key_operation(u8 key_state, u8 key_type, u8 key_source);
};

