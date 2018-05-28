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
		ImVec3 _scale;
		ImVec3 _rotation;
		ImVec2 _translation;
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
	int collect_property_range(vproperty_list& vplist)
	{
		int plen = ft_base::collect_property_range(vplist);
		int len = sizeof(intl_pt);
		vplist.emplace_back(&_pt, len);
		len += plen;
		return len;
	}
	void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
	bool init_json_unit(Value& junit);
#endif
};
REGISTER_CONTROL(ft_cube)
