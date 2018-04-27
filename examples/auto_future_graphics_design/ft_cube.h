#pragma once
#include "ft_base.h"
#include "basic3d.h"
class ft_cube :
	public ft_base
{
	tri_mesh _mesh;
	//render transformation
	glm::vec3 _scale;
	glm::vec3 _rotation;
	glm::vec3 _translation;
	basic_shader* _pshader;

public:
	ft_cube();
	~ft_cube();
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
	bool init_json_unit(Value& junit);
#endif
};
REGISTER_CONTROL(ft_cube)
