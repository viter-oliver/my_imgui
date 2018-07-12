#include "ui_assembler.h"
#include <fstream>
#include "texture_res_load.h"
#include "af_shader.h"
#include "material.h"
#ifdef IMGUI_WAYLAND
#include "../../deps/glad/glad.h"
#else
#include <GL/gl3w.h> 
#endif
#include "SOIL.h"

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
			Value& texture_list = jroot["texture_list"];
			auto isize = texture_list.size();
			UInt ix = 0;
			extern string g_cureent_project_file_path;
			string str_img_path = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
			str_img_path += "images\\";

			for (ix = 0; ix < isize;ix++)
			{
				Value& txt_unit = texture_list[ix];
				auto& kname = txt_unit.asString();
				auto img_file_path = str_img_path + kname;
				GLubyte* imgdata = NULL;
				int width, height, channels;

				imgdata = SOIL_load_image(img_file_path.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
				if (imgdata == NULL)
				{
					printf("Fail to load texture file:%s\n", img_file_path.c_str());
					break;
				}
				GLuint textureId = 0;
				glGenTextures(1, &textureId);
				glBindTexture(GL_TEXTURE_2D, textureId);
				if (channels == SOIL_LOAD_RGBA)
				{
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}
				// Step2 设定wrap参数
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				// Step3 设定filter参数
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					GL_LINEAR_MIPMAP_LINEAR); // 为MipMap设定filter方法
				// Step4 加载纹理

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
					0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);
				glGenerateMipmap(GL_TEXTURE_2D);
				// Step5 释放纹理图片资源
				SOIL_free_image_data(imgdata);
				auto pimge = make_shared<af_texture>();
				pimge->_txt_id = textureId;
				pimge->_width = width;
				pimge->_height = height;
				g_mtexture_list[kname] = pimge;
			}
			Value& shader_list = jroot["shader_list"];
			isize = shader_list.size();
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
	Value texture_list(arrayValue);
	for (auto& txt : g_mtexture_list)
	{
		auto& kname = txt.first;
		Value txt_unit(kname);
		//auto& af_txt = txt.second;
		texture_list.append(txt_unit);
	}
	jroot["texture_list"] = texture_list;
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
