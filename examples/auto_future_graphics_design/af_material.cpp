#include "af_material.h"
shared_ptr<shader_uf> get_sd_uf(shader_variable& unif)
{
	switch (unif._variable_type)
	{
	case GL_FLOAT:
		return make_shared<shader_uf_float>(unif._size, 1);
	case GL_FLOAT_VEC2:
		return  make_shared<shader_uf_float>(unif._size, 2);
	case GL_FLOAT_VEC3:
		return  make_shared<shader_uf_float>(unif._size, 3);
	case GL_FLOAT_VEC4:
	case GL_FLOAT_MAT2:
		return  make_shared<shader_uf_float>(unif._size, 4);
	case GL_FLOAT_MAT3:
		return  make_shared<shader_uf_float>(unif._size, 9);
	case GL_FLOAT_MAT4:
		return  make_shared<shader_uf_float>(unif._size, 16);
	case GL_FLOAT_MAT2x3:
	case GL_FLOAT_MAT3x2:
		return  make_shared<shader_uf_float>(unif._size, 6);
	case GL_FLOAT_MAT2x4:
	case GL_FLOAT_MAT4x2:
		return  make_shared<shader_uf_float>(unif._size, 8);
	case GL_FLOAT_MAT3x4:
	case GL_FLOAT_MAT4x3:
		return  make_shared<shader_uf_float>(unif._size, 12);
	case GL_DOUBLE:
		return  make_shared<shader_uf_double>(unif._size, 1);
	case GL_DOUBLE_VEC2:
		return  make_shared<shader_uf_double>(unif._size, 2);
	case GL_DOUBLE_VEC3:
		return  make_shared<shader_uf_double>(unif._size, 3);
	case GL_DOUBLE_VEC4:
	case GL_DOUBLE_MAT2:
		return  make_shared<shader_uf_double>(unif._size, 4);
	case GL_DOUBLE_MAT3:
		return  make_shared<shader_uf_double>(unif._size, 9);
	case GL_DOUBLE_MAT4:
		return  make_shared<shader_uf_double>(unif._size, 16);
	case GL_DOUBLE_MAT2x3:
	case GL_DOUBLE_MAT3x2:
		return  make_shared<shader_uf_double>(unif._size, 6);
		break;
	case GL_DOUBLE_MAT2x4:
	case GL_DOUBLE_MAT4x2:
		return  make_shared<shader_uf_double>(unif._size, 8);
	case GL_DOUBLE_MAT3x4:
	case GL_DOUBLE_MAT4x3:
		return  make_shared<shader_uf_double>(unif._size, 12);
	case GL_INT:
		return  make_shared<shader_uf_int>(unif._size, 1);
	case GL_INT_VEC2:
		return  make_shared<shader_uf_int>(unif._size, 2);
	case GL_INT_VEC3:
		return  make_shared<shader_uf_int>(unif._size, 3);
	case GL_INT_VEC4:
		return  make_shared<shader_uf_int>(unif._size, 4);
		break;
	case GL_UNSIGNED_INT:
		return  make_shared<shader_uf_uint>(unif._size, 1);
	case GL_UNSIGNED_INT_VEC2:
		return  make_shared<shader_uf_uint>(unif._size, 2);
	case GL_UNSIGNED_INT_VEC3:
		return  make_shared<shader_uf_uint>(unif._size, 3);
	case GL_UNSIGNED_INT_VEC4:
		return  make_shared<shader_uf_uint>(unif._size, 4);
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
		return  make_shared<shader_uf_txt>(unif._size, 1);
	}
	return nullptr;
}
material::material(shared_ptr<af_shader> psd)
	:_pshader(psd)
{
	mshader_variable_list& sd_uf_def = _pshader->get_uf_defs();
	for (auto& uf_def_ut : sd_uf_def)
	{
		auto& un_name = uf_def_ut.first;
		auto& unif = uf_def_ut.second;
		shared_ptr<shader_uf> pnunf = get_sd_uf(unif);
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
	if (!_pshader->is_valid())
	{
		return;
	}
	_pshader->use();
	auto& sd_uf_def = _pshader->get_uf_defs();
	shader_uf_txt::reset_sample_index();
	for (auto& shd_uf_value : _mp_shader_uf)
	{
		auto& kname = shd_uf_value.first;
		auto& ufdef = sd_uf_def[kname];
		auto& sd_uf = shd_uf_value.second;
		sd_uf->set_to_loaction(ufdef._location);	
	}
}

void material::set_value(string unf_name, float* pfvalue, GLuint len)
{
	auto& shd_ut = _mp_shader_uf.find(unf_name);
	if (shd_ut==_mp_shader_uf.end())
	{
		printf("fail to set uniform:%s\n", unf_name.c_str());
		return;
	}
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
	auto& shd_ut = _mp_shader_uf.find(unf_name);
	if (shd_ut == _mp_shader_uf.end())
	{
		printf("fail to set uniform:%s\n", unf_name.c_str());
		return;
	}
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
	auto& shd_ut = _mp_shader_uf.find(unf_name);
	if (shd_ut == _mp_shader_uf.end())
	{
		printf("fail to set uniform:%s\n", unf_name.c_str());
		return;
	}
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
void material::set_value(string unf_name, double* pdvalue, GLuint len)
{
	auto& shd_ut = _mp_shader_uf.find(unf_name);
	if (shd_ut == _mp_shader_uf.end())
	{
		printf("fail to set uniform:%s\n", unf_name.c_str());
		return;
	}
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

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "user_control_imgui.h"
void material::refresh_shader_uf()
{
	auto& sd_uf_def = _pshader->get_uf_defs();
	for (auto& iuf = _mp_shader_uf.begin(); iuf != _mp_shader_uf.end();)
	{
		auto& ikey = iuf->first;
		auto& isdu = sd_uf_def.find(ikey);
		if (isdu == sd_uf_def.end())
		{
			iuf = _mp_shader_uf.erase(iuf);
		}
		else
		{
			auto& sd_uf = *iuf->second;
			auto& sd_var = isdu->second;
			if (sd_uf.get_type() != sd_var._variable_type)
			{
				iuf = _mp_shader_uf.erase(iuf);
			}
			else
			{
				iuf++;
			}
		}
	}
	for (auto& isu_var : sd_uf_def)
	{
		auto& iuf = _mp_shader_uf.find(isu_var.first);
		if (iuf == _mp_shader_uf.end())
		{
			auto& un_name = isu_var.first;
			auto& unif = isu_var.second;
			shared_ptr<shader_uf> pnunf = get_sd_uf(unif);
			if (pnunf)
			{
				pnunf->_unf_name = un_name;
				pnunf->set_type(unif._variable_type);
				_mp_shader_uf[un_name] = pnunf;
			}
		}
	}
}
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
		pshd_fu._unf_name = shd_uf_value.first;
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
bool material::init_from_json(Value& jvalue)
{
	_name = jvalue["name"].asString();
	auto& shd_name = jvalue["shader"].asString();

	auto& shd_unit = g_af_shader_list.find(shd_name);
	if (shd_unit != g_af_shader_list.end())
	{
		_pshader = shd_unit->second;
	}
	else
	{
		return false;
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
	return true;
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
			pmtl->refresh_shader_uf();
		}

	}
}
#endif
bool set_mp_text_uf(const char* mtl_nm, const char* txt_uf_nm, const char* txt_nm)
{
	const auto& imtl = g_material_list.find(mtl_nm);
	if (imtl != g_material_list.end())
	{
		auto& mtl = *imtl->second;
		auto& mp_sduf = mtl.get_mp_sd_uf();
		auto& isduf = mp_sduf.find(txt_uf_nm);
		if (isduf != mp_sduf.end())
		{
			auto& txt_sduf = *isduf->second;
			auto data_hd = txt_sduf.get_data_head();
			memcpy(data_hd, txt_nm, strlen(txt_nm));
			txt_sduf.link();
			return true;
		}
	}
	return false;
}