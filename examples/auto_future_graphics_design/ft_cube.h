#pragma once
#include "ft_base.h"
#include "basic3d.h"
class ft_cube :
	public ft_base
{
	tri_mesh _mesh;
	basic_shader* _pshader;
	//render transformation
	glm::vec3 _scale;
	glm::vec3 _rotation;
	glm::vec3 _translation;
public:
	ft_cube();
	ft_cube(ft_cube& bsource);
	~ft_cube();
	base_ui_component*get_a_copy(){ return new ft_cube(*this); }
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
#endif
	bool init_from_json(Value& jvalue);
	bool init_json_unit(Value& junit);

};
REGISTER_CONTROL(ft_cube)
