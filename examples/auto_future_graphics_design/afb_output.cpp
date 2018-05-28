#include "afb_output.h"
#include <msgpack.hpp>
#include "res_output.h"
#include <fstream>
#include <sstream>
#include <memory>
#include <iostream>
#include "imgui.h"
#include <GL/gl3w.h>

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
	/*MsgPack::array ppty;
	for (auto& pt_unit : vplist)
	{
		uint8_t* pua = (uint8_t*)pt_unit._p_head_address;
		MsgPack::binary ptmm(pua, pua + pt_unit._len);
		MsgPack::object ptmo{ { "pt_mem", ptmm } };
		ppty.push_back(ptmo);
	}
	pt_object["property"] = ppty;
	int chldcnt = tar.child_count();
	if (chldcnt>0)
	{
		MsgPack::array child_list;
		for (size_t ix = 0; ix < chldcnt; ix++)
		{
			base_ui_component* pchild = tar.get_child(ix);
			pack_ui_component_data(*pchild, pk);
		}
		pt_object["childs"] = child_list;
	}
	*/
}
void afb_output::output_afb(const char* afb_file)
{
	ofstream fout;
	fout.open(afb_file, ios::binary);
	msgpack::packer<ofstream> pk(&fout);
	pk.pack_array(4);
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
		pk.pack_bin(txt_size);
		pk.pack_bin_body(reinterpret_cast<char const*>(txtdata), txt_size);

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
		delete txtdata;
		
	}
	pack_ui_component_data(_pj, pk);
	/**
	MsgPack mg_pj;
	MsgPack::object mg_root = { { "screenw", base_ui_component::screenw },
	                            { "screenh", base_ui_component::screenh },
							{ "current_txt_index", g_cur_texture_id_index } };
	MsgPack::array mg_res_list;
	for (auto& res_unit : g_vres_texture_list)
	{
		MsgPack::binary txtdata;
		txtdata.reserve(res_unit.texture_width*res_unit.texture_height * 4);
		txtdata.resize(res_unit.texture_width*res_unit.texture_height * 4);
		glBindTexture(GL_TEXTURE_2D, res_unit.texture_id);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &txtdata[0]);
		MsgPack::array texture_coordinate_list;
		for (auto& tcd_unit:res_unit.vtexture_coordinates)
		{
			texture_coordinate_list.push_back(MsgPack::object{ { "file_name", tcd_unit._file_name }, 
			{ "x0", tcd_unit._x0 }, { "y0", tcd_unit._y0 }, { "x1", tcd_unit._x1 }, { "y1", tcd_unit._y1 } });
		}
		mg_res_list.push_back(MsgPack::object{ { "txtpk_file", txtdata },
		{ "txtpk_w", res_unit.texture_width },{ "txtpk_h", res_unit.texture_height }, { "txtpk_fmt", texture_coordinate_list } });
	}
	mg_root["txtres_list"] = mg_res_list;

	mg_root["control_objects"] = get_ui_component_data(_pj);
	mg_pj = mg_root;
	string str_output;
	mg_pj.dump(str_output);
	fout << str_output << endl;
	*/
	fout.close();
}