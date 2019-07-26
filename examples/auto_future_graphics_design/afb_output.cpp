#include "afb_output.h"
#include <msgpack.hpp>
#include "res_output.h"
#include <fstream>
#include <sstream>
#include <memory>
#include <iostream>
#include "imgui.h"
#include <GL/gl3w.h>
#include "af_shader.h"
#include "af_material.h"
#include "dir_output.h"
#include "file_outputor.h"
#include "af_primitive_object.h"
#include "af_model.h"
#include "af_font_res_set.h"
#include "af_bind.h"
#include "af_state_manager.h"
#include "common_functions.h"
extern "C"{
#include "image_DXT.h"
}
#include "SOIL.h"
#define _DX5_COMPRESS
#define STB_DXT_IMPLEMENTATION
#include "afb_res_index.h"
#include "miniz.h"

void pack_ui_component_data(base_ui_component& tar,
#ifdef _DX5_COMPRESS
	msgpack::packer<ofstream>& pk
#else
	msgpack::packer<msgpack::sbuffer>& pk
#endif
	)
{
	int chldcnt = tar.child_count();
	if (chldcnt > 0)
		pk.pack_array(3);
	else
        pk.pack_array(2);
	string cname = typeid(tar).name();
	cname = cname.substr(sizeof("class autofuture::"));
	pk.pack_str(cname.size());
	pk.pack_str_body(cname.c_str(), cname.size());
	vproperty_list vplist;
	int mlen=tar.collect_property_range(vplist);
	uint8_t* pmem = new uint8_t[mlen];
	uint8_t* pms = pmem;
	for (auto& pt_unit : vplist)
	{
		uint8_t* pua = (uint8_t*)pt_unit._p_head_address;
		memcpy(pmem, pt_unit._p_head_address, pt_unit._len);
		pmem += pt_unit._len;
	}
	pk.pack_bin(mlen);
	pk.pack_bin_body(reinterpret_cast<char const*>(pms), mlen);
	delete[] pms;
	if (chldcnt>0)
	{
		pk.pack_array(chldcnt);
		for (size_t ix = 0; ix < chldcnt; ix++)
		{
			base_ui_component* pchild = tar.get_child(ix);
			pack_ui_component_data(*pchild, pk);
		}
	}
}
extern GLuint       g_FontTexture;
extern string g_cureent_directory;
void afb_output::output_afb(const char* afb_file)
{
	ofstream fout;
	fout.open(afb_file, ios::binary);
#ifdef _DX5_COMPRESS
	msgpack::packer<ofstream> pk(&fout);
#else
	msgpack::sbuffer sbuff;
	msgpack::packer<msgpack::sbuffer> pk(sbuff);
#endif
	pk.pack_array(en_afb_res_cnt);
	pk.pack_float(base_ui_component::screenw);//en_screen_w
	pk.pack_float(base_ui_component::screenh);//en_screen_h
	//vfont_face_name& ft_nm_list = g_pfont_face_manager->get_font_name_list();
	auto& dic_ft = g_pfont_face_manager->get_dic_fonts();
	string font_fold_path = g_cureent_directory + font_fold;
	pk.pack_array(dic_ft.size());//en_font_faces
	for (auto& ft_item : dic_ft)
	{
		//auto& cfg_data = atlas->ConfigData[ii];
		pk.pack_array(4);
		auto&face_name_item = ft_item->_name;
		pk.pack_str(face_name_item.size());
		pk.pack_str_body(face_name_item.c_str(), face_name_item.size());
		auto fcols = ft_item->_char_count_c;
		auto frows = ft_item->_char_count_r;
		pk.pack_int(fcols);
		pk.pack_int(frows);
		string font_name_path = font_fold_path + face_name_item;
		handle_file_data(font_name_path, [&](char* file_buff, unsigned int data_len){
			pk.pack_bin(data_len);
			pk.pack_bin_body(file_buff, data_len);
		});

		
	}
	pk.pack_array(2);//en_output_bin_format
	pk.pack_int(g_output_bin_format._txt_fmt);
	pk.pack_int(g_output_bin_format._pgm_fmt);
	string output_file_path = g_cureent_directory + "afb\\";
	int idx = 0;
	file_outputor fout_put(output_file_path);
	pk.pack_array(g_vres_texture_list.size());//en_vtextures_res
	function<uint8_t*(uint8_t*, int,int,int&)> ftxt_press;
	if (g_output_bin_format._txt_fmt == en_uncompressed_txt){
		ftxt_press = [](uint8_t* prawdata, int iwidth,int iheight, int&out_sz)->uint8_t*{
			out_sz = iwidth*iheight*4;
			return prawdata;
		};
	}
	else if (g_output_bin_format._txt_fmt == en_dxt5){
		ftxt_press = [](uint8_t* prawdata, int iwidth, int iheight, int&out_sz)->uint8_t*{
			uint8_t* pDDS_data = convert_image_to_DXT5(prawdata, iwidth, iheight, SOIL_LOAD_RGBA, &out_sz);
			return pDDS_data;
		};
	}
	int DDS_size = 0;
	unsigned char *DDS_data = NULL;
	for (auto& res_unit : g_vres_texture_list)
	{
		pk.pack_array(5);
		pk.pack_str(res_unit.texture_pack_file.size());
		pk.pack_str_body(res_unit.texture_pack_file.c_str(), res_unit.texture_pack_file.size());
		pk.pack_int(res_unit.texture_width);
		pk.pack_int(res_unit.texture_height);
		int txt_size = res_unit.texture_width*res_unit.texture_height * 4;
		uint8_t* txtdata = new uint8_t[txt_size];

		glBindTexture(GL_TEXTURE_2D, res_unit.texture_id());
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, txtdata);
		DDS_data = ftxt_press(txtdata, res_unit.texture_width, res_unit.texture_height, DDS_size);
		if (res_unit._is_separated)
		{
			pk.pack_uint8(0);
			string str_resname = output_file_path + res_unit.texture_pack_file;
			str_resname += ".safb";
			ofstream fresout;
			fresout.open(str_resname, ios::out | ios::binary);
			fresout.write((char*)DDS_data, DDS_size);
			fresout.close();
		}
		else
		{
			pk.pack_bin(DDS_size);
			pk.pack_bin_body(reinterpret_cast<char const*>(DDS_data), DDS_size);
		}
		if (DDS_data != txtdata)
		{
			SOIL_free_image_data(DDS_data);
		}
		delete[] txtdata;
		pk.pack_array(res_unit.vtexture_coordinates.size());
		string enum_file = "enum_txt_name";
		char str_ix[20] = { 0 };
		sprintf(str_ix, "%d.h", idx);
		enum_file += str_ix;
		idx++;
		fout_put.begin_enum_file(enum_file);
		for (auto& tcd_unit : res_unit.vtexture_coordinates)
		{
			pk.pack_array(5);
			pk.pack_str(tcd_unit._file_name.size());
			pk.pack_str_body(tcd_unit._file_name.c_str(), tcd_unit._file_name.size());
			pk.pack_float(tcd_unit._x0);
			pk.pack_float(tcd_unit._x1);
			pk.pack_float(tcd_unit._y0);
			pk.pack_float(tcd_unit._y1);
			fout_put.push_enum_name(tcd_unit._file_name);
		}
		fout_put.end_enum_file();
	}


	pk.pack_array(g_mtexture_list.size());//en_mtextures_res
	for (auto& txt_unit : g_mtexture_list)
	{
		auto& kname = txt_unit.first;
		auto& txtv = txt_unit.second;
		pk.pack_array(5);
		pk.pack_str(kname.size());
		pk.pack_str_body(kname.c_str(), kname.size());
		pk.pack_uint32(txtv->_width);
		pk.pack_uint32(txtv->_height);
		
		uint32_t txt_size = txtv->_width*txtv->_height * 4;
		uint8_t* txt_data = new uint8_t[txt_size];
		glBindTexture(GL_TEXTURE_2D, txtv->_txt_id());
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, txt_data);
		DDS_data = ftxt_press(txt_data, txtv->_width, txtv->_height, DDS_size);

		if (txtv->_is_separated)
		{
			pk.pack_uint8(0);
			string str_resname = output_file_path + kname;
			str_resname += ".safb";
			ofstream fresout;
			fresout.open(str_resname, ios::out|ios::binary);
			fresout.write((char*)DDS_data, DDS_size);
			fresout.close();
		}
		else
		{
			pk.pack_bin(DDS_size);
			pk.pack_bin_body(reinterpret_cast<char const*>(DDS_data), DDS_size);
		}
		uint8_t mipv = txtv->_mip_map;
		pk.pack_uint8(mipv);
		if (DDS_data != txt_data)
		{
			SOIL_free_image_data(DDS_data);
		}
		delete[] txt_data;
		DDS_data = NULL;
		DDS_size = 0;
	}
	pk.pack_array(g_mfiles_list.size());//en_mfiles_res
	for (auto& file_unit : g_mfiles_list)
	{
		auto& kname = file_unit.first;
		auto& filev = file_unit.second;
		pk.pack_array(2);
		pk.pack_str(kname.size());
		pk.pack_str_body(kname.c_str(), kname.size());
		pk.pack_bin(filev->_fsize);
		pk.pack_bin_body(reinterpret_cast<char const*>(filev->_pbin), filev->_fsize);
	}
	/*GLint formats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
	GLint *binaryFormats = new GLint[formats];
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, binaryFormats);*/
	pk.pack_array(g_af_shader_list.size());//en_shaders_res

	for (auto& shd_ut : g_af_shader_list)
	{
		pk.pack_array(3);
		pk.pack_str(shd_ut.first.size());
		pk.pack_str_body(shd_ut.first.c_str(), shd_ut.first.size());
		if (g_output_bin_format._pgm_fmt == en_shader_code)
		{
			pk.pack_str(shd_ut.second->get_vs_code().size());
			pk.pack_str_body(shd_ut.second->get_vs_code().c_str(), shd_ut.second->get_vs_code().size());
			pk.pack_str(shd_ut.second->get_fs_code().size());
			pk.pack_str_body(shd_ut.second->get_fs_code().c_str(), shd_ut.second->get_fs_code().size());
		}
		else if (g_output_bin_format._pgm_fmt == en_shader_bin_general)
		{
	
			GLuint pid = shd_ut.second->program_id();
			GLint len = 0;
			glGetProgramiv(pid, GL_PROGRAM_BINARY_LENGTH, &len);
			char* binary =new char[len];
			GLenum binaryFormat = 0;
			glGetProgramBinary(pid, len, NULL, (GLenum*)&binaryFormat, binary);
			pk.pack_int(binaryFormat);
			pk.pack_bin(len);
			pk.pack_bin_body(binary, len);
			delete[] binary;
		}

		/*pk.pack_str(shd_ut.second->get_vs_code().size());
		pk.pack_str_body(shd_ut.second->get_vs_code().c_str(), shd_ut.second->get_vs_code().size());
		pk.pack_str(shd_ut.second->get_fs_code().size());
		pk.pack_str_body(shd_ut.second->get_fs_code().c_str(), shd_ut.second->get_fs_code().size());*/
	}
	pk.pack_array(g_material_list.size());//en_materials_res
	for (auto& mtl_ut : g_material_list)
	{
		pk.pack_array(3);
		pk.pack_str(mtl_ut.first.size());
		pk.pack_str_body(mtl_ut.first.c_str(), mtl_ut.first.size());
		auto& pshd = mtl_ut.second->get_shader();
		pk.pack_str(pshd->get_name().size());
		pk.pack_str_body(pshd->get_name().c_str(), pshd->get_name().size());
		auto& mpshd_uf = mtl_ut.second->get_mp_sd_uf();
		pk.pack_array(mpshd_uf.size());
		for (auto& shd_uf : mpshd_uf)
		{
			pk.pack_array(6);
			pk.pack_str(shd_uf.first.size());
			pk.pack_str_body(shd_uf.first.c_str(), shd_uf.first.size());
			string cname = typeid(*shd_uf.second).name();
			cname = cname.substr(sizeof("class"));
			pk.pack_str(cname.size());
			pk.pack_str_body(cname.c_str(), cname.size());
			pk.pack_uint32(shd_uf.second->usize());
			pk.pack_uint32(shd_uf.second->elsize());
			pk.pack_uint32(shd_uf.second->get_type());
			auto wsize = shd_uf.second->data_len();// shd_uf.second->usize()*shd_uf.second->elsize();
			pk.pack_bin(wsize);
			pk.pack_bin_body(reinterpret_cast<char const*>(shd_uf.second->get_data_head()), wsize);
		}
	}
	pk.pack_array(g_primitive_list.size());//en_primitives_res
	for (auto& pm:g_primitive_list)
	{
		pk.pack_array(4);
		pk.pack_str(pm.first.size());
		pk.pack_str_body(pm.first.c_str(),pm.first.size());
		auto& pmu=pm.second;
		pk.pack_uint32(pmu->_vertex_buf_len);
		pk.pack_uint32(pmu->_ele_buf_len);
		pk.pack_bin(pmu->_ele_format.size());
		pk.pack_bin_body((char*)&pmu->_ele_format[0],pmu->_ele_format.size());
	}
	pk.pack_array(g_mmodel_list.size());//en_models_res
	for (auto& modle_unit:g_mmodel_list)
	{
		pk.pack_array(2);
		pk.pack_str(modle_unit.first.size());
		pk.pack_str_body(modle_unit.first.c_str(),modle_unit.first.size());
		auto& mesh_list=*modle_unit.second;
		pk.pack_array(mesh_list.size());
		for (auto& mesh_unit:mesh_list)
		{
			pk.pack_array(5);
			pk.pack_str(mesh_unit._prm_id.size());
			pk.pack_str_body(mesh_unit._prm_id.c_str(), mesh_unit._prm_id.size());
			auto& txt_diffs = mesh_unit._text_diffuse_list;
			pk.pack_array(txt_diffs.size());
			for (auto&txt_dff:txt_diffs)
			{
				pk.pack_str(txt_dff.size());
				pk.pack_str_body(txt_dff.c_str(), txt_dff.size());
			}
			auto& txt_specs = mesh_unit._text_specular_list;
			pk.pack_array(txt_specs.size());
			for (auto&txt_spec:txt_specs)
			{
				pk.pack_str(txt_spec.size());
				pk.pack_str_body(txt_spec.c_str(), txt_spec.size());
			}
			auto& txt_ambients = mesh_unit._text_ambient_list;
			pk.pack_array(txt_ambients.size());
			for (auto&txt_ambient : txt_ambients)
			{
				pk.pack_str(txt_ambient.size());
				pk.pack_str_body(txt_ambient.c_str(), txt_ambient.size());
			}
			auto& txt_heights = mesh_unit._text_height_list;
			pk.pack_array(txt_heights.size());
			for (auto&txt_height : txt_heights)
			{
				pk.pack_str(txt_height.size());
				pk.pack_str_body(txt_height.c_str(), txt_height.size());
			}
		}
	}
	pack_ui_component_data(_pj, pk);//en_control_res
	pk.pack_array(g_aliase_dic.size());//en_aliase_dic
	for (auto& ial:g_aliase_dic)
	{
		pk.pack_array(2);
		auto& ikey = ial.first;
		pk.pack_str(ikey.size());
		pk.pack_str_body(ikey.c_str(), ikey.size());
		auto& opep_pos = *ial.second;
		prop_ele_pos_index pep_id;
		calcu_prop_ele_pos_index(opep_pos, pep_id);
		auto bin_sz = pep_id.size()*sizeof(unsigned short);
		pk.pack_bin(bin_sz);
		pk.pack_bin_body((char*)&pep_id[0], bin_sz);
	}
	pk.pack_array(g_bind_dic.size());//en_bind_dic
	for (auto& ibind_dic:g_bind_dic)
	{
		pk.pack_array(3);
		prop_ele_pos_index pep_id;
		auto& ikey = ibind_dic.first;
		calcu_prop_ele_pos_index(ikey, pep_id);
		auto bin_sz = pep_id.size()*sizeof(unsigned short);
		pk.pack_bin(bin_sz);
		pk.pack_bin_body((char*)&pep_id[0], bin_sz);
		auto& bind_unit = *ibind_dic.second;
		auto& pm_list = bind_unit._param_list;
		pk.pack_array(pm_list.size());
		for (auto& pele_pos:pm_list)
		{
			prop_ele_pos_index sub_pep_id;
			calcu_prop_ele_pos_index(pele_pos, sub_pep_id);
			auto sub_bin_sz = sub_pep_id.size()*sizeof(unsigned short);
			pk.pack_bin(sub_bin_sz);
			pk.pack_bin_body((char*)&sub_pep_id[0], sub_bin_sz);
		}
		auto& expr = bind_unit._expression;
		pk.pack_str(expr.size());
		pk.pack_str_body(expr.c_str(), expr.size());

	}
	pk.pack_array(g_bind_ref_dic.size());//en_bind_ref_dic
	for (auto& ibind_ref_dic:g_bind_ref_dic)
	{
		pk.pack_array(2);
		auto& ikey = ibind_ref_dic.first;
		prop_ele_pos_index pep_id;
		calcu_prop_ele_pos_index(ikey, pep_id);
		auto bin_sz = pep_id.size()*sizeof(unsigned short);
		pk.pack_bin(bin_sz);
		pk.pack_bin_body((char*)&pep_id[0], bin_sz);
		auto& ref_bind_unit = *ibind_ref_dic.second;
		pk.pack_array(ref_bind_unit.size());
		for (auto& iref:ref_bind_unit)
		{
			prop_ele_pos_index sub_pep_id;
			calcu_prop_ele_pos_index(iref, sub_pep_id);
			auto sub_bin_sz = sub_pep_id.size()*sizeof(unsigned short);
			pk.pack_bin(sub_bin_sz);
			pk.pack_bin_body((char*)&sub_pep_id[0], sub_bin_sz);
		}
	}
	pk.pack_array(g_mstate_manager.size());//en_state_manager
	for (auto& ims:g_mstate_manager)
	{
		pk.pack_array(6);
		auto& ikey=ims.first;
		pk.pack_str(ikey.size());
		pk.pack_str_body(ikey.c_str(),ikey.size());
		auto& stm_unit=*ims.second;
		auto& prop_list=stm_unit._prop_list;
		pk.pack_array(prop_list.size());
		for (auto& pp_unit:prop_list)
		{
			prop_ele_pos_index prp_id;
			calcu_prop_ele_pos_index(pp_unit, prp_id);
			auto bin_sz = prp_id.size()*sizeof(unsigned short);
			pk.pack_bin(bin_sz);
			pk.pack_bin_body((char*)&prp_id[0], bin_sz);
		}
		auto& prop_value_list=stm_unit._prop_value_list;
		pk.pack_array(prop_value_list.size());
		for (auto& vprp_value:prop_value_list)
		{
			pk.pack_array(vprp_value.size());
			for (auto& prp_value:vprp_value)
			{
				pk.pack_bin(prp_value.size());
				pk.pack_bin_body(prp_value.c_str(), prp_value.size());
			}
		}
		auto& any_to_any=stm_unit._any_to_any;
		pk.pack_array(3);
		pk.pack_int(any_to_any._start_time);
		pk.pack_int(any_to_any._duration);
		pk.pack_int(any_to_any._easing_func);
		pk.pack_unsigned_char(stm_unit._state_idx);
		auto& mtrans=stm_unit._mtrans;
		pk.pack_array(mtrans.size());
		for (auto& itran:mtrans)
		{
			pk.pack_array(5);
			pk.pack_int(itran.first._from);
			pk.pack_int(itran.first._to);
			auto& tran = *itran.second;
			pk.pack_int(tran._start_time);
			pk.pack_int(tran._duration);
			pk.pack_int(tran._easing_func);
		}
	}
#ifndef _DX5_COMPRESS
	uint8_t* pout_buff = new uint8_t[sbuff.size()];
	mz_stream stream = {};
	mz_deflateInit(&stream, MZ_BEST_SPEED);
	stream.avail_in = (int)sbuff.size();
	stream.next_in = reinterpret_cast<const unsigned char*>( sbuff.data()) ;
	stream.avail_out = (int)sbuff.size();
	stream.next_out = pout_buff;
	int status = mz_deflate(&stream, Z_SYNC_FLUSH);

	if (mz_deflateEnd(&stream) != Z_OK)
	{
		//delete[] pout_buff;
		printf("fail to compress\n");

	}
	else
	{
		fout.write((const char*)pout_buff, stream.total_out);
	}
	delete[] pout_buff;
#endif
	fout.close();

}