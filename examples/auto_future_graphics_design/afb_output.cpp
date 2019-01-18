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
#include "material.h"
#include "dir_output.h"
#include "file_outputor.h"
#include "af_font_res_set.h"
#include "common_functions.h"
extern "C"{
#include "image_DXT.h"
}
#include "SOIL.h"
#define _DX5_COMPRESS
#define STB_DXT_IMPLEMENTATION

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
	pk.pack_array(10);
	pk.pack_float(base_ui_component::screenw);
	pk.pack_float(base_ui_component::screenh);
	vfont_face_name& ft_nm_list = g_pfont_face_manager->get_font_name_list();
	string font_fold_path = g_cureent_directory + font_fold;
	pk.pack_array(ft_nm_list.size());
	for (auto& face_name_item : ft_nm_list)
	{
		//auto& cfg_data = atlas->ConfigData[ii];
		pk.pack_array(2);

		pk.pack_str(face_name_item.size());
		pk.pack_str_body(face_name_item.c_str(), face_name_item.size());
		string font_name_path = font_fold_path + face_name_item;
		handle_file_data(font_name_path, [&](char* file_buff, unsigned int data_len){
			pk.pack_bin(data_len);
			pk.pack_bin_body(file_buff, data_len);
		});

		
	}
	pk.pack_array(2);
	pk.pack_int(g_output_bin_format._txt_fmt);
	pk.pack_int(g_output_bin_format._pgm_fmt);
	string output_file_path = g_cureent_directory + "afb\\";
	int idx = 0;
	file_outputor fout_put(output_file_path);
	pk.pack_array(g_vres_texture_list.size());
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


	pk.pack_array(g_mtexture_list.size());
	for (auto& txt_unit : g_mtexture_list)
	{
		auto& kname = txt_unit.first;
		auto& txtv = txt_unit.second;
		pk.pack_array(4);
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
		if (DDS_data != txt_data)
		{
			SOIL_free_image_data(DDS_data);
		}
		delete[] txt_data;
		DDS_data = NULL;
		DDS_size = 0;
	}
	pk.pack_array(g_mfiles_list.size());
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
	pk.pack_array(g_af_shader_list.size());

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
	pk.pack_array(g_material_list.size());
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
			auto wsize = shd_uf.second->usize()*shd_uf.second->elsize();
			pk.pack_bin(wsize);
			pk.pack_bin_body(reinterpret_cast<char const*>(shd_uf.second->get_data_head()), wsize);
		}
	}
	pack_ui_component_data(_pj, pk);
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