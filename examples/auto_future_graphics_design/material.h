#pragma once
#include "af_shader.h"
#include "shader_uf.h"
typedef map<string, shared_ptr<shader_uf>> mp_shader_uf;
/**
* @brief is instance of the shader  \n
* description:
* - a material contain a variable list which is correspond to the shader variable type of the shader referenced by the material
*/
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
	/**
	* @brief  when a material is used, the shader referenced is used \n
	*/
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
/*!< material of g_material_list will be referenced by some ui components*/
extern mmaterial g_material_list;