#pragma once
#include "ft_base.h"
#include "basic3d.h"
class ft_cube :
	public ft_base
{
	//tri_mesh _mesh;
	//render transformation
	struct intl_pt
	{
		glm::vec3 _scale;
		glm::vec3 _rotation;
		glm::vec3 _translation;
		//int _shader_instance_index;
		intl_pt() {}
	};
	intl_pt _pt;
	basic_shader* _pshader;
	static GLuint _vao, _vbo,_ebo;
	static unsigned short _cube_instance_cnt;
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
