#include "ui_assembler.h"
#include <fstream>
#include "texture_res_load.h"
#include "af_shader.h"
#include "material.h"
bool ui_assembler::load_ui_component_from_file(const char* file_path)
{
	ifstream fin;
	fin.open(file_path);
	if (fin.is_open())
	{
		Reader reader;
		Value jroot;
		if (reader.parse(fin, jroot, false))
		{
			Value screenw = jroot["screenw"];
			Value screenh = jroot["screenh"];
			if (!screenw.isNull())
			{
				base_ui_component::screenw = screenw.asDouble();
			}
			else
			{
			    base_ui_component::screenw = 2000.0f;
			}
			if (!screenh.isNull())
			{
				base_ui_component::screenh = screenh.asDouble();
			}
			else
			{
				base_ui_component::screenh = 1000.0f;
			}

			texture_res_load tresload(g_vres_texture_list);
			tresload.load_res_from_json(jroot);
			Value& shader_list = jroot["shader_list"];
			auto isize = shader_list.size();
			UInt ix = 0;
			for (ix = 0; ix < isize; ix++)
			{
				Value& shd_unit = shader_list[ix];
				auto& vs_code = shd_unit["vs_code"].asString();
				auto& fs_code = shd_unit["fs_code"].asString();
				shared_ptr<af_shader> pshd = make_shared<af_shader>(vs_code.c_str(), fs_code.c_str());
				pshd->_vs_name = shd_unit["vs_name"].asString();
				pshd->_fs_name = shd_unit["fs_name"].asString();
				pshd->set_name(shd_unit["name"].asString());
				g_af_shader_list[shd_unit["name"].asString()] = pshd;
			}
			Value& material_list = jroot["material_list"];
			isize = material_list.size();
			for (ix = 0; ix < isize; ix++)
			{
				Value& mtl_unit = material_list[ix];
				shared_ptr<material> pmtl = make_shared<material>();
				pmtl->init_from_json(mtl_unit);
				g_material_list[pmtl->get_name()]=pmtl;

			}
			_root.init_from_json(jroot);
		}
		fin.close();
	}
	else
	{
		printf("invalid file_path:%s\n", file_path);
		return false;
	}
	return true;
}

bool ui_assembler::output_ui_component_to_file(const char* file_path)
{

	ofstream fout;
	fout.open(file_path);
	Value jroot(objectValue);
	jroot["screenw"] = base_ui_component::screenw;
	jroot["screenh"] = base_ui_component::screenh;
	Value jtexture(arrayValue);
	for (auto& reslist:g_vres_texture_list)
	{
		Value jtext_res_unit(objectValue);
		jtext_res_unit["texture_pack_file"] = reslist.texture_pack_file;
		jtext_res_unit["texture_data_file"] = reslist.texture_data_file;
		jtexture.append(jtext_res_unit);
	}
	jroot["texture_res_list"] = jtexture;
	Value jshader(arrayValue);
	for (auto& shd_ut : g_af_shader_list)
	{
		Value jshd_unit(objectValue);
		auto& sd_name = shd_ut.first;
		auto& p_sd = shd_ut.second;
		jshd_unit["name"] = sd_name;
		jshd_unit["vs_name"] = p_sd->_vs_name;
		jshd_unit["fs_name"] = p_sd->_fs_name;
		jshd_unit["vs_code"] = p_sd->get_vs_code();
		jshd_unit["fs_code"] = p_sd->get_fs_code();
		jshader.append(jshd_unit);
	}
	jroot["shader_list"] = jshader;
	Value jmaterial(arrayValue);
	for (auto& mtl_ut : g_material_list)
	{
		Value jmtl_ut(objectValue);
		auto& pmtl = mtl_ut.second;
		pmtl->output_2_json(jmtl_ut);
		jmaterial.append(jmtl_ut);
	}
	jroot["material_list"] = jmaterial;

	_root.init_json_unit(jroot);
	fout << jroot << endl;
	fout.close();
	return true;
}
