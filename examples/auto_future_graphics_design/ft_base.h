#pragma once
#include "factory.h"
class ft_base :
	public base_ui_component
{
public:
	ft_base() :base_ui_component(){}
	ft_base(ft_base& tar);
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
#endif
	bool init_from_json(Value& jvalue);
	bool init_json_unit(Value& junit);
	bool handle_mouse();
};
REGISTER_CONTROL(ft_base)
