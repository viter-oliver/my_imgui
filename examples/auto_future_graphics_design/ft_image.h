#pragma once
#include "ft_base.h"
class ft_image :
	public ft_base
{
	ImVec3 _pos;
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
	void offset(ImVec2& imof);
	void translate_axis_pos(ImVec2& offset){ _axis_pos.x += offset.x; _axis_pos.y += offset.y; }
	void rotate(float angle){  _angle = angle; }
};
REGISTER_CONTROL(ft_image)
