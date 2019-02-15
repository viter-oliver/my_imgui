#include "ui_assembler.h"
#include <fstream>
#include "texture_res_load.h"
#include "af_shader.h"
#include "material.h"
#include "af_font_res_set.h"
#include "SOIL.h"
#include "dir_output.h"
#include "primitive_object.h"
#include "af_model.h"
//#include "af_model.h"
//#include "./fbx_save_info.h"
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
			g_pfont_face_manager = make_shared<font_face_manager>();
			if (!fonts.isNull())
			{
				string str_font_path = g_cureent_directory+font_fold;
				int isz = fonts.size();
				for (int ix = 0; ix < isz;ix++)
				{
					Value& jfont = fonts[ix];
					string font_name = jfont.asString();
					string font_full_name= str_font_path + font_name;
					g_pfont_face_manager->load_font(font_name, font_full_name);
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
				pimge->_loaded = true;
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
				GLuint ebo_len = vformat.size();
				vector<GLubyte> ele_format;
				for (int ii = 0; ii < ebo_len; ii++)
				{
					Value& fmu = vformat[ii];
					ele_format.emplace_back(fmu.asInt());
				}
				//GL_UNSIGNED_INT_8_8_8_8_REV
				GLuint vbo_len = jpm["vbo_len"].asUInt();
				ebo_len = jpm["ebo_len"].asUInt();
				load_primitive_from_file(kname, ele_format, vbo_len, ebo_len);
			}
			 g_cur_texture_id_index=jroot["texture_id_index"].asInt();
			 Value& models = jroot["models"];
			 if (models.isObject())
			 {
				 Value::Members md_mb = models.getMemberNames();
				 for (auto& md_nm:md_mb)
				 {
					 Value& jmeshlist = models[md_nm];
					 auto pmodel = make_shared<af_model>();
					 g_mmodel_list[md_nm] = pmodel;
					 af_model& mdl = *pmodel;
					 int iisz = jmeshlist.size();
					 for (int iix = 0; iix < iisz; ++iix)
					 {
						 Value& jmesh_unit = jmeshlist[iix];
						 mdl.emplace_back();
						 af_mesh& mesh_unit = mdl[iix];
						 mesh_unit._prm_id = jmesh_unit["prim_id"].asString();
						 Value& jdiffuse_list = jmesh_unit["diffuse_list"];
						 int jdsize = jdiffuse_list.size();
						 for (int jd = 0; jd < jdsize; ++jd)
						 {
							 auto& diff_txt = jdiffuse_list[jd].asString();
							 mesh_unit._text_diffuse_list.emplace_back(diff_txt);
						 }
						 Value& jspecular_list = jmesh_unit["specular_list"];
						 jdsize = jspecular_list.size();
						 for (int jd = 0; jd < jdsize; ++jd)
						 {
							 auto& specular_txt = jspecular_list[jd].asString();
							 mesh_unit._text_specular_list.emplace_back(specular_txt);
						 }
						 Value& jheight_list = jmesh_unit["height_list"];
						 jdsize = jheight_list.size();
						 for (int jd = 0; jd < jdsize; ++jd)
						 {
							 auto& height_txt = jheight_list[jd].asString();
							 mesh_unit._text_height_list.emplace_back(height_txt);
						 }
						 Value& jambient_list = jmesh_unit["ambient_list"];
						 jdsize = jambient_list.size();
						 for (int jd = 0; jd < jdsize; ++jd)
						 {
							 auto& ambient_txt = jambient_list[jd].asString();
							 mesh_unit._text_ambient_list.emplace_back(ambient_txt);
						 }
					 }
				 }
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
	vfont_face_name& ft_nm_list = g_pfont_face_manager->get_font_name_list();
	for (auto& face_name_item:ft_nm_list)
	{
		fonts.append(face_name_item);
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
	for (auto& fileu : g_mfiles_list)
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
	Value jmodels(objectValue);

	for (auto& model_unit : g_mmodel_list)
	{
		Value jmesh_list(arrayValue);
		auto& mesh_list = *model_unit.second;
		for (auto& mesh_unit : mesh_list)
		{
			Value jmesh_unit(objectValue);
			jmesh_unit["prim_id"] = mesh_unit._prm_id;
			Value diffuse_list(arrayValue);
			for (auto& diffuse : mesh_unit._text_diffuse_list)
			{
				diffuse_list.append(diffuse);
			}
			jmesh_unit["diffuse_list"] = diffuse_list;
			Value spcular_list(arrayValue);
			for (auto& specular : mesh_unit._text_specular_list)
			{
				spcular_list.append(specular);
			}
			jmesh_unit["spcular_list"] = spcular_list;
			Value height_list(arrayValue);
			for (auto& height : mesh_unit._text_height_list)
			{
				height_list.append(height);
			}
			jmesh_unit["height_list"] = height_list;
			Value ambient_list(arrayValue);
			for (auto& ambient : mesh_unit._text_ambient_list)
			{
				ambient_list.append(ambient);
			}
			jmesh_unit["ambient_list"] = ambient_list;

			jmesh_list.append(jmesh_unit);
		}
		jmodels[model_unit.first] = jmesh_list;
	}
	jroot["models"] = jmodels;
	_root.save_property_to_json(jroot);
	fout << jroot << endl;
	fout.close();

	//save_fbx_file();

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
		auto& pkey = pm.first;
		string file_key = pkey + ".vbo";
		auto ifile = g_mfiles_list.find(file_key);
		if (ifile!=g_mfiles_list.end())
		{
			continue;
		}
		auto& pmu = pm.second;
		GLuint file_size = pmu->_vertex_buf_len * sizeof(GLfloat);
		g_mfiles_list[file_key] = make_shared<af_file>(file_size);
		glBindBuffer(GL_ARRAY_BUFFER, pmu->_vbo);
		GLfloat* pvbo = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		memcpy(g_mfiles_list[file_key]->_pbin, pvbo, file_size);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		if (pmu->_ele_buf_len)
		{
			auto file_ebo_key = pkey + ".ebo";
			GLuint efile_size = pmu->_ele_buf_len * sizeof(GLuint);
			g_mfiles_list[file_ebo_key] = make_shared<af_file>(efile_size);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pmu->_ebo);
			GLuint* pebo = (GLuint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
			memcpy(g_mfiles_list[file_ebo_key]->_pbin, pebo, efile_size);
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		}
	}
}

void ui_assembler::load_primitive_from_file(string &kname, vector<GLubyte> ele_format, GLuint vbo_len, GLuint ebo_len)
{
	GLfloat* pvbo = 0;
	GLuint* pebo = 0;
	auto vbo_key = kname + ".vbo";
	auto ifile = g_mfiles_list.find(vbo_key);
	if (ifile!=g_mfiles_list.end())
	{
		auto& pfobj = ifile->second;
		auto& fopj = *pfobj;
		pvbo = (GLfloat*)fopj._pbin;
	}
	

	if (ebo_len)
	{
		auto ebo_key = kname + ".ebo";
		auto iefile = g_mfiles_list.find(ebo_key);
		if (iefile!=g_mfiles_list.end())
		{
			auto& pefile = iefile->second;
			auto& efile = *pefile;
			pebo = (GLuint*)efile._pbin;
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