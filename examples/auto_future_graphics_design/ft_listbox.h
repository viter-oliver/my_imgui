#pragma once
#include "ft_base.h"
class ft_listbox :
	public ft_base
{
	bool _vertical;
	int _width, _height;
public:
	ft_listbox();
	ft_listbox(ft_listbox& bsource);
	base_ui_component* get_a_copy();
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
#endif
	bool init_from_json(Value& jvalue);
	bool handle_mouse();

};
REGISTER_CONTROL(ft_listbox)
