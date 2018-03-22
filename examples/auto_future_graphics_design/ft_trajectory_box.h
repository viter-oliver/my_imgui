#pragma once
#include "ft_base.h"
class ft_trajectory_box :
	public ft_base
{
	ImVector<ImVec4>  _path;
	vector<ImVector<ImDrawVert>> _components;
public:
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
#endif
	bool init_from_json(Value& jvalue);
	bool handle_mouse();
};

REGISTER_CONTROL(ft_trajectory_box)
