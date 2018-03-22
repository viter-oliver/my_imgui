#pragma once
#include "ft_base.h"
class ft_button :
	public ft_base
{
	ImVec4 _pos[4];
	int _texture_indices[2];
	string _label;
public:
	//ft_button(int tid) :base_ui_component(tid){}
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
#endif
	bool init_from_json(Value& jvalue);
	bool handle_mouse();
};
REGISTER_CONTROL(ft_button)
