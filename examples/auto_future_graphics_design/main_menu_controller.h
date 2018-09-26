#pragma once
#include "afg.h"
class main_menu_controller
{
	base_ui_component* _root;
	ft_image* _img_selected;
	ft_image* _second_level_title;
	ft_image* _menu_bg;
	base_ui_component* _first_level_menu;
	base_ui_component* _media_group;
	base_ui_component* _tpms_group;
	base_ui_component* _fuel_cons_group;
	base_ui_component* _mileage_group;
	base_ui_component* _info_group;
	base_ui_component* _drive_mode_group;
	base_ui_component* _navi_group;
public:
	main_menu_controller(base_ui_component* root);
	~main_menu_controller(){}
};

