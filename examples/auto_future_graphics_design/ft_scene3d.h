#pragma once
#include "ft_base.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class ft_scene3d :
	public ft_base
{
	ImVec2 _size;
	int _texture_index;
	//frame buffer
	unsigned int _fboId;
	unsigned int _colorTextId;
	unsigned int _depthStencilTextId;
public:
	ft_scene3d();
	ft_scene3d(ft_scene3d& bsource);
	~ft_scene3d();
	base_ui_component* get_a_copy(){ return new ft_scene3d(*this); }
	//ft_button(int tid) :base_ui_component(tid){}
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
#endif
	bool init_from_json(Value& jvalue);
	bool handle_mouse();
};

REGISTER_CONTROL(ft_scene3d)