#include "ui_assembler.h"
#include <fstream>
#include "texture_res_load.h"
#include "af_shader.h"
#include "material.h"

#include "SOIL.h"
#include "dir_output.h"
#include "primitive_object.h"
#include "./fbx_save_info.h"
extern string g_cureent_directory;
extern bool show_project_window, show_edit_window, \
show_property_window, show_resource_manager, show_fonts_manager, show_file_manager;
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
			Value& window_show = jroot["window_show"];
			if (!window_show.isNull())
			{
				show_project_window = window_show["show_project_window"].asBool();
				show_edit_window=window_show["show_edit_window"].asBool();
				show_property_window = window_show["show_property_window"].asBool();
				show_resource_manager = window_show["show_resource_manager"].asBool();
				show_fonts_manager = window_show["show_fonts_manager"].asBool();
				show_file_manager=window_show["show_file_manager"].asBool();
			}
			Value& fonts = jroot["fonts"];
			if (!fonts.isNull())
			{
				string str_font_path = g_cureent_directory+font_fold;
				int isz = fonts.size();
				for (int ix = 0; ix < isz;ix++)
				{
					Value& jfont = fonts[ix];
					string font_name = jfont["name"].asString();
					font_name = str_font_path + font_name;
					float SizePixels = jfont["SizePixels"].asDouble();
					ImFont* nfont=ImGui::GetIO().Fonts->AddFontFromFileTTF(font_name.c_str(), SizePixels, NULL, ImGui::GetIO().Fonts->GetGlyphRangesChinese());
					Value jdefault = jfont["default"];
					if (!jdefault.isNull())
					{
						ImGui::GetIO().FontDefault = nfont;
					}
				}

			}
			Value& output_bin_fmt = jroot["output_bin_fmt"];
			if (!output_bin_fmt.isNull())
			{
				g_output_bin_format._txt_fmt = static_cast<texture_format> (output_bin_fmt["txt_fmt"].asInt());
				g_output_bin_format._pgm_fmt = static_cast<program_format> (output_bin_fmt["pgm_fmt"].asInt());
			}
			texture_res_load tresload(g_vres_texture_list);
			tresload.load_res_from_json(jroot);
			Value& texture_list = jroot["texture_list"];
			auto isize = texture_list.size();
			UInt ix = 0;
			string str_img_path = g_cureent_directory+image_fold;
			string str_files_path = g_cureent_directory + files_fold;
			for (ix = 0; ix < isize;ix++)
			{
				Value& txt_unit = texture_list[ix];
				auto& kname = txt_unit["name"].asString();
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
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//					GL_LINEAR_MIPMAP_LINEAR); // 为MipMap设定filter方法
				// Step4 加载纹理

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
					0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);
				//glGenerateMipmap(GL_TEXTURE_2D);
				// Step5 释放纹理图片资源
				SOIL_free_image_data(imgdata);
				auto pimge = make_shared<af_texture>();
				pimge->_atxt_id = textureId;
				pimge->_width = width;
				pimge->_height = height;
				pimge->_is_separated = txt_unit["separated"].asBool();
				g_mtexture_list[kname] = pimge;
			}
			Value& file_list = jroot["file_list"];
			auto fsize = file_list.size();
			for (ix = 0; ix < fsize; ix++)
			{
				Value& filse_unit = file_list[ix];
				auto& kname = filse_unit.asString();
				auto file_path = str_files_path + kname;
				ifstream ifs;
				ifs.open(file_path);
				filebuf* pbuf = ifs.rdbuf();
				size_t sz_file = pbuf->pubseekoff(0, ifs.end, ifs.in);
				pbuf->pubseekpos(0, ifs.in);
				g_mfiles_list[kname] = make_shared<af_file>(sz_file);
				pbuf->sgetn((char*)g_mfiles_list[kname]->_pbin, sz_file);
				ifs.close();
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
			Value& primitive_list = jroot["primitive_list"];
			isize = primitive_list.size();
			for (ix = 0; ix < isize; ix++)
			{
				Value& jpm = primitive_list[ix];
				auto& kname = jpm["name"].asString();

				Value& vformat = jpm["format"];
				int ebo_len = vformat.size();
				vector<GLubyte> ele_format;
				for (int ii = 0; ii < ebo_len; ii++)
				{
					Value& fmu = vformat[ii];
					ele_format.emplace_back(fmu.asInt());
				}
				//GL_UNSIGNED_INT_8_8_8_8_REV
				int vbo_len = jpm["vbo_len"].asInt();
				ebo_len = jpm["ebo_len"].asInt();
				load_primitive_from_file(kname, ele_format, vbo_len, ebo_len);
			}
			_root.init_property_from_json(jroot);//
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
	output_primitive_to_file();

	ofstream fout;
	fout.open(file_path);
	Value jroot(objectValue);
	jroot["screenw"] = base_ui_component::screenw;
	jroot["screenh"] = base_ui_component::screenh;
	Value window_show(objectValue);
	window_show["show_project_window"] = show_project_window;
	window_show["show_edit_window"] = show_edit_window;
	window_show["show_property_window"] = show_property_window;
	window_show["show_resource_manager"] = show_resource_manager;
	window_show["show_fonts_manager"] = show_fonts_manager;
	window_show["show_file_manager"] = show_file_manager;

	jroot["window_show"] = window_show;
	Value fonts(arrayValue);
	ImFontAtlas* atlas = ImGui::GetIO().Fonts;
	
	for (size_t i = 0; i < atlas->Fonts.size(); i++)
	{
		auto& cfg_data = atlas->ConfigData[i];
		ImFont* font = atlas->Fonts[i];
		Value jfont(objectValue);
		string fontname = cfg_data.Name;
		fontname = fontname.substr(0, fontname.find_first_of(','));
		if ("ProggyClean.ttf" == fontname)//默认字体不需要存
		{
			continue;
		}
		jfont["SizePixels"] = cfg_data.SizePixels;
		jfont["name"] = fontname;
		if (ImGui::GetIO().FontDefault==font)
		{
			jfont["default"] = true;
		}
		fonts.append(jfont);
	}
	jroot["fonts"] = fonts;
	Value output_bin_fmt(objectValue);
	output_bin_fmt["txt_fmt"] = g_output_bin_format._txt_fmt;
	output_bin_fmt["pgm_fmt"] = g_output_bin_format._pgm_fmt;
	jroot["output_bin_fmt"] = output_bin_fmt;
	Value jtexture(arrayValue);
	for (auto& reslist:g_vres_texture_list)
	{
		Value jtext_res_unit(objectValue);
		jtext_res_unit["texture_pack_file"] = reslist.texture_pack_file;
		jtext_res_unit["texture_data_file"] = reslist.texture_data_file;
		jtext_res_unit["separated"] = reslist._is_separated;
		jtexture.append(jtext_res_unit);
	}
	jroot["texture_res_list"] = jtexture;
	Value texture_list(arrayValue);
	for (auto& txt : g_mtexture_list)
	{
		auto& kname = txt.first;
		Value txt_unit(objectValue);
		txt_unit["name"] = kname;
		txt_unit["separated"] = txt.second->_is_separated;
		//auto& af_txt = txt.second;
		texture_list.append(txt_unit);
	}
	jroot["texture_list"] = texture_list;
	Value file_list(arrayValue);
	for (auto& fileu:g_mfiles_list)
	{
		auto& kname = fileu.first;
		Value jfile(kname);
		file_list.append(jfile);
	}
	jroot["file_list"] = file_list;
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
	Value jprimitive_list(arrayValue);
	for (auto& pm:g_primitive_list)
	{
		Value jpm(objectValue);
		jpm["name"] = pm.first; 
		
		auto& pmu = pm.second;	
		Value jformat(arrayValue);
		for (auto& fmu:pmu->_ele_format)
		{
			jformat.append(fmu);
		}
		jpm["format"] = jformat;
		jpm["vbo_len"] = pmu->_vertex_buf_len;
		jpm["ebo_len"] = pmu->_ele_buf_len;
		jprimitive_list.append(jpm);
	}
	jroot["primitive_list"] = jprimitive_list;

	jroot["texture_id_index"] = g_cur_texture_id_index;
	_root.save_property_to_json(jroot);
	fout << jroot << endl;
	fout.close();

	save_fbx_file();

	return true;
}
int real_id_after_update(string& file_name)
{
	res_texture_list& cur_txt_list = g_vres_texture_list[g_cur_texture_id_index];
	vres_txt_cd& cur_txt_cd = cur_txt_list.vtexture_coordinates;
	for (int ix = 0; ix < cur_txt_cd.size();++ix)
	{
		if (file_name==cur_txt_cd[ix]._file_name)
		{
			return ix;
		}
	}
	return 0;
}
void update_ui_component_texture_res_index(base_ui_component& tar, vres_txt_cd& old_txt_cd)
{
	vp_prop_ele& cur_vprop_eles = tar.get_prop_ele();
	for (auto& prop_ele:cur_vprop_eles)
	{
		auto& prop_page = prop_ele->_pro_page;
		for (auto& memb:prop_page)
		{
			auto mtype = memb->_type;
			auto mname = memb->_name;
			auto moffset = memb->_offset;
			string rg = mname.substr(mname.length() - 3, 3);
			if (mtype=="int"&&rg=="txt")
			{
				void* memb_address = (char*)prop_ele->_pro_address + moffset;
			    int* ptxtidx = (int*)memb_address;
				int ix_old = *ptxtidx;
				*ptxtidx = real_id_after_update(old_txt_cd[ix_old]._file_name);
			}
		}
	}
	int chldcnt = tar.child_count();
	for (size_t ix = 0; ix < chldcnt;++ix)
	{
		base_ui_component* pchild = tar.get_child(ix);
		update_ui_component_texture_res_index(*pchild, old_txt_cd);
	}

}
bool ui_assembler::update_texture_res()
{
	res_texture_list& cur_txt_list=g_vres_texture_list[g_cur_texture_id_index];
	vres_txt_cd cur_txt_cd = cur_txt_list.vtexture_coordinates;
	unsigned int txtid = cur_txt_list.texture_id();
	glDeleteTextures(1, &txtid);
	cur_txt_list.txt_id = 0;
	cur_txt_list.vtexture_coordinates.clear();
	load_texture_info(cur_txt_list, cur_txt_list.texture_pack_file, cur_txt_list.texture_data_file);
	int chldcnt = _root.child_count();
	for (size_t ix = 0; ix < chldcnt; ++ix)
	{
		base_ui_component* pchild = _root.get_child(ix);
		update_ui_component_texture_res_index(*pchild, cur_txt_cd);
	}
	return true;
}

void ui_assembler::output_primitive_to_file()
{
	for (auto& pm : g_primitive_list)
	{
		auto& pmu = pm.second;
		if (pmu->_vertex_buf_len)
		{
			ofstream ofin(g_cureent_directory + files_fold + pm.first + ".vbo", ios::trunc | ios::out | ios::binary);
			if (ofin.is_open())
			{
				glBindBuffer(GL_ARRAY_BUFFER, pmu->_vbo);
				GLfloat* pvbo = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);

				printf("%f\n", pvbo[pmu->_vertex_buf_len-1]);

				ofin.write((char *)pvbo, pmu->_vertex_buf_len * sizeof(GLfloat));
				glUnmapBuffer(GL_ARRAY_BUFFER);
				ofin.close();
			}
		}

		if (pmu->_ele_buf_len)
		{
			ofstream ofin(g_cureent_directory + files_fold + pm.first + ".ebo", ios::trunc | ios::out | ios::binary);
			if (ofin.is_open())
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pmu->_ebo);
				GLshort* pebo = (GLshort*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
				ofin.write((const char *)pebo, pmu->_ele_buf_len * sizeof(GLshort));
				glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
				ofin.close();
			}
		}
	}
}

void ui_assembler::load_primitive_from_file(string &kname, vector<GLubyte> ele_format, int vbo_len, int ebo_len)
{
	GLfloat* pvbo = 0;
	if (vbo_len)
	{
		ifstream ivfin(g_cureent_directory + files_fold + kname + ".vbo", ios::binary);
		if (ivfin.is_open())
		{
			pvbo = new GLfloat[vbo_len];
			ivfin.read((char *)pvbo, vbo_len*sizeof(GLfloat));
			ivfin.close();
		}
	}

	GLushort* pebo = 0;
	if (ebo_len)
	{
		ifstream iefin(g_cureent_directory + files_fold + kname + ".ebo", ios::binary);
		if (iefin.is_open())
		{
			pebo = new GLushort[ebo_len];
			iefin.seekg(0, ios::beg);
			iefin.read((char *)pebo, ebo_len*sizeof(GLushort));
			iefin.close();
		}
	}

	g_primitive_list[kname] = make_shared<primitive_object>();
	g_primitive_list[kname]->set_ele_format(ele_format);
	g_primitive_list[kname]->load_vertex_data(pvbo, vbo_len, pebo, ebo_len);
	delete[] pvbo;
	if (pebo)
	{
		delete[] pebo;
	}
}