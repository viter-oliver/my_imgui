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
#define STB_DXT_IMPLEMENTATION
#include "dxt5_compress.h"
void pack_ui_component_data(base_ui_component& tar, msgpack::packer<ofstream>& pk)
{
	int chldcnt = tar.child_count();
	if (chldcnt > 0)
		pk.pack_array(3);
	else
        pk.pack_array(2);
	string cname = typeid(tar).name();
	cname = cname.substr(sizeof("class"));
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
void afb_output::output_afb(const char* afb_file)
{
	ofstream fout;
	fout.open(afb_file, ios::binary);
	msgpack::packer<ofstream> pk(&fout);
	pk.pack_array(6);
	pk.pack_float(base_ui_component::screenw);
	pk.pack_float(base_ui_component::screenh);
	pk.pack_array(g_vres_texture_list.size());
	for (auto& res_unit : g_vres_texture_list)
	{
		pk.pack_array(4);
		pk.pack_int(res_unit.texture_width);
		pk.pack_int(res_unit.texture_height);
		int txt_size = res_unit.texture_width*res_unit.texture_height * 4;
		uint8_t* txtdata = new uint8_t[txt_size];

		glBindTexture(GL_TEXTURE_2D, res_unit.texture_id);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, txtdata);
		
		uint32_t blockDim[2] = { 4, 4 };

		const uint32_t blocksWide = (res_unit.texture_width + blockDim[0] - 1) / blockDim[0];
		const uint32_t blocksHigh = (res_unit.texture_height + blockDim[1] - 1) / blockDim[1];
		const uint32_t uncompBlockSize = blockDim[0] * blockDim[1] * sizeof(uint32_t);
		const uint32_t nBlocks = blocksWide * blocksHigh;
		const uint32_t blockSz = 16;
		uint32_t linear_size =nBlocks * blockSz;
		uint8_t* txtdata_compress = new uint8_t[linear_size];
		compress_image_2_dxt5(txtdata_compress, txtdata, res_unit.texture_width, res_unit.texture_height);

		/*uint8_t* txtdata_compress = DXT5Compress(txtdata, linear_size, txt_size, res_unit.texture_width, res_unit.texture_height);*/
		pk.pack_bin(linear_size);
		pk.pack_bin_body(reinterpret_cast<char const*>(txtdata_compress), linear_size);

		pk.pack_array(res_unit.vtexture_coordinates.size());
		for (auto& tcd_unit : res_unit.vtexture_coordinates)
		{
			pk.pack_array(5);
			pk.pack_str(tcd_unit._file_name.size());
			pk.pack_str_body(tcd_unit._file_name.c_str(), tcd_unit._file_name.size());
			pk.pack_float(tcd_unit._x0);
			pk.pack_float(tcd_unit._x1);
			pk.pack_float(tcd_unit._y0);
			pk.pack_float(tcd_unit._y1);
		}
		delete[] txtdata_compress;
		delete[] txtdata;
	}
	pk.pack_array(g_af_shader_list.size());
	for (auto& shd_ut : g_af_shader_list)
	{
		pk.pack_array(3);
		pk.pack_str(shd_ut.first.size());
		pk.pack_str_body(shd_ut.first.c_str(), shd_ut.first.size());
		pk.pack_str(shd_ut.second->get_vs_code().size());
		pk.pack_str_body(shd_ut.second->get_vs_code().c_str(), shd_ut.second->get_vs_code().size());
		pk.pack_str(shd_ut.second->get_fs_code().size());
		pk.pack_str_body(shd_ut.second->get_fs_code().c_str(), shd_ut.second->get_fs_code().size());
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
	fout.close();
}