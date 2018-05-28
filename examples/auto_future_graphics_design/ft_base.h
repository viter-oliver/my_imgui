#pragma once
#include "factory.h"
class ft_base :
	public base_ui_component
{
public:
	ft_base() :base_ui_component(){}
	
	void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
	bool init_json_unit(Value& junit);
#endif
	bool handle_mouse();
	base_ui_component* get_copy_of_object();
};
void property_copy(vproperty_list& vdest, vproperty_list& vsource);
base_ui_component* get_copy_of_object(base_ui_component* byobject);
REGISTER_CONTROL(ft_base)
