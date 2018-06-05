#include "material.h"

material::material(af_shader& sd)
	:_shader(sd)
{
	mshader_variable_list& sd_uf_def = _shader.get_uf_defs();
	for (auto& uf_def_ut : sd_uf_def)
	{
		auto& un_name = uf_def_ut.first;
		auto& unif = uf_def_ut.second;
		switch (unif._variable_type)
		{
		case GL_FLOAT:
			_mp_shader_uf[un_name] = make_shared<shader_uf_float>(unif._size);
			break;
		case GL_FLOAT_VEC2:
			_mp_shader_uf[un_name] = make_shared<shader_uf_float>(unif._size*2);
			break;
		case GL_FLOAT_VEC3:
			_mp_shader_uf[un_name] = make_shared<shader_uf_float>(unif._size*3);
			break;
		case GL_FLOAT_VEC4:
		case GL_FLOAT_MAT2:
			_mp_shader_uf[un_name] = make_shared<shader_uf_float>(unif._size*4);
			break;
		case GL_FLOAT_MAT3:
			_mp_shader_uf[un_name] = make_shared<shader_uf_float>(unif._size*9);
			break;
		case GL_FLOAT_MAT4:
			_mp_shader_uf[un_name] = make_shared<shader_uf_float>(unif._size*16);
			break;
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT3x2:
			_mp_shader_uf[un_name] = make_shared<shader_uf_float>(unif._size*6);
			break;
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT4x2:
			_mp_shader_uf[un_name] = make_shared<shader_uf_float>(unif._size*8);
			break;
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4x3:
			_mp_shader_uf[un_name] = make_shared<shader_uf_float>(unif._size*12);
			break;
		case GL_DOUBLE:
			_mp_shader_uf[un_name] = make_shared<shader_uf_double>(unif._size);
			break;
		case GL_DOUBLE_VEC2:
			_mp_shader_uf[un_name] = make_shared<shader_uf_double>(unif._size * 2);
			break;
		case GL_DOUBLE_VEC3:
			_mp_shader_uf[un_name] = make_shared<shader_uf_double>(unif._size * 3);
			break;
		case GL_DOUBLE_VEC4:
		case GL_DOUBLE_MAT2:
			_mp_shader_uf[un_name] = make_shared<shader_uf_double>(unif._size * 4);
			break;
		case GL_DOUBLE_MAT3:
			_mp_shader_uf[un_name] = make_shared<shader_uf_double>(unif._size * 9);
			break;
		case GL_DOUBLE_MAT4:
			_mp_shader_uf[un_name] = make_shared<shader_uf_double>(unif._size * 16);
			break;
		case GL_DOUBLE_MAT2x3:
		case GL_DOUBLE_MAT3x2:
			_mp_shader_uf[un_name] = make_shared<shader_uf_double>(unif._size * 6);
			break;
		case GL_DOUBLE_MAT2x4:
		case GL_DOUBLE_MAT4x2:
			_mp_shader_uf[un_name] = make_shared<shader_uf_double>(unif._size * 8);
			break;
		case GL_DOUBLE_MAT3x4:
		case GL_DOUBLE_MAT4x3:
			_mp_shader_uf[un_name] = make_shared<shader_uf_double>(unif._size * 12);
			break;
		case GL_INT:
			_mp_shader_uf[un_name] = make_shared<shader_uf_int>(unif._size);
			break;
		case GL_INT_VEC2:
			_mp_shader_uf[un_name] = make_shared<shader_uf_int>(unif._size * 2);
			break;
		case GL_INT_VEC3:
			_mp_shader_uf[un_name] = make_shared<shader_uf_int>(unif._size * 3);
			break;
		case GL_INT_VEC4:
			_mp_shader_uf[un_name] = make_shared<shader_uf_int>(unif._size * 4);
			break;
		case GL_UNSIGNED_INT:
			_mp_shader_uf[un_name] = make_shared<shader_uf_uint>(unif._size);
			break;
		case GL_UNSIGNED_INT_VEC2:
			_mp_shader_uf[un_name] = make_shared<shader_uf_uint>(unif._size * 2);
			break;
		case GL_UNSIGNED_INT_VEC3:
			_mp_shader_uf[un_name] = make_shared<shader_uf_uint>(unif._size * 3);
			break;
		case GL_UNSIGNED_INT_VEC4:
			_mp_shader_uf[un_name] = make_shared<shader_uf_uint>(unif._size * 4);
			break;
		}
	}
}
void material::use()
{
	_shader.use();
	auto& sd_uf_def = _shader.get_uf_defs();
	for (auto& shd_uf_value : _mp_shader_uf)
	{
		auto& kname = shd_uf_value.first;
		auto& ufdef = sd_uf_def[kname];
		auto& sd_uf = shd_uf_value.second;
		sd_uf->set_to_loaction(ufdef._location);
		/*switch (shd_uf_value.second.value_type())
		{
		}*/
		//_shader.uniform(shd_uf_value.first, &shd_uf_value.second[);
		
	}
}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "user_control_imgui.h"
void material::edit_ufs()
{
	for (auto& shd_uf_value : _mp_shader_uf)
	{
		string unf_name = shd_uf_value.first;
		unf_name += ":";
		ImGui::Text(unf_name.c_str());
		auto& pshd_fu = *shd_uf_value.second;
		pshd_fu.edit();
		//for (int ix = 0;ix)
	}
}
#endif
vmaterial g_material_list;