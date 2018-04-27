#pragma once
#include "ft_base.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class ft_scene3d :
	public ft_base
{
	ImVec2 _size;
	//frame buffer
	unsigned int _fboId;
	unsigned int _colorTextId;
	unsigned int _depthStencilTextId;
public:
	ft_scene3d();
	~ft_scene3d();
	void draw();
	void collect_property_range(vproperty_list& vplist)
	{
		ft_base::collect_property_range(vplist);
		vplist.push_back(property_range(&_size, sizeof(ImVec2)));
	}
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
#endif
	bool handle_mouse();
};

REGISTER_CONTROL(ft_scene3d)