#include "material.h"

material::material(shared_ptr<af_shader> psd)
	:_pshader(psd)
{
	mshader_variable_list& sd_uf_def = _pshader->get_uf_defs();
	for (auto& uf_def_ut : sd_uf_def)
	{
		auto& un_name = uf_def_ut.first;
		auto& unif = uf_def_ut.second;
		shared_ptr<shader_uf> pnunf = nullptr;
		switch (unif._variable_type)
		{
		case GL_FLOAT:
			pnunf = make_shared<shader_uf_float>(unif._size,1);
			break;
		case GL_FLOAT_VEC2:
			pnunf = make_shared<shader_uf_float>(unif._size , 2);
			break;
		case GL_FLOAT_VEC3:
			pnunf = make_shared<shader_uf_float>(unif._size , 3);
			break;
		case GL_FLOAT_VEC4:
		case GL_FLOAT_MAT2:
			pnunf = make_shared<shader_uf_float>(unif._size , 4);
			break;
		case GL_FLOAT_MAT3:
			pnunf = make_shared<shader_uf_float>(unif._size , 9);
			break;
		case GL_FLOAT_MAT4:
			pnunf = make_shared<shader_uf_float>(unif._size , 16);
			break;
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT3x2:
			pnunf = make_shared<shader_uf_float>(unif._size , 6);
			break;
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT4x2:
			pnunf = make_shared<shader_uf_float>(unif._size , 8);
			break;
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4x3:
			pnunf = make_shared<shader_uf_float>(unif._size , 12);
			break;
#if !defined(GLFW_INCLUDE_ES3)
		case GL_DOUBLE:
			pnunf = make_shared<shader_uf_double>(unif._size,1);
			break;
		case GL_DOUBLE_VEC2:
			pnunf = make_shared<shader_uf_double>(unif._size , 2);
			break;
		case GL_DOUBLE_VEC3:
			pnunf = make_shared<shader_uf_double>(unif._size , 3);
			break;
		case GL_DOUBLE_VEC4:
		case GL_DOUBLE_MAT2:
			pnunf = make_shared<shader_uf_double>(unif._size , 4);
			break;
		case GL_DOUBLE_MAT3:
			pnunf = make_shared<shader_uf_double>(unif._size , 9);
			break;
		case GL_DOUBLE_MAT4:
			pnunf = make_shared<shader_uf_double>(unif._size , 16);
			break;
		case GL_DOUBLE_MAT2x3:
		case GL_DOUBLE_MAT3x2:
			pnunf = make_shared<shader_uf_double>(unif._size , 6);
			break;
		case GL_DOUBLE_MAT2x4:
		case GL_DOUBLE_MAT4x2:
			pnunf = make_shared<shader_uf_double>(unif._size , 8);
			break;
		case GL_DOUBLE_MAT3x4:
		case GL_DOUBLE_MAT4x3:
			pnunf = make_shared<shader_uf_double>(unif._size , 12);
			break;
#endif
		case GL_INT:
			pnunf = make_shared<shader_uf_int>(unif._size,1);
			break;
		case GL_INT_VEC2:
			pnunf = make_shared<shader_uf_int>(unif._size , 2);
			break;
		case GL_INT_VEC3:
			pnunf = make_shared<shader_uf_int>(unif._size , 3);
			break;
		case GL_INT_VEC4:
			pnunf = make_shared<shader_uf_int>(unif._size , 4);
			break;
		case GL_UNSIGNED_INT:
			pnunf = make_shared<shader_uf_uint>(unif._size,1);
			break;
		case GL_UNSIGNED_INT_VEC2:
			pnunf = make_shared<shader_uf_uint>(unif._size , 2);
			break;
		case GL_UNSIGNED_INT_VEC3:
			pnunf = make_shared<shader_uf_uint>(unif._size , 3);
			break;
		case GL_UNSIGNED_INT_VEC4:
			pnunf = make_shared<shader_uf_uint>(unif._size , 4);
			break;
		}
		if (pnunf)
		{
			pnunf->_unf_name = un_name;
			pnunf->set_type(unif._variable_type);
			_mp_shader_uf[un_name] = pnunf;

		}
	}
}
void material::use()
{
	_pshader->use();
	auto& sd_uf_def = _pshader->get_uf_defs();
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
void material::set_value(string unf_name, float* pfvalue, GLuint len)
{
	auto shd_ut = _mp_shader_uf.find(unf_name);
	auto& pshd = shd_ut->second;
	string cname = typeid(*pshd).name();
	cname = cname.substr(sizeof("class"));
	if (cname != "shader_uf_float")
	{
		printf("fail to match type:type of %s is not shader_uf_float but %s\n", unf_name.c_str(), cname.c_str());
		return;
	}
	shared_ptr<shader_uf_float> pshdf = static_pointer_cast<shader_uf_float>(pshd);
	pshdf->set_fvalue(pfvalue, len);
}
void material::set_value(string unf_name, int* pivalue, GLuint len)
{
	auto shd_ut = _mp_shader_uf.find(unf_name);
	auto& pshd = shd_ut->second;
	string cname = typeid(*pshd).name();
	cname = cname.substr(sizeof("class"));
	if (cname != "shader_uf_int")
	{
		printf("fail to match type:type of %s is not shader_uf_int but %s\n", unf_name.c_str(), cname.c_str());
		return;
	}
	shared_ptr<shader_uf_int> pshdf = static_pointer_cast<shader_uf_int>(pshd);
	pshdf->set_ivalue(pivalue, len);
}
void material::set_value(string unf_name, unsigned int* puivalue, GLuint len)
{
	auto shd_ut = _mp_shader_uf.find(unf_name);
	auto& pshd = shd_ut->second;
	string cname = typeid(*pshd).name();
	cname = cname.substr(sizeof("class"));
	if (cname != "shader_uf_uint")
	{
		printf("fail to match type:type of %s is not shader_uf_uint but %s\n", unf_name.c_str(), cname.c_str());
		return;
	}
	shared_ptr<shader_uf_uint> pshdf = static_pointer_cast<shader_uf_uint>(pshd);
	pshdf->set_uivalue(puivalue, len);

}
#if !defined(GLFW_INCLUDE_ES3)
void material::set_value(string unf_name, double* pdvalue, GLuint len)
{
	auto shd_ut = _mp_shader_uf.find(unf_name);
	auto& pshd = shd_ut->second;
	string cname = typeid(*pshd).name();
	cname = cname.substr(sizeof("class"));
	if (cname != "shader_uf_double")
	{
		printf("fail to match type:type of %s is not shader_uf_double but %s\n", unf_name.c_str(), cname.c_str());
		return;
	}
	shared_ptr<shader_uf_double> pshdf = static_pointer_cast<shader_uf_double>(pshd);
	pshdf->set_dvalue(pdvalue, len);
}
#endif
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "user_control_imgui.h"
void material::edit_ufs()
{
	string shd_name = "shader name:";
	shd_name += _pshader->get_name();
	ImGui::Text(shd_name.c_str());
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
void material::output_2_json(Value& jvalue)
{
	jvalue["name"] = _name;
	jvalue["shader"] = _pshader->get_name();
	Value junifs(objectValue);
	for (auto& unif_ut : _mp_shader_uf)
	{
		Value junif_ut(objectValue);
		unif_ut.second->output_2_json(junif_ut);
		junifs[unif_ut.first] = junif_ut;
	}
	jvalue["uniform_list"] = junifs;
}
void material::init_from_json(Value& jvalue)
{
	_name = jvalue["name"].asString();
	auto& shd_name = jvalue["shader"].asString();

	const auto& shd_unit = g_af_shader_list.find(shd_name);
	if (shd_unit != g_af_shader_list.end())
	{
		_pshader = shd_unit->second;
	}

	Value& uniform_list = jvalue["uniform_list"];
	Value::Members memb(uniform_list.getMemberNames());
	for (auto itmem = memb.begin(); itmem != memb.end(); ++itmem)
	{
		auto& mname = *itmem;
		Value& junif = uniform_list[mname];

		string str_type = junif["type"].asString();
		GLuint usize = junif["usize"].asUInt();
		GLuint el_size = junif["el_size"].asUInt();
		_mp_shader_uf[mname] = std::move(fac_shader_uf::get().Create(str_type, usize, el_size));
		shared_ptr<shader_uf> pnunf = _mp_shader_uf[mname];
		pnunf->init_from_json(junif);
	}
}
void material::refresh()
{
	mshader_variable_list& sd_uf_def = _pshader->get_uf_defs();
	for (auto& uf_def_ut : sd_uf_def)
	{
		auto& un_name = uf_def_ut.first;
		auto& unif = uf_def_ut.second;
		auto ifd = _mp_shader_uf.find(un_name);
		if (ifd == _mp_shader_uf.end() || ifd->second->get_type() != unif._variable_type && (_mp_shader_uf.erase(ifd), true))
		{

			shared_ptr<shader_uf> pnunf = nullptr;
			switch (unif._variable_type)
			{
			case GL_FLOAT:
				pnunf = make_shared<shader_uf_float>(unif._size, 1);
				break;
			case GL_FLOAT_VEC2:
				pnunf = make_shared<shader_uf_float>(unif._size, 2);
				break;
			case GL_FLOAT_VEC3:
				pnunf = make_shared<shader_uf_float>(unif._size, 3);
				break;
			case GL_FLOAT_VEC4:
			case GL_FLOAT_MAT2:
				pnunf = make_shared<shader_uf_float>(unif._size, 4);
				break;
			case GL_FLOAT_MAT3:
				pnunf = make_shared<shader_uf_float>(unif._size, 9);
				break;
			case GL_FLOAT_MAT4:
				pnunf = make_shared<shader_uf_float>(unif._size, 16);
				break;
			case GL_FLOAT_MAT2x3:
			case GL_FLOAT_MAT3x2:
				pnunf = make_shared<shader_uf_float>(unif._size, 6);
				break;
			case GL_FLOAT_MAT2x4:
			case GL_FLOAT_MAT4x2:
				pnunf = make_shared<shader_uf_float>(unif._size, 8);
				break;
			case GL_FLOAT_MAT3x4:
			case GL_FLOAT_MAT4x3:
				pnunf = make_shared<shader_uf_float>(unif._size, 12);
				break;
#if !defined(GLFW_INCLUDE_ES3)
			case GL_DOUBLE:
				pnunf = make_shared<shader_uf_double>(unif._size, 1);
				break;
			case GL_DOUBLE_VEC2:
				pnunf = make_shared<shader_uf_double>(unif._size, 2);
				break;
			case GL_DOUBLE_VEC3:
				pnunf = make_shared<shader_uf_double>(unif._size, 3);
				break;
			case GL_DOUBLE_VEC4:
			case GL_DOUBLE_MAT2:
				pnunf = make_shared<shader_uf_double>(unif._size, 4);
				break;
			case GL_DOUBLE_MAT3:
				pnunf = make_shared<shader_uf_double>(unif._size, 9);
				break;
			case GL_DOUBLE_MAT4:
				pnunf = make_shared<shader_uf_double>(unif._size, 16);
				break;
			case GL_DOUBLE_MAT2x3:
			case GL_DOUBLE_MAT3x2:
				pnunf = make_shared<shader_uf_double>(unif._size, 6);
				break;
#endif
			case GL_DOUBLE_MAT2x4:
			case GL_DOUBLE_MAT4x2:
				pnunf = make_shared<shader_uf_double>(unif._size, 8);
				break;
			case GL_DOUBLE_MAT3x4:
			case GL_DOUBLE_MAT4x3:
				pnunf = make_shared<shader_uf_double>(unif._size, 12);
				break;
			case GL_INT:
				pnunf = make_shared<shader_uf_int>(unif._size, 1);
				break;
			case GL_INT_VEC2:
				pnunf = make_shared<shader_uf_int>(unif._size, 2);
				break;
			case GL_INT_VEC3:
				pnunf = make_shared<shader_uf_int>(unif._size, 3);
				break;
			case GL_INT_VEC4:
				pnunf = make_shared<shader_uf_int>(unif._size, 4);
				break;
			case GL_UNSIGNED_INT:
				pnunf = make_shared<shader_uf_uint>(unif._size, 1);
				break;
			case GL_UNSIGNED_INT_VEC2:
				pnunf = make_shared<shader_uf_uint>(unif._size, 2);
				break;
			case GL_UNSIGNED_INT_VEC3:
				pnunf = make_shared<shader_uf_uint>(unif._size, 3);
				break;
			case GL_UNSIGNED_INT_VEC4:
				pnunf = make_shared<shader_uf_uint>(unif._size, 4);
				break;
			}
			if (pnunf)
			{
				pnunf->_unf_name = un_name;
				pnunf->set_type(unif._variable_type);
				_mp_shader_uf[un_name] = pnunf;
			}
		}
	}
	for (auto idd = _mp_shader_uf.begin(); idd != _mp_shader_uf.end();)
	{
		auto kname = idd->first;
		auto iff = sd_uf_def.find(kname);
		if (iff != sd_uf_def.end())
		{
			idd = _mp_shader_uf.erase(idd);
		}
		else
		{
			idd++;
		}
	}
}
#endif
mmaterial g_material_list;
bool create_material(string& shader_name, string& material_name, string& real_material_name)
{
	auto shd = g_af_shader_list.find(shader_name);
	if (shd==g_af_shader_list.end())
	{
		printf("fail to find shader:%s\n", shader_name.c_str());
		return false;
	}
	auto mtlu = g_material_list.find(material_name);

	if (mtlu!=g_material_list.end())
	{
		real_material_name = material_name + "#";
	}
	else
	{
		real_material_name = material_name;
	}

	g_material_list[real_material_name]=make_shared<material>(shd->second);
	g_material_list[real_material_name]->set_name(real_material_name);
	return true;
}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void refresh_material(shared_ptr<af_shader> pshd)
{
	for (auto& mtl:g_material_list)
	{
		auto pmtl = mtl.second;
		if (pmtl->get_shader()==pshd)
		{
			pmtl->refresh();
		}

	}
}
#endif
