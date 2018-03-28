#pragma once
#include "ft_base.h"
class ft_image :
	public ft_base
{
	ImVec2 _size;
	int _texture_index;
	float _angle;
	ImVec2 _axis_pos;
public:
	ft_image() : _texture_index(0), _angle(0.0){}
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
#endif
	bool init_from_json(Value& jvalue);
	bool init_json_unit(Value& junit);
	void offset(ImVec2& imof);
	void rotate(float angle){  _angle = angle; }
};
REGISTER_CONTROL(ft_image)
