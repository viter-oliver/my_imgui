#pragma once
#include "ft_base.h"
class ft_listbox :
	public ft_base
{
	bool _vertical;
	ImVec4 _origin_pos;
	int _width, _height;
	vector<base_ui_component*> _vlist;
public:
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
#endif
	bool init_from_json(Value& jvalue);
	bool handle_mouse();

};
REGISTER_CONTROL(ft_listbox)
