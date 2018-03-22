#pragma once
#include "ft_base.h"
class ft_slide :
	public ft_base
{
	ImVec4 _thumb[4];
	ImVec4 _background[4];
	int _texture_index_thumb;
	int _texture_index_background;
public:
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
#endif
	bool init_from_json(Value& jvalue);
	bool handle_mouse();
};

REGISTER_CONTROL(ft_slide)