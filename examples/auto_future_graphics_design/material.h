#pragma once
#include "af_shader.h"
#include "shader_uf.h"
typedef map<string, shared_ptr<shader_uf>> mp_shader_uf;
class material
{
	shared_ptr<af_shader> _pshader;
	mp_shader_uf _mp_shader_uf;
	string _name;
public:
	material(){}
	material(shared_ptr<af_shader> psd);
	void set_pshader(shared_ptr<af_shader> psd)
	{
		_pshader = psd;
	}
	shared_ptr<af_shader>& get_shader()
	{
		return _pshader;
	}
	void set_name(string nm)
	{
		_name = nm;
	}
	string get_name()
	{
		return _name;
	}
	mp_shader_uf& get_mp_sd_uf()
	{
		return _mp_shader_uf;
	}
	void use();
	void set_value(string unf_name, float* pfvalue, GLuint len);
	void set_value(string unf_name, int* pivalue, GLuint len);
	void set_value(string unf_name, unsigned int* puivalue, GLuint len);
	void set_value(string unf_name, double* pdvalue, GLuint len);

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void edit_ufs();
	bool _sel{ false };

	void output_2_json(Value& jvalue);
	void init_from_json(Value& jvalue);
#endif
};
typedef map<string,shared_ptr<material>> mmaterial;
extern mmaterial g_material_list;