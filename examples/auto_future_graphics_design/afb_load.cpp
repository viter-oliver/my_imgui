#include "afb_load.h"
#include <msgpack.hpp>
#include "factory.h"
#include "res_output.h"
#include <fstream>
#include "imgui.h"
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif

void init_ui_component_by_mgo(base_ui_component*&ptar, msgpack::v2::object& obj)
{
	auto obj_arr_sz = obj.via.array.size;
	auto obj_typename = obj.via.array.ptr[0];
	auto obj_tp_nmlen = obj_typename.via.str.size;
	char* str_type_name = new char[obj_tp_nmlen + 1];
	memset(str_type_name, 0, obj_tp_nmlen + 1);
	memcpy(str_type_name, obj_typename.via.str.ptr, obj_tp_nmlen);
	ptar = factory::get().produce(str_type_name);
	delete[] str_type_name;
	vproperty_list vplist;
	int mlen=ptar->collect_property_range(vplist);
	auto mem_obj=obj.via.array.ptr[1];
	uint8_t* pmem = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(mem_obj.via.bin.ptr));
	for (auto& munit : vplist)
	{
		memcpy(munit._p_head_address, pmem, munit._len);
		pmem += munit._len;
	}
	if (obj_arr_sz>2)
	{
		auto childs_obj = obj.via.array.ptr[2];
		auto cd_sz = childs_obj.via.array.size;
		for (size_t ix = 0; ix < cd_sz; ix++)
		{
			auto cd_obj = childs_obj.via.array.ptr[ix];
			base_ui_component* pchild;
			init_ui_component_by_mgo(pchild, cd_obj);
			ptar->add_child(pchild);
		}
	}
	/*MsgPack::array ppty{ mg_obj["property"].array_items()};
	int idx = 0;
	for (auto& pu : ppty)//so dirty!!!
	{
		MsgPack::object ptmo{ pu.object_items() };
		MsgPack::binary pt_mem{ ptmo["pt_mem"].binary_items() };
		memcpy(vplist[idx]._p_head_address, &pt_mem[0], vplist[idx]._len);
		idx++;
	}
	auto& chld_lst = mg_obj.find("childs");
	if (chld_lst != mg_obj.end())
	{
		MsgPack::array child_list{ chld_lst->second.array_items() };
		if (child_list.size() > 0)
		{
			for (auto& chld : child_list)
			{
				MsgPack::object chld_oj{ chld.object_items() };
				string cname = chld_oj["type"].string_value();
				base_ui_component* pcontrol_instance = factory::get().produce(cname);
				init_ui_component_by_mgo(*pcontrol_instance, chld_oj);
				tar.add_child(pcontrol_instance);
			}
		}
	}
	*/
}

void afb_load::load_afb(const char* afb_file)
{
	ifstream fin;
	fin.open(afb_file, ios::binary );
	if (!fin.is_open())
	{
		printf("invalid afb file:%s\n", afb_file);
	}
	auto file_size=fin.tellg();
	fin.seekg(0, ios::end);
	file_size = fin.tellg() - file_size;
	fin.seekg(0, ios::beg);
	msgpack::unpacker unpac;
	unpac.reserve_buffer(file_size);
	fin.read(unpac.buffer(), unpac.buffer_capacity());
	unpac.buffer_consumed(static_cast<size_t>(fin.gcount()));
	msgpack::object_handle oh;
	unpac.next(oh);
	auto obj_w = oh.get();
	base_ui_component::screenw = obj_w.via.array.ptr[0].as<float>();
	base_ui_component::screenh = obj_w.via.array.ptr[1].as<float>();
	auto obj_res = obj_w.via.array.ptr[2];
	auto re_cnt = obj_res.via.array.size;
	for (size_t ix = 0; ix < re_cnt; ix++)
	{
		g_vres_texture_list.emplace_back(res_texture_list());
		int cur_pos = g_vres_texture_list.size() - 1;
		res_texture_list& res_unit = g_vres_texture_list[cur_pos];
		glGenTextures(1, &res_unit.texture_id);
		glBindTexture(GL_TEXTURE_2D, res_unit.texture_id);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Step3 设定filter参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		auto bin_res_unit = obj_res.via.array.ptr[ix];
		res_unit.texture_width = bin_res_unit.via.array.ptr[0].as<int>();
		res_unit.texture_height = bin_res_unit.via.array.ptr[1].as<int>();
		auto res_bin = bin_res_unit.via.array.ptr[2];
		auto bin_sz = res_bin.via.bin.size;
		//uint8_t* ptxt = new uint8_t[bin_sz];
		//memcpy(ptxt, res_bin.via.bin.ptr, bin_sz);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res_unit.texture_width, res_unit.texture_height,
			0, GL_RGBA, GL_UNSIGNED_BYTE, res_bin.via.bin.ptr);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		auto res_data = bin_res_unit.via.array.ptr[3];
		auto res_data_sz = res_data.via.array.size;
		for (size_t iy = 0; iy < res_data_sz; iy++)
		{
			res_unit.vtexture_coordinates.emplace_back(res_texture_coordinate());
			int curpos = res_unit.vtexture_coordinates.size() - 1;
			res_texture_coordinate& cd_unit = res_unit.vtexture_coordinates[curpos];
			auto bin_cd_unit = res_data.via.array.ptr[iy];
			auto bin_filen = bin_cd_unit.via.array.ptr[0];
			//char str_name[50] = {0};
			cd_unit._file_name.reserve(bin_filen.via.str.size+1);
			cd_unit._file_name.resize(bin_filen.via.str.size+1);
			memcpy(&cd_unit._file_name[0], bin_filen.via.str.ptr, bin_filen.via.str.size);
			cd_unit._x0 = bin_cd_unit.via.array.ptr[1].as<float>();
			cd_unit._x1 = bin_cd_unit.via.array.ptr[2].as<float>();
			cd_unit._y0 = bin_cd_unit.via.array.ptr[3].as<float>();
			cd_unit._y1 = bin_cd_unit.via.array.ptr[4].as<float>();
		}

	}
	auto obj_ui = obj_w.via.array.ptr[3];
	init_ui_component_by_mgo(_pj, obj_ui);
	    

	/*while (unpac.next(oh))
	{
		auto sw = oh.get();
		std::cout << type_str[sw.type] << ":" << sw << std::endl;
	}
	*/
	/*string err;
	MsgPack ui_obj = MsgPack::parse(outbuff, err);
	MsgPack::object root{ ui_obj.object_items() };
	base_ui_component::screenw = root["screenw"].float32_value();
	base_ui_component::screenh = root["screenh"].float32_value();
	g_cur_texture_id_index = root["current_txt_index"].int32_value();
	//MsgPack::array res_list{ root["txtres_list"].array_items() };
	for (auto& mg_resunit : root["txtres_list"].array_items())
	{
		g_vres_texture_list.emplace_back(res_texture_list());
		int cur_pos = g_vres_texture_list.size() - 1;
		res_texture_list& res_unit = g_vres_texture_list[cur_pos];
		glGenTextures(1, &res_unit.texture_id);
		glBindTexture(GL_TEXTURE_2D, res_unit.texture_id);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Step3 设定filter参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		MsgPack::object mg_res_obj{ mg_resunit.object_items() };
		res_unit.texture_width = mg_res_obj["txtpk_w"].int32_value();
		res_unit.texture_height = mg_res_obj["txtpk_h"].int32_value();
		MsgPack::binary imgdata{ mg_res_obj["txtpk_file"].binary_items() };
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res_unit.texture_width, res_unit.texture_height,
			0, GL_RGBA, GL_UNSIGNED_BYTE, &imgdata[0]);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		MsgPack::array  txt_coord_data{ mg_res_obj["txtpk_fmt"].array_items() };
		vres_txt_cd& txt_cd = res_unit.vtexture_coordinates;
		for (auto& cood_unit : txt_coord_data)
		{
			txt_cd.emplace_back(res_texture_coordinate());
			int c_pos = txt_cd.size() - 1;
			res_texture_coordinate& cd_ut = txt_cd[c_pos];
			MsgPack::object mgcd{ cood_unit.object_items() };
			cd_ut._file_name = mgcd["file_name"].string_value();
			cd_ut._x0= mgcd["x0"].float32_value();
			cd_ut._x1= mgcd["x1"].float32_value();
			cd_ut._y0= mgcd["y0"].float32_value();
			cd_ut._y1= mgcd["y1"].float32_value();
		}
	}
	MsgPack::object control_objects{ root["control_objects"].object_items() };
	init_ui_component_by_mgo(_pj, control_objects);
	*/
	
}