#pragma once
#include "af_shader.h"
#include "shader_uf.h"

class material
{
	af_shader& _shader;
	typedef map<string, shared_ptr<shader_uf>> mp_shader_uf;
	mp_shader_uf _mp_shader_uf;
	string _name;
public:
	material(af_shader& sd);
	void set_name(string nm)
	{
		_name = nm;
	}
	string get_name()
	{
		return _name;
	}
	void use();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void edit_ufs();
	bool _sel{ false };
#endif
};
typedef vector<shared_ptr<material>> vmaterial;
extern vmaterial g_material_list;