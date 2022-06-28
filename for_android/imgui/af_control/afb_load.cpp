#include "afb_load.h"
#include <msgpack.hpp>
#include "af_factory.h"
#include "res_output.h"
#include "af_font_res_set.h"
#include <fstream>
#include "imgui.h"
#if defined(INCLUDE_ES3)
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include <GL/gl3w.h>
#endif
#include "af_shader.h"
#include "af_material.h"
#include "af_primitive_object.h"
#include "af_model.h"
#include "af_bind.h"
#include "af_state_manager.h"
#include "af_feedback.h"
#include "af_playlist_group.h"
//#define DXT5_DECOMPRESSED
#ifdef DXT5_DECOMPRESSED
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif 
#include "afb_res_index.h"


void afb_load::init_ui_component_by_mgo(base_ui_component*&ptar, msgpack::v2::object& obj)
{
	auto obj_arr_sz = obj.via.array.size;
	auto obj_typename = obj.via.array.ptr[0];
	auto obj_tp_nmlen = obj_typename.via.str.size;
	char* str_type_name = new char[obj_tp_nmlen+1 ];
	memset(str_type_name, 0, obj_tp_nmlen+1 );
	memcpy(str_type_name, obj_typename.via.str.ptr, obj_tp_nmlen);
	str_type_name[obj_tp_nmlen]='\0';
	//printf("a control type:%s\n",str_type_name);
	ptar = factory::get().produce(str_type_name);
	vproperty_list vplist;
	int mlen=ptar->collect_property_range(vplist);
	auto mem_obj=obj.via.array.ptr[1];
	auto bin_sz = mem_obj.via.bin.size;
	uint8_t* pmem = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(mem_obj.via.bin.ptr));
	uint8_t* pmem_bk = pmem;
	for (auto& munit : vplist)
	{
		memcpy(munit._p_head_address, pmem, munit._len);
		pmem += munit._len;
	}
	int bksz = pmem - pmem_bk;
	//cout << "peroperty_sizeof " << str_type_name << " is " << bin_sz << "bksz is"<<bksz<< endl;
	ptar->link();
	delete[] str_type_name;
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
	ptar->clear_rebundent_memory();
}
extern GLuint       g_FontTexture;
#ifdef _DEBUG
#include <chrono>
#define _CHECK_TIME_CONSUME
#endif
void afb_load::load_afb(const char* afb_file)
{
#ifdef _CHECK_TIME_CONSUME
	auto lastTime = std::chrono::high_resolution_clock::now();
	auto currentTime = lastTime;
	int delta = 0;
#define _STR(x) #x
#define STR(x) _STR(x)
#define TIME_CHECK(x) 	currentTime = std::chrono::high_resolution_clock::now();\
	delta = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(currentTime - lastTime).count();\
		LOGI(STR(x)" consume %d milli secs\n", delta);lastTime = currentTime;
	
#else
#define TIME_CHECK(x)
#endif 
	ifstream fin;
	fin.open(afb_file, ios::binary );
	if (!fin.is_open())
	{
		LOGE("invalid afb file:%s\n", afb_file);
	}
	auto file_size=fin.tellg();
	fin.seekg(0, ios::end);
	file_size = fin.tellg() - file_size;
	fin.seekg(0, ios::beg);
	msgpack::unpacker unpac;
	unpac.reserve_buffer(file_size);
	fin.read(unpac.buffer(), unpac.buffer_capacity());
	unpac.buffer_consumed(static_cast<size_t>(fin.gcount()));
	TIME_CHECK(reading afb)
	string cur_proj = afb_file;
	string cur_dir = cur_proj.substr(0, cur_proj.find_last_of('\\') + 1);
	msgpack::object_handle oh;
	unpac.next(oh);
	auto obj_w = oh.get();
	base_ui_component::screenw = obj_w.via.array.ptr[en_screen_w].as<float>();
	base_ui_component::screenh = obj_w.via.array.ptr[en_screen_h].as<float>();


	auto font_res = obj_w.via.array.ptr[en_font_faces];
	auto font_cnt = font_res.via.array.size;
	g_pfont_face_manager = make_shared<font_face_manager>();
	for (size_t ix = 0; ix < font_cnt; ix++)
	{
		auto font_unit = font_res.via.array.ptr[ix];
		auto font_name = font_unit.via.array.ptr[0];
		auto fnm_sz=font_name.via.str.size;
		string str_font_name;
		str_font_name.resize(fnm_sz);
		memcpy(&str_font_name[0], font_name.via.str.ptr, fnm_sz);
		auto fcols = font_unit.via.array.ptr[1].as<int>();
		auto frows= font_unit.via.array.ptr[2].as<int>();
		auto font_data = font_unit.via.array.ptr[3];
		auto data_sz = font_data.via.bin.size;
		uint8_t* font_data_buff = new uint8_t[data_sz];
		memcpy(font_data_buff, font_data.via.bin.ptr, data_sz);
		auto ft_unit= g_pfont_face_manager->load_font(str_font_name, font_data_buff, data_sz);
		ft_unit->_char_count_c = fcols;
		ft_unit->_char_count_r = frows;
	}
	TIME_CHECK(Font faces)
	auto obj_format = obj_w.via.array.ptr[en_output_bin_format];
	g_output_bin_format._txt_fmt = static_cast<texture_format>(obj_format.via.array.ptr[0].as<int>());
	g_output_bin_format._pgm_fmt = static_cast<program_format>(obj_format.via.array.ptr[1].as<int>());
	auto obj_res = obj_w.via.array.ptr[en_vtextures_res];
	auto re_cnt = obj_res.via.array.size;
	function<unsigned int(const char*, int, int, unsigned int,bool mipv)> f_gen_txt;
	if (g_output_bin_format._txt_fmt == en_uncompressed_txt){
		f_gen_txt = [](const char* ptxt_data, int iw, int ih, unsigned int bin_sz,bool mipv){
			GLuint txt_id;
			glGenTextures(1, &txt_id);
			glBindTexture(GL_TEXTURE_2D, txt_id);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// Step3 �趨filter����
			/*GLfloat largest_supported_anisotropy; 
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy); 
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);*/
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if (mipv)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // ΪMipMap�趨filter����
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iw, ih,
					0, GL_RGBA, GL_UNSIGNED_BYTE, ptxt_data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iw, ih,
					0, GL_RGBA, GL_UNSIGNED_BYTE, ptxt_data);
			}
			//glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			return txt_id;
		};
	}
	else if (g_output_bin_format._txt_fmt == en_dxt5){
		f_gen_txt = [](const char* ptxt_data, int iw, int ih, unsigned int bin_sz,bool mipv){
			GLuint txt_id;
			glGenTextures(1, &txt_id);
			//printf("gen txtid:%u\n", txt_id);
			glBindTexture(GL_TEXTURE_2D, txt_id);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// Step3 �趨filter����
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if (mipv)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // ΪMipMap�趨filter����
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, iw, ih, 0, bin_sz, ptxt_data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, iw, ih, 0, bin_sz, ptxt_data);
			}

			//glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			return txt_id;
		};
	}
	for (size_t ix = 0; ix < re_cnt; ix++)
	{
          g_vres_texture_list.emplace_back( make_shared<res_texture_list>() );
		res_texture_list& res_unit = *g_vres_texture_list.back();
		auto bin_res_unit = obj_res.via.array.ptr[ix];
		auto txt_name = bin_res_unit.via.array.ptr[0];
		auto txt_name_sz = txt_name.via.str.size;
		char* txt_kname = new char[txt_name_sz + 1];
		memcpy(txt_kname, txt_name.via.str.ptr, txt_name_sz);
		txt_kname[txt_name_sz] = 0;
		//res_unit.texture_pack_file = txt_kname;
		res_unit.texture_width = bin_res_unit.via.array.ptr[1].as<int>();
		res_unit.texture_height = bin_res_unit.via.array.ptr[2].as<int>();
		auto res_bin = bin_res_unit.via.array.ptr[3];
		if (res_bin.type == msgpack::type::POSITIVE_INTEGER)
		{
			res_unit._is_separated=true;
			res_unit.txt_id = f_gen_txt(0, res_unit.texture_width, res_unit.texture_height, 0,false);
			string res_file = cur_dir + txt_kname;
			res_file += ".safb";
			thread th_lod_res([&](string fnm){
				ifstream ifs;
				ifs.open(fnm, ios::binary);
				auto res_size = ifs.tellg();
				ifs.seekg(0, ios::end);
				res_size = ifs.tellg() - res_size;
				ifs.seekg(0, ios::beg);
				string& rd_buf = res_unit.txt_buff;
				rd_buf.resize(res_size);
				ifs.read(&rd_buf[0], res_size);
				ifs.close();
				res_unit._loaded = true;
			}, res_file);
			th_lod_res.detach();
		}
		else
		{
			auto bin_sz = res_bin.via.bin.size;
			res_unit.txt_id = f_gen_txt(res_bin.via.bin.ptr, res_unit.texture_width, res_unit.texture_height, bin_sz,false);
		}
		delete[] txt_kname;
		auto res_data = bin_res_unit.via.array.ptr[4];
		auto res_data_sz = res_data.via.array.size;
		for (size_t iy = 0; iy < res_data_sz; iy++)
		{
			res_unit.vtexture_coordinates.emplace_back(res_texture_coordinate());
			res_texture_coordinate& cd_unit = res_unit.vtexture_coordinates.back();
			auto bin_cd_unit = res_data.via.array.ptr[iy];
			auto bin_filen = bin_cd_unit.via.array.ptr[0];
			cd_unit._file_name.reserve(bin_filen.via.str.size + 1);
			cd_unit._file_name.resize(bin_filen.via.str.size + 1);
			memcpy(&cd_unit._file_name[0], bin_filen.via.str.ptr, bin_filen.via.str.size);
			cd_unit._x0 = bin_cd_unit.via.array.ptr[1].as<float>();
			cd_unit._x1 = bin_cd_unit.via.array.ptr[2].as<float>();
			cd_unit._y0 = bin_cd_unit.via.array.ptr[3].as<float>();
			cd_unit._y1 = bin_cd_unit.via.array.ptr[4].as<float>();
		}

	}

	TIME_CHECK(globe texture res)
	auto obj_txt_list = obj_w.via.array.ptr[en_mtextures_res];
	auto txt_cnt = obj_txt_list.via.array.size;
	for (size_t ix = 0; ix < txt_cnt;ix++)
	{
		auto txt_unit = obj_txt_list.via.array.ptr[ix];
		auto txt_name = txt_unit.via.array.ptr[0];
		auto txt_width = txt_unit.via.array.ptr[1].as<uint32_t>();
		auto txt_height = txt_unit.via.array.ptr[2].as<uint32_t>();
		auto txt_bin = txt_unit.via.array.ptr[3];
		auto txt_mip = txt_unit.via.array.ptr[4];
		bool mipv = txt_mip.as<uint8_t>()>0;
		auto txt_name_sz = txt_name.via.str.size;
		char* txt_kname = new char[txt_name_sz+1];
		memset(txt_kname, 0, txt_name_sz + 1);
		memcpy(txt_kname, txt_name.via.str.ptr, txt_name_sz);
		g_mtexture_list[txt_kname] = make_shared<af_texture>();
		auto& a_txt = g_mtexture_list[txt_kname];
		a_txt->_width = txt_width;
		a_txt->_height = txt_height;
		a_txt->_mip_map = mipv;
		if (txt_bin.type == msgpack::type::POSITIVE_INTEGER)
		{
			a_txt->_is_separated=true;
			a_txt->_atxt_id = f_gen_txt(0, a_txt->_width, a_txt->_height, 0, mipv);

			string res_file = cur_dir + txt_kname;
			res_file += ".safb";
			thread th_lod_res([&](string fnm){
				ifstream ifs;
				ifs.open(fnm, ios::binary);
				//printf("res name:%s\n", fnm.c_str());
				auto res_size = ifs.tellg();
				ifs.seekg(0, ios::end);
				res_size = ifs.tellg() - res_size;
				ifs.seekg(0, ios::beg);
				string& rd_buf = a_txt->txt_buff;
				rd_buf.resize(res_size);
				ifs.read(&rd_buf[0], res_size);
				ifs.close();
				a_txt->_loaded = true;
			},res_file);
			th_lod_res.detach();
		}
		else
		{
			auto txt_bin_sz = txt_bin.via.bin.size;
			a_txt->_atxt_id = f_gen_txt(txt_bin.via.bin.ptr, txt_width, txt_height, txt_bin_sz, mipv);
			a_txt->_loaded = true;
		}
	
		delete[] txt_kname;

	}
	TIME_CHECK(texture list res)
	auto obj_file_list = obj_w.via.array.ptr[en_mfiles_res];
	auto file_cnt = obj_file_list.via.array.size;
	for (size_t ix = 0; ix < file_cnt;ix++)
	{
		auto file_unit = obj_file_list.via.array.ptr[ix];
		auto file_name = file_unit.via.array.ptr[0];
		auto file_bin = file_unit.via.array.ptr[1];
		auto file_name_size = file_name.via.str.size;
		char* file_kname = new char[file_name_size + 1];
		memset(file_kname, 0, file_name_size + 1);
		memcpy(file_kname, file_name.via.str.ptr, file_name_size);
		auto a_file = make_shared<af_file>(file_bin.via.bin.size);
		memcpy(a_file->_pbin, file_bin.via.bin.ptr, a_file->_fsize);
		g_mfiles_list[file_kname] = a_file;
		delete[] file_kname;
	}

	TIME_CHECK(file list res)
	auto obj_shader_list = obj_w.via.array.ptr[en_shaders_res];
	auto shd_cnt = obj_shader_list.via.array.size;
	if (g_output_bin_format._pgm_fmt == en_shader_code)
	{
		for (size_t ix = 0; ix < shd_cnt; ix++)
		{
			auto shd_unit = obj_shader_list.via.array.ptr[ix];
			auto shd_name = shd_unit.via.array.ptr[0];
			auto str_shd_nm_sz = shd_name.via.str.size;
			char* pshd_name = new char[str_shd_nm_sz + 1];
			memset(pshd_name, 0, str_shd_nm_sz + 1);
			memcpy(pshd_name, shd_name.via.str.ptr, str_shd_nm_sz);
			auto shd_vs_code = shd_unit.via.array.ptr[1];
			auto str_vs_code_sz = shd_vs_code.via.str.size;
			char* psd_vs_code = new char[str_vs_code_sz + 1];
			memset(psd_vs_code, 0, str_vs_code_sz + 1);
			memcpy(psd_vs_code, shd_vs_code.via.str.ptr, str_vs_code_sz);
			auto shd_fs_code = shd_unit.via.array.ptr[2];
			auto str_fs_code_sz = shd_fs_code.via.str.size;
			char* psd_fs_code = new char[str_fs_code_sz + 1];
			memset(psd_fs_code, 0, str_fs_code_sz + 1);
			memcpy(psd_fs_code, shd_fs_code.via.str.ptr, str_fs_code_sz);
			g_af_shader_list[pshd_name] = make_shared<af_shader>(psd_vs_code, psd_fs_code);
			delete[] pshd_name;
			delete[] psd_vs_code;
			delete[] psd_fs_code;
		}
	}
	else if (g_output_bin_format._pgm_fmt == en_shader_bin_general)
	{
		for (size_t ix = 0; ix < shd_cnt; ix++)
		{
			auto shd_unit = obj_shader_list.via.array.ptr[ix];
			auto shd_name = shd_unit.via.array.ptr[0];
			auto str_shd_nm_sz = shd_name.via.str.size;
			char* pshd_name = new char[str_shd_nm_sz + 1];
			memset(pshd_name, 0, str_shd_nm_sz + 1);
			memcpy(pshd_name, shd_name.via.str.ptr, str_shd_nm_sz);
			GLenum binaryFormat = shd_unit.via.array.ptr[1].as<int>();
			auto binCode = shd_unit.via.array.ptr[2];
			auto binCodeSize = binCode.via.bin.size;
			auto pbin = binCode.via.bin.ptr;
			g_af_shader_list[pshd_name] = make_shared<af_shader>(binaryFormat, (void*)pbin, binCodeSize);
			delete[] pshd_name;
		}
	}
	TIME_CHECK(shader list res)
	auto obj_material_list = obj_w.via.array.ptr[en_materials_res];
	auto mtl_cnt = obj_material_list.via.array.size;
	for (size_t ix = 0; ix < mtl_cnt; ix++)
	{
		auto mtl_unit = obj_material_list.via.array.ptr[ix];
		auto mtl_name = mtl_unit.via.array.ptr[0];
		auto mtl_name_size = mtl_name.via.str.size;
		char* pstr_mtl_name = new char[mtl_name_size + 1];
		memset(pstr_mtl_name, 0, mtl_name_size + 1);
		memcpy(pstr_mtl_name, mtl_name.via.str.ptr, mtl_name_size);
		shared_ptr<material> pmtl = make_shared<material>();
		g_material_list[pstr_mtl_name] = pmtl;
		auto shd_name = mtl_unit.via.array.ptr[1];
		auto shd_name_size = shd_name.via.str.size;
		char* pstr_shd_name = new char[shd_name_size + 1];
		memset(pstr_shd_name, 0, shd_name_size+1);
		memcpy(pstr_shd_name, shd_name.via.str.ptr, shd_name_size);
		auto pshd = g_af_shader_list.find(pstr_shd_name);
		if (pshd != g_af_shader_list.end())
		{
			pmtl->set_pshader(pshd->second);
		}
		auto& mpshd_uf = pmtl->get_mp_sd_uf();
		auto shd_uf_list = mtl_unit.via.array.ptr[2];
		auto shd_uf_list_cnt = shd_uf_list.via.array.size;
		for (size_t iy = 0; iy < shd_uf_list_cnt; iy++)
		{
			auto shd_uf_unit = shd_uf_list.via.array.ptr[iy];
			auto shd_uf_name = shd_uf_unit.via.array.ptr[0];
			auto shd_uf_name_sz = shd_uf_name.via.str.size;
			char* pshd_uf_name = new char[shd_uf_name_sz + 1];
			memset(pshd_uf_name, 0, shd_uf_name_sz + 1);
			memcpy(pshd_uf_name, shd_uf_name.via.str.ptr, shd_uf_name_sz);
			auto shd_uf_type=shd_uf_unit.via.array.ptr[1];
			auto shd_uf_type_sz = shd_uf_type.via.str.size;
			char* pshd_uf_type = new char[shd_uf_type_sz + 1];
			memset(pshd_uf_type, 0, shd_uf_type_sz + 1);
			memcpy(pshd_uf_type, shd_uf_type.via.str.ptr, shd_uf_type_sz);

			auto shd_uf_usize=shd_uf_unit.via.array.ptr[2];
			auto shd_uf_el_size=shd_uf_unit.via.array.ptr[3];
			auto shd_uf_utype=shd_uf_unit.via.array.ptr[4];

			shared_ptr<shader_uf> pnunf = std::move(fac_shader_uf::get().Create(pshd_uf_type, shd_uf_usize.as<GLuint>(), shd_uf_el_size.as<GLuint>()));
			pnunf->set_type(shd_uf_utype.as<GLenum>());
			auto shd_data = shd_uf_unit.via.array.ptr[5];
			auto wsize = shd_data.via.bin.size;// shd_uf_usize.as<GLuint>()*shd_uf_el_size.as<GLuint>();
			//printf("wsize=%d\n",wsize);
			
			memcpy(pnunf->get_data_head(), shd_data.via.bin.ptr,wsize);
			pnunf->link();
			mpshd_uf[pshd_uf_name] = pnunf;
			delete[] pshd_uf_name;
			delete[] pshd_uf_type;
		}
		delete[] pstr_mtl_name;
	}
	TIME_CHECK(materil list res);
	auto obj_primitives = obj_w.via.array.ptr[en_primitives_res];
	auto obj_prm_size = obj_primitives.via.array.size;
	for (size_t ix = 0; ix < obj_prm_size;++ix)
	{
		auto prm_unit = obj_primitives.via.array.ptr[ix];
		auto prm_name = prm_unit.via.array.ptr[0];
		auto p_vt_bf_len = prm_unit.via.array.ptr[1];
		auto pebo_len = prm_unit.via.array.ptr[2];
		auto pformat=prm_unit.via.array.ptr[3];
		auto prm_name_sz = prm_name.via.str.size;
		string key_name;
		key_name.resize(prm_name_sz);
		memcpy(&key_name[0], prm_name.via.str.ptr, prm_name_sz);
		//key_name[prm_name_sz] = 0;
		auto p_obj = make_shared<primitive_object>();
		auto ebo_len = pebo_len.as<GLuint>();
		auto vbo_len = p_vt_bf_len.as<GLuint>();
		GLfloat* pvbo = 0;
		GLuint* pebo = 0;
		auto ifile = g_mfiles_list.find(key_name);
		if (ifile!=g_mfiles_list.end())
		{
			auto& fobj = *ifile->second;
			char* phead = (char*)fobj._pbin;
			GLuint* phead_len = (GLuint*)phead;
			phead += 4;
			pvbo = (GLfloat*)phead;
			if (ebo_len)
			{
				pebo = (GLuint*)(phead + *phead_len);
			}
		}
		
		vector<GLubyte> ele_fmt;
		auto fmt_sz = pformat.via.bin.size;
		ele_fmt.resize(fmt_sz);
		memcpy(&ele_fmt[0], pformat.via.bin.ptr, fmt_sz);
		p_obj->set_ele_format(ele_fmt);
		p_obj->load_vertex_data(pvbo, vbo_len, pebo, ebo_len);
		p_obj->_ps_file = ifile->second;
		g_primitive_list[key_name] = p_obj;
	}
	auto obj_models = obj_w.via.array.ptr[en_models_res];
	auto obj_modles_sz = obj_models.via.array.size;
	for (size_t ix = 0; ix < obj_modles_sz;++ix)
	{
		auto obj_md_unit = obj_models.via.array.ptr[ix];
		auto md_name = obj_md_unit.via.array.ptr[0];
		auto obj_msh_lst=obj_md_unit.via.array.ptr[1];
		string md_key;
		auto md_name_sz = md_name.via.str.size;
		md_key.resize(md_name_sz);
		memcpy(&md_key[0], md_name.via.str.ptr, md_name_sz);
		//md_key[md_name_sz] = 0;
		g_mmodel_list[md_key] = make_shared<af_model>();
		auto& mdl = *g_mmodel_list[md_key];
		auto obj_msh_lst_sz = obj_msh_lst.via.array.size;
		for (size_t iy = 0; iy < obj_msh_lst_sz; iy++)
		{
			mdl.emplace_back();
			af_mesh& mesh_unit = mdl[iy];
			auto obj_mesh_unit = obj_msh_lst.via.array.ptr[iy];
			auto obj_prmid = obj_mesh_unit.via.array.ptr[0];
			auto obj_diffs = obj_mesh_unit.via.array.ptr[1];
			auto obj_speculars = obj_mesh_unit.via.array.ptr[2];
			auto obj_heights = obj_mesh_unit.via.array.ptr[3];
			auto obj_ambients = obj_mesh_unit.via.array.ptr[4];
			string& prim_id = mesh_unit._prm_id;
			auto prim_id_sz = obj_prmid.via.str.size;
			prim_id.resize(prim_id_sz);
			memcpy(&prim_id[0], obj_prmid.via.str.ptr, prim_id_sz);
			const auto& iprim = g_primitive_list.find(prim_id);
			assert(iprim != g_primitive_list.end() && "prim id is missed?");
			mesh_unit._ps_prm_id = iprim->second;
			auto obj_diffs_sz = obj_diffs.via.array.size;
			for (size_t id = 0; id < obj_diffs_sz;++id)
			{
				auto obj_diff = obj_diffs.via.array.ptr[id];
				auto obj_diff_sz = obj_diff.via.str.size;
				string diff;
				diff.resize(obj_diff_sz);
				memcpy(&diff[0], obj_diff.via.str.ptr, obj_diff_sz);
				mesh_unit._text_diffuse_list.emplace_back(diff);
				const auto& idiff = g_mtexture_list.find(diff);
				if (idiff!=g_mtexture_list.end())
				{
					mesh_unit._ps_text_diffuse_list.emplace_back(idiff->second);
				}
			}
			auto obj_speculars_sz = obj_speculars.via.array.size;
			for (size_t id = 0; id < obj_speculars_sz; ++id)
			{
				auto obj_specular = obj_speculars.via.array.ptr[id];
				auto obj_specular_sz = obj_specular.via.str.size;
				string specular;
				specular.resize(obj_specular_sz);
				memcpy(&specular[0], obj_specular.via.str.ptr, obj_specular_sz);
				mesh_unit._text_specular_list.emplace_back(specular);
				const auto& ispec = g_mtexture_list.find(specular);
				if (ispec!=g_mtexture_list.end())
				{
					mesh_unit._ps_text_specular_list.emplace_back(ispec->second);
				}
			}
			auto obj_heights_sz = obj_heights.via.array.size;
			for (size_t id = 0; id < obj_heights_sz; ++id)
			{
				auto obj_height = obj_heights.via.array.ptr[id];
				auto obj_height_sz = obj_height.via.str.size;
				string height;
				height.resize(obj_height_sz);
				memcpy(&height[0], obj_height.via.str.ptr, obj_height_sz);
				mesh_unit._text_height_list.emplace_back(height);
				const auto& iheight = g_mtexture_list.find(height);
				if (iheight!=g_mtexture_list.end())
				{
					mesh_unit._ps_text_height_list.emplace_back(iheight->second);
				}
			}
			auto obj_ambients_sz = obj_ambients.via.array.size;
			for (size_t id = 0; id < obj_ambients_sz; ++id)
			{
				auto obj_ambient = obj_ambients.via.array.ptr[id];
				auto obj_ambient_sz = obj_ambient.via.str.size;
				string ambient;
				ambient.resize(obj_ambient_sz);
				memcpy(&ambient[0], obj_ambient.via.str.ptr, obj_ambient_sz);
				mesh_unit._text_ambient_list.emplace_back(ambient);
				const auto& iamb = g_mtexture_list.find(ambient);
				if (iamb!=g_mtexture_list.end())
				{
					mesh_unit._ps_text_ambient_list.emplace_back(iamb->second);
				}
			}
		}
	}
	auto obj_ui = obj_w.via.array.ptr[en_control_res];
	init_ui_component_by_mgo(_pj, obj_ui);
	TIME_CHECK(control list res)
	auto obj_2_prp_pos = [this](msgpack::v2::object& okey,prop_ele_position&prp_epos){
		auto obin_sz = okey.via.bin.size;
		auto rsz = obin_sz / sizeof(unsigned short);
		prop_ele_pos_index prp_ep_idx;
		prp_ep_idx.resize(rsz);
		memcpy(&prp_ep_idx[0], okey.via.bin.ptr, obin_sz);
		int ii = rsz - 1;
		base_ui_component* pcontrol = _pj;
		for (; ii > 1;ii--)
		{
			pcontrol = pcontrol->get_child(prp_ep_idx[ii]);
		}
		prp_epos._pobj = pcontrol;
		prp_epos._page_index = prp_ep_idx[1];
		prp_epos._field_index = prp_ep_idx[0];
	};	
	auto obj_valiase = obj_w.via.array.ptr[en_aliase_dic];
	auto obj_aliase_sz = obj_valiase.via.array.size;
	for (size_t ix = 0; ix < obj_aliase_sz;ix++)
	{
		auto obj_aliase = obj_valiase.via.array.ptr[ix];
		auto obj_key = obj_aliase.via.array.ptr[0];
		auto obj_key_sz = obj_key.via.str.size;
		string aliase_key;
		aliase_key.resize(obj_key_sz);
		memcpy(&aliase_key[0], obj_key.via.str.ptr, obj_key_sz);
		auto obj_pep_pos = obj_aliase.via.array.ptr[1];
		auto ps_pep_pos = make_shared<prop_ele_position>();
		obj_2_prp_pos(obj_pep_pos, *ps_pep_pos);
		g_aliase_dic[aliase_key] = ps_pep_pos;
	}
	TIME_CHECK(aliase list res)
	auto obj_bind_dic = obj_w.via.array.ptr[en_bind_dic];
	auto obj_bind_dic_sz = obj_bind_dic.via.array.size;
	for (size_t ix = 0; ix < obj_bind_dic_sz;ix++)
	{
		auto oprp_ele = obj_bind_dic.via.array.ptr[ix];
		auto obind_key = oprp_ele.via.array.ptr[0];
		prop_ele_position prp_ele_pos;
		obj_2_prp_pos(obind_key, prp_ele_pos);
		auto ps_bind_unit = make_shared<prop_ele_bind_unit>();
		auto& vparam = ps_bind_unit->_param_list;
		auto opm_list = oprp_ele.via.array.ptr[1];
		auto opm_list_sz = opm_list.via.array.size;
		for (size_t iix = 0; iix < opm_list_sz;iix++)
		{
			auto opele_pos = opm_list.via.array.ptr[iix];
			vparam.emplace_back();	
			auto& sub_prp_ele_pos = vparam.back(); //vparam[iix];
			obj_2_prp_pos(opele_pos, sub_prp_ele_pos);
		}
		auto oexpr = oprp_ele.via.array.ptr[2];
		auto& expr = ps_bind_unit->_expression;
		auto exp_sz = oexpr.via.str.size;
		expr.resize(exp_sz);
		memcpy(&expr[0], oexpr.via.str.ptr, exp_sz);
		g_bind_dic[prp_ele_pos] = ps_bind_unit;
	}
	auto obj_bind_ref_dic = obj_w.via.array.ptr[en_bind_ref_dic];
	auto bind_ref_dic_sz = obj_bind_ref_dic.via.array.size;
	for (size_t ix = 0; ix < bind_ref_dic_sz;ix++)
	{
		auto oprp_ele = obj_bind_ref_dic.via.array.ptr[ix];
		auto obind_key = oprp_ele.via.array.ptr[0];
		prop_ele_position prp_ele_pos;
		obj_2_prp_pos(obind_key, prp_ele_pos);
		auto ps_ref_list = make_shared<vprop_pos>();
		auto oref_list=oprp_ele.via.array.ptr[1];
		auto oref_list_sz = oref_list.via.array.size;
		for (size_t iix = 0; iix < oref_list_sz;iix++)
		{
			auto oref = oref_list.via.array.ptr[iix];
			ps_ref_list->emplace_back();
			auto& sub_prp_pos = (*ps_ref_list)[iix];
			obj_2_prp_pos(oref, sub_prp_pos);
		}
		g_bind_ref_dic[prp_ele_pos] = ps_ref_list;
	}
	TIME_CHECK(bind list res)
	auto obj_state_manger = obj_w.via.array.ptr[en_state_manager];
	auto stm_sz = obj_state_manger.via.array.size;
	for (size_t ix = 0; ix < stm_sz;ix++)
	{
		auto omstm = obj_state_manger.via.array.ptr[ix];
		auto omkey = omstm.via.array.ptr[0];
		auto mkey_sz = omkey.via.str.size;
		string mskey;
		mskey.resize(mkey_sz);
		memcpy(&mskey[0], omkey.via.str.ptr, mkey_sz);
		auto ps_stm = make_shared<af_state_manager>();
		auto& stm = *ps_stm;
		auto& prop_list = stm._prop_list;
		auto oprop_list = omstm.via.array.ptr[1];
		auto oprp_lst_sz = oprop_list.via.array.size;
		for (size_t ii = 0; ii < oprp_lst_sz;ii++)
		{
			auto oprp_id = oprop_list.via.array.ptr[ii];
			prop_list.emplace_back();
			auto& prp_ele_pos = prop_list[ii];
			obj_2_prp_pos(oprp_id, prp_ele_pos);
		}
		auto oprp_value_list = omstm.via.array.ptr[2];
		auto oprp_value_list_sz = oprp_value_list.via.array.size;
		auto& prp_value_list = stm._prop_value_list;
		for (size_t ii = 0; ii < oprp_value_list_sz;ii++)
		{
			auto ovprp_value = oprp_value_list.via.array.ptr[ii];
			prp_value_list.emplace_back();
			auto& prp_value = prp_value_list[ii];
			auto ovprp_value_sz = ovprp_value.via.array.size;
			for (size_t jj = 0; jj< ovprp_value_sz;jj++)
			{
				auto ovalue_bock = ovprp_value.via.array.ptr[jj];
				auto valuesz = ovalue_bock.via.bin.size;
				prp_value.emplace_back();
				auto& value_bk = prp_value[jj];
				value_bk.resize(valuesz);
				memcpy(&value_bk[0], ovalue_bock.via.bin.ptr, valuesz);
			}
		}
		auto oany= omstm.via.array.ptr[3];
		auto& any_to_any = stm._any_to_any;
		any_to_any._start_time = oany.via.array.ptr[0].as<int>();
		any_to_any._duration=oany.via.array.ptr[1].as<int>();
		any_to_any._easing_func = oany.via.array.ptr[2].as<int>();
		stm._state_idx=omstm.via.array.ptr[4].as<unsigned char>();
		auto omtrans=omstm.via.array.ptr[5];
		auto& mtrans = stm._mtrans;
		auto mtrans_sz = omtrans.via.array.size;
		for (size_t jj = 0; jj < mtrans_sz;jj++)
		{
			auto otrans = omtrans.via.array.ptr[jj];
			auto ofrom = otrans.via.array.ptr[0];
			auto oto = otrans.via.array.ptr[1];
			trans_key tkey = { ofrom.as<int>(), oto.as<int>() };
			auto ostart_tm = otrans.via.array.ptr[2];
			auto oduration=otrans.via.array.ptr[3];
			auto oseasing_fun = otrans.via.array.ptr[4];
			auto ps_trans = make_shared<state_transition>();
			ps_trans->_start_time = ostart_tm.as<int>();
			ps_trans->_duration = oduration.as<int>();
			ps_trans->_easing_func = oseasing_fun.as<int>();
			mtrans[tkey] = ps_trans;
		}
          auto oplaylist_list = omstm.via.array.ptr[ 6 ];
          auto& playlist_list = stm._playlist_list;
          auto oplstlst_sz = oplaylist_list.via.array.size;
          for( size_t jj = 0; jj < oplstlst_sz; jj++ )
          {
               auto oplaylist = oplaylist_list.via.array.ptr[ jj ];
               playlist_list.emplace_back();
               auto& playlist = playlist_list[ jj ];
               auto plsz = oplaylist.via.array.size;
               for( size_t ix = 0; ix < plsz;ix++ )
               {
                    auto otran = oplaylist.via.array.ptr[ ix ];
                    playlist.emplace_back();
                    auto& plu = playlist[ ix ];
                    plu._from = otran.via.array.ptr[ 0 ].as<int>();
                    plu._to=otran.via.array.ptr[ 1 ].as<int>();
               }
          }
		g_mstate_manager[mskey] = ps_stm;
	}
     TIME_CHECK(staterans list res)
     auto obj_common_value_dic = obj_w.via.array.ptr[ en_common_value ];
     auto cmv_sz = obj_common_value_dic.via.array.size;
     for( size_t ix = 0; ix < cmv_sz; ix++ )
     {
          auto ocmv_u = obj_common_value_dic.via.array.ptr[ ix ];
          auto ocmv_key = ocmv_u.via.array.ptr[ 0 ];
          auto mkey_sz = ocmv_key.via.str.size;
          string mskey,scmv_tp;
          mskey.resize( mkey_sz );
          memcpy( &mskey[ 0 ], ocmv_key.via.str.ptr, mkey_sz );
          auto cmv_tp = ocmv_u.via.array.ptr[ 1 ];
          auto tp_sz = cmv_tp.via.str.size;
          scmv_tp.resize( tp_sz );
          memcpy( &scmv_tp[ 0 ], cmv_tp.via.str.ptr, tp_sz );
          auto ps_cmv = make_shared<base_prp_type>( scmv_tp );
          auto ovalue = ocmv_u.via.array.ptr[ 2 ];
          auto ov_sz = ovalue.via.bin.size;
          memcpy( ps_cmv->_pbase, ovalue.via.bin.ptr, ov_sz );
          auto& prop_list = ps_cmv->_param_list;
          auto oprop_list=ocmv_u.via.array.ptr[ 3 ];
          auto oprop_sz = oprop_list.via.array.size;
          for( size_t ii = 0; ii < oprop_sz; ii++ )
          {
               auto oprp_id = oprop_list.via.array.ptr[ ii ];
               prop_list.emplace_back();
               auto& prp_ele_pos = prop_list[ ii ];
               obj_2_prp_pos( oprp_id, prp_ele_pos );
          }
          g_base_prp_dic[ mskey ] = ps_cmv;

     }
	TIME_CHECK(commonvalue list res)
	auto obj_feedback_list = obj_w.via.array.ptr[en_feedback_list];
	auto fdsz = obj_feedback_list.via.array.size;
	for (size_t ix = 0; ix < fdsz;ix++)
	{
		 auto ofdv_u = obj_feedback_list.via.array.ptr[ix];
		 auto omtl_key = ofdv_u.via.array.ptr[0];
		 auto oprm_key = ofdv_u.via.array.ptr[1];
		 string mtl_key, prm_key;
		 auto mtl_key_sz = omtl_key.via.str.size;
		 mtl_key.resize(mtl_key_sz);
		 memcpy(&mtl_key[0], omtl_key.via.str.ptr, mtl_key_sz);
		 const auto& imtl = g_material_list.find(mtl_key);
		 if (imtl == g_material_list.end())
		 {
			 LOGE("invalid material name:%s for the current feedback\n", mtl_key.c_str());
			 continue;
		 }
		 auto prm_key_sz = oprm_key.via.str.size;
		 prm_key.resize(prm_key_sz);
		 memcpy(&prm_key[0], oprm_key.via.str.ptr, prm_key_sz);
		 const auto& iprm = g_primitive_list.find(prm_key);
		 if (iprm == g_primitive_list.end())
		 {
			 LOGE("invalid primitive name:%s for the current feedback\n", prm_key.c_str());
			 continue;
		 }
		 feedback_key fkey = { mtl_key, prm_key };
		 g_feedback_list[fkey] = make_shared<af_feedback>(imtl->second, iprm->second);
	}
	TIME_CHECK(feedback list)
	auto obj_playlist_group_list = obj_w.via.array.ptr[en_playlist_group_list];
	fdsz = obj_playlist_group_list.via.array.size;
	for (size_t ix = 0; ix < fdsz; ix++)
	{
		auto oplg = obj_playlist_group_list.via.array.ptr[ix];
		auto oplg_key = oplg.via.array.ptr[0];
		auto oplg_u = oplg.via.array.ptr[1];
		string plg_key;
		auto plg_key_sz = oplg_key.via.str.size;
		plg_key.resize(plg_key_sz);
		memcpy(&plg_key[0], oplg_key.via.str.ptr, plg_key_sz);
		auto ps_plg_list = make_shared<playlist_unit_list>();
		g_playlist_group_list[plg_key] = ps_plg_list;
		auto plg_u_sz = oplg_u.via.array.size;
		for (size_t iy = 0; iy < plg_u_sz;iy++)
		{
			 auto oplg = oplg_u.via.array.ptr[iy];
			 auto ostm = oplg.via.array.ptr[0];
			 string stm_name;
			 auto ostm_sz = ostm.via.str.size;
			 stm_name.resize(ostm_sz);
			 memcpy(&stm_name[0], ostm.via.str.ptr, ostm_sz);
			 auto oplaylist_id = oplg.via.array.ptr[1];
			 playlist_unit plu = { stm_name, oplaylist_id.as<int>() };
			 ps_plg_list->emplace_back(plu);
		}
	}
}
void afb_load::load_afb_from_buff(const char* buff,unsigned int buff_len)
{
	LOGI("buff_len:%d\n",buff_len);
	msgpack::object_handle oh=msgpack::unpack(buff,buff_len);
	auto obj_w = oh.get();
	LOGI("load afb:%d\n",__LINE__);
	base_ui_component::screenw = obj_w.via.array.ptr[en_screen_w].as<float>();
	base_ui_component::screenh = obj_w.via.array.ptr[en_screen_h].as<float>();

	LOGI("load afb:%d\n",__LINE__);
	auto font_res = obj_w.via.array.ptr[en_font_faces];
	auto font_cnt = font_res.via.array.size;
	g_pfont_face_manager = make_shared<font_face_manager>();
	for (size_t ix = 0; ix < font_cnt; ix++)
	{
		auto font_unit = font_res.via.array.ptr[ix];
		auto font_name = font_unit.via.array.ptr[0];
		auto fnm_sz=font_name.via.str.size;
		string str_font_name;
		str_font_name.resize(fnm_sz);
		memcpy(&str_font_name[0], font_name.via.str.ptr, fnm_sz);
		auto fcols = font_unit.via.array.ptr[1].as<int>();
		auto frows= font_unit.via.array.ptr[2].as<int>();
		auto font_data = font_unit.via.array.ptr[3];
		auto data_sz = font_data.via.bin.size;
		uint8_t* font_data_buff = new uint8_t[data_sz];
		memcpy(font_data_buff, font_data.via.bin.ptr, data_sz);
		auto ft_unit= g_pfont_face_manager->load_font(str_font_name, font_data_buff, data_sz);
		ft_unit->_char_count_c = fcols;
		ft_unit->_char_count_r = frows;
	}
	LOGI("load afb:%d\n",__LINE__);
	auto obj_format = obj_w.via.array.ptr[en_output_bin_format];
	g_output_bin_format._txt_fmt = static_cast<texture_format>(obj_format.via.array.ptr[0].as<int>());
	g_output_bin_format._pgm_fmt = static_cast<program_format>(obj_format.via.array.ptr[1].as<int>());
	auto res_cid = obj_w.via.array.ptr[	en_vtextures_res_cidx];
	auto obj_res = obj_w.via.array.ptr[en_vtextures_res];
	auto re_cnt = obj_res.via.array.size;
	function<unsigned int(const char*, int, int, unsigned int,bool mipv)> f_gen_txt;
	if (g_output_bin_format._txt_fmt == en_uncompressed_txt){
		f_gen_txt = [](const char* ptxt_data, int iw, int ih, unsigned int bin_sz,bool mipv){
			GLuint txt_id;
			glGenTextures(1, &txt_id);
			glBindTexture(GL_TEXTURE_2D, txt_id);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// Step3 �趨filter����
			/*GLfloat largest_supported_anisotropy; 
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy); 
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);*/
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if (mipv)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // ΪMipMap�趨filter����
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iw, ih,
					0, GL_RGBA, GL_UNSIGNED_BYTE, ptxt_data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iw, ih,
					0, GL_RGBA, GL_UNSIGNED_BYTE, ptxt_data);
			}
			//glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			return txt_id;
		};
	}
	else if (g_output_bin_format._txt_fmt == en_dxt5){
		f_gen_txt = [](const char* ptxt_data, int iw, int ih, unsigned int bin_sz,bool mipv){
			GLuint txt_id;
			glGenTextures(1, &txt_id);
			//printf("gen txtid:%u\n", txt_id);
			glBindTexture(GL_TEXTURE_2D, txt_id);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// Step3 �趨filter����
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if (mipv)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // ΪMipMap�趨filter����
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, iw, ih, 0, bin_sz, ptxt_data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, iw, ih, 0, bin_sz, ptxt_data);
			}

			//glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			return txt_id;
		};
	}
	LOGI("load afb:%d\n",__LINE__);
	for (size_t ix = 0; ix < re_cnt; ix++)
	{
          g_vres_texture_list.emplace_back( make_shared<res_texture_list>() );
		res_texture_list& res_unit = *g_vres_texture_list.back();
		auto bin_res_unit = obj_res.via.array.ptr[ix];
		auto txt_name = bin_res_unit.via.array.ptr[0];
		auto txt_name_sz = txt_name.via.str.size;
		char* txt_kname = new char[txt_name_sz + 1];
		memcpy(txt_kname, txt_name.via.str.ptr, txt_name_sz);
		txt_kname[txt_name_sz] = 0;
		//res_unit.texture_pack_file = txt_kname;
		res_unit.texture_width = bin_res_unit.via.array.ptr[1].as<int>();
		res_unit.texture_height = bin_res_unit.via.array.ptr[2].as<int>();
		auto res_bin = bin_res_unit.via.array.ptr[3];
		{
			auto bin_sz = res_bin.via.bin.size;
			res_unit.txt_id = f_gen_txt(res_bin.via.bin.ptr, res_unit.texture_width, res_unit.texture_height, bin_sz,false);
		}
		delete[] txt_kname;
		auto res_data = bin_res_unit.via.array.ptr[4];
		auto res_data_sz = res_data.via.array.size;
		for (size_t iy = 0; iy < res_data_sz; iy++)
		{
			res_unit.vtexture_coordinates.emplace_back(res_texture_coordinate());
			res_texture_coordinate& cd_unit = res_unit.vtexture_coordinates.back();
			auto bin_cd_unit = res_data.via.array.ptr[iy];
			auto bin_filen = bin_cd_unit.via.array.ptr[0];
			cd_unit._file_name.reserve(bin_filen.via.str.size + 1);
			cd_unit._file_name.resize(bin_filen.via.str.size + 1);
			memcpy(&cd_unit._file_name[0], bin_filen.via.str.ptr, bin_filen.via.str.size);
			cd_unit._x0 = bin_cd_unit.via.array.ptr[1].as<float>();
			cd_unit._x1 = bin_cd_unit.via.array.ptr[2].as<float>();
			cd_unit._y0 = bin_cd_unit.via.array.ptr[3].as<float>();
			cd_unit._y1 = bin_cd_unit.via.array.ptr[4].as<float>();
		}

	}
	LOGI("load afb:%d,re_cnt:%d\n",__LINE__,re_cnt);
	auto obj_txt_list = obj_w.via.array.ptr[en_mtextures_res];
	auto txt_cnt = obj_txt_list.via.array.size;
	for (size_t ix = 0; ix < txt_cnt;ix++)
	{
		auto txt_unit = obj_txt_list.via.array.ptr[ix];
		auto txt_name = txt_unit.via.array.ptr[0];
		auto txt_width = txt_unit.via.array.ptr[1].as<uint32_t>();
		auto txt_height = txt_unit.via.array.ptr[2].as<uint32_t>();
		auto txt_bin = txt_unit.via.array.ptr[3];
		auto txt_mip = txt_unit.via.array.ptr[4];
		bool mipv = txt_mip.as<uint8_t>()>0;
		auto txt_name_sz = txt_name.via.str.size;
		char* txt_kname = new char[txt_name_sz+1];
		memset(txt_kname, 0, txt_name_sz + 1);
		memcpy(txt_kname, txt_name.via.str.ptr, txt_name_sz);
		g_mtexture_list[txt_kname] = make_shared<af_texture>();
		auto& a_txt = g_mtexture_list[txt_kname];
		a_txt->_width = txt_width;
		a_txt->_height = txt_height;
		a_txt->_mip_map = mipv;
		{
			auto txt_bin_sz = txt_bin.via.bin.size;
			a_txt->_atxt_id = f_gen_txt(txt_bin.via.bin.ptr, txt_width, txt_height, txt_bin_sz, mipv);
			a_txt->_loaded = true;
		}
	
		delete[] txt_kname;

	}
	LOGI("load afb:%d txt_cnt:%d\n",__LINE__,txt_cnt);
	auto obj_file_list = obj_w.via.array.ptr[en_mfiles_res];
	auto file_cnt = obj_file_list.via.array.size;
	for (size_t ix = 0; ix < file_cnt;ix++)
	{
		auto file_unit = obj_file_list.via.array.ptr[ix];
		auto file_name = file_unit.via.array.ptr[0];
		auto file_bin = file_unit.via.array.ptr[1];
		auto file_name_size = file_name.via.str.size;
		char* file_kname = new char[file_name_size + 1];
		memset(file_kname, 0, file_name_size + 1);
		memcpy(file_kname, file_name.via.str.ptr, file_name_size);
		auto a_file = make_shared<af_file>(file_bin.via.bin.size);
		memcpy(a_file->_pbin, file_bin.via.bin.ptr, a_file->_fsize);
		g_mfiles_list[file_kname] = a_file;
		delete[] file_kname;
	}
	LOGI("load afb:%d file_cnt:%d\n",__LINE__,file_cnt);
	auto obj_shader_list = obj_w.via.array.ptr[en_shaders_res];
	auto shd_cnt = obj_shader_list.via.array.size;
	if (g_output_bin_format._pgm_fmt == en_shader_code)
	{
		for (size_t ix = 0; ix < shd_cnt; ix++)
		{
			auto shd_unit = obj_shader_list.via.array.ptr[ix];
			auto shd_name = shd_unit.via.array.ptr[0];
			auto str_shd_nm_sz = shd_name.via.str.size;
			char* pshd_name = new char[str_shd_nm_sz + 1];
			memset(pshd_name, 0, str_shd_nm_sz + 1);
			memcpy(pshd_name, shd_name.via.str.ptr, str_shd_nm_sz);
			auto shd_vs_code = shd_unit.via.array.ptr[1];
			auto str_vs_code_sz = shd_vs_code.via.str.size;
			char* psd_vs_code = new char[str_vs_code_sz + 1];
			memset(psd_vs_code, 0, str_vs_code_sz + 1);
			memcpy(psd_vs_code, shd_vs_code.via.str.ptr, str_vs_code_sz);
			auto shd_fs_code = shd_unit.via.array.ptr[2];
			auto str_fs_code_sz = shd_fs_code.via.str.size;
			char* psd_fs_code = new char[str_fs_code_sz + 1];
			memset(psd_fs_code, 0, str_fs_code_sz + 1);
			memcpy(psd_fs_code, shd_fs_code.via.str.ptr, str_fs_code_sz);
			g_af_shader_list[pshd_name] = make_shared<af_shader>(psd_vs_code, psd_fs_code);
			delete[] pshd_name;
			delete[] psd_vs_code;
			delete[] psd_fs_code;
		}
	}
	else if (g_output_bin_format._pgm_fmt == en_shader_bin_general)
	{
		for (size_t ix = 0; ix < shd_cnt; ix++)
		{
			auto shd_unit = obj_shader_list.via.array.ptr[ix];
			auto shd_name = shd_unit.via.array.ptr[0];
			auto str_shd_nm_sz = shd_name.via.str.size;
			char* pshd_name = new char[str_shd_nm_sz + 1];
			memset(pshd_name, 0, str_shd_nm_sz + 1);
			memcpy(pshd_name, shd_name.via.str.ptr, str_shd_nm_sz);
			GLenum binaryFormat = shd_unit.via.array.ptr[1].as<int>();
			auto binCode = shd_unit.via.array.ptr[2];
			auto binCodeSize = binCode.via.bin.size;
			auto pbin = binCode.via.bin.ptr;
			g_af_shader_list[pshd_name] = make_shared<af_shader>(binaryFormat, (void*)pbin, binCodeSize);
			delete[] pshd_name;
		}
	}
	LOGI("load afb:%d shd_cnt=%d\n",__LINE__,shd_cnt);
	auto obj_material_list = obj_w.via.array.ptr[en_materials_res];
	auto mtl_cnt = obj_material_list.via.array.size;
	for (size_t ix = 0; ix < mtl_cnt; ix++)
	{
		auto mtl_unit = obj_material_list.via.array.ptr[ix];
		auto mtl_name = mtl_unit.via.array.ptr[0];
		auto mtl_name_size = mtl_name.via.str.size;
		char* pstr_mtl_name = new char[mtl_name_size + 1];
		memset(pstr_mtl_name, 0, mtl_name_size + 1);
		memcpy(pstr_mtl_name, mtl_name.via.str.ptr, mtl_name_size);
		shared_ptr<material> pmtl = make_shared<material>();
		g_material_list[pstr_mtl_name] = pmtl;
		auto shd_name = mtl_unit.via.array.ptr[1];
		auto shd_name_size = shd_name.via.str.size;
		char* pstr_shd_name = new char[shd_name_size + 1];
		memset(pstr_shd_name, 0, shd_name_size+1);
		memcpy(pstr_shd_name, shd_name.via.str.ptr, shd_name_size);
		auto pshd = g_af_shader_list.find(pstr_shd_name);
		if (pshd != g_af_shader_list.end())
		{
			pmtl->set_pshader(pshd->second);
		}
		auto& mpshd_uf = pmtl->get_mp_sd_uf();
		auto shd_uf_list = mtl_unit.via.array.ptr[2];
		auto shd_uf_list_cnt = shd_uf_list.via.array.size;
		for (size_t iy = 0; iy < shd_uf_list_cnt; iy++)
		{
			auto shd_uf_unit = shd_uf_list.via.array.ptr[iy];
			auto shd_uf_name = shd_uf_unit.via.array.ptr[0];
			auto shd_uf_name_sz = shd_uf_name.via.str.size;
			char* pshd_uf_name = new char[shd_uf_name_sz + 1];
			memset(pshd_uf_name, 0, shd_uf_name_sz + 1);
			memcpy(pshd_uf_name, shd_uf_name.via.str.ptr, shd_uf_name_sz);
			auto shd_uf_type=shd_uf_unit.via.array.ptr[1];
			auto shd_uf_type_sz = shd_uf_type.via.str.size;
			char* pshd_uf_type = new char[shd_uf_type_sz + 1];
			memset(pshd_uf_type, 0, shd_uf_type_sz + 1);
			memcpy(pshd_uf_type, shd_uf_type.via.str.ptr, shd_uf_type_sz);

			auto shd_uf_usize=shd_uf_unit.via.array.ptr[2];
			auto shd_uf_el_size=shd_uf_unit.via.array.ptr[3];
			auto shd_uf_utype=shd_uf_unit.via.array.ptr[4];

			shared_ptr<shader_uf> pnunf = std::move(fac_shader_uf::get().Create(pshd_uf_type, shd_uf_usize.as<GLuint>(), shd_uf_el_size.as<GLuint>()));
			pnunf->set_type(shd_uf_utype.as<GLenum>());
			auto shd_data = shd_uf_unit.via.array.ptr[5];
			auto wsize = shd_data.via.bin.size;// shd_uf_usize.as<GLuint>()*shd_uf_el_size.as<GLuint>();
			//printf("wsize=%d\n",wsize);
			
			memcpy(pnunf->get_data_head(), shd_data.via.bin.ptr,wsize);
			pnunf->link();
			mpshd_uf[pshd_uf_name] = pnunf;
			delete[] pshd_uf_name;
			delete[] pshd_uf_type;
		}
		delete[] pstr_mtl_name;
	}
	LOGI("load afb:%d mtl_cnt:%d\n",__LINE__,mtl_cnt);
	auto obj_primitives = obj_w.via.array.ptr[en_primitives_res];
	auto obj_prm_size = obj_primitives.via.array.size;
	for (size_t ix = 0; ix < obj_prm_size;++ix)
	{
		auto prm_unit = obj_primitives.via.array.ptr[ix];
		auto prm_name = prm_unit.via.array.ptr[0];
		auto p_vt_bf_len = prm_unit.via.array.ptr[1];
		auto pebo_len = prm_unit.via.array.ptr[2];
		auto pformat=prm_unit.via.array.ptr[3];
		auto prm_name_sz = prm_name.via.str.size;
		string key_name;
		key_name.resize(prm_name_sz);
		memcpy(&key_name[0], prm_name.via.str.ptr, prm_name_sz);
		//key_name[prm_name_sz] = 0;
		auto p_obj = make_shared<primitive_object>();
		auto ebo_len = pebo_len.as<GLuint>();
		auto vbo_len = p_vt_bf_len.as<GLuint>();
		GLfloat* pvbo = 0;
		GLuint* pebo = 0;
		auto ifile = g_mfiles_list.find(key_name);
		if (ifile!=g_mfiles_list.end())
		{
			auto& fobj = *ifile->second;
			char* phead = (char*)fobj._pbin;
			GLuint* phead_len = (GLuint*)phead;
			phead += 4;
			pvbo = (GLfloat*)phead;
			if (ebo_len)
			{
				pebo = (GLuint*)(phead + *phead_len);
			}
		}
		
		vector<GLubyte> ele_fmt;
		auto fmt_sz = pformat.via.bin.size;
		ele_fmt.resize(fmt_sz);
		memcpy(&ele_fmt[0], pformat.via.bin.ptr, fmt_sz);
		p_obj->set_ele_format(ele_fmt);
		p_obj->load_vertex_data(pvbo, vbo_len, pebo, ebo_len);
		p_obj->_ps_file = ifile->second;
		g_primitive_list[key_name] = p_obj;
	}
	LOGI("load afb:%d,obj_prm_size:%d\n",__LINE__,obj_prm_size);
	auto obj_models = obj_w.via.array.ptr[en_models_res];
	auto obj_modles_sz = obj_models.via.array.size;
	for (size_t ix = 0; ix < obj_modles_sz;++ix)
	{
		auto obj_md_unit = obj_models.via.array.ptr[ix];
		auto md_name = obj_md_unit.via.array.ptr[0];
		auto obj_msh_lst=obj_md_unit.via.array.ptr[1];
		string md_key;
		auto md_name_sz = md_name.via.str.size;
		md_key.resize(md_name_sz);
		memcpy(&md_key[0], md_name.via.str.ptr, md_name_sz);
		//md_key[md_name_sz] = 0;
		g_mmodel_list[md_key] = make_shared<af_model>();
		auto& mdl = *g_mmodel_list[md_key];
		auto obj_msh_lst_sz = obj_msh_lst.via.array.size;
		for (size_t iy = 0; iy < obj_msh_lst_sz; iy++)
		{
			mdl.emplace_back();
			af_mesh& mesh_unit = mdl[iy];
			auto obj_mesh_unit = obj_msh_lst.via.array.ptr[iy];
			auto obj_prmid = obj_mesh_unit.via.array.ptr[0];
			auto obj_diffs = obj_mesh_unit.via.array.ptr[1];
			auto obj_speculars = obj_mesh_unit.via.array.ptr[2];
			auto obj_heights = obj_mesh_unit.via.array.ptr[3];
			auto obj_ambients = obj_mesh_unit.via.array.ptr[4];
			string& prim_id = mesh_unit._prm_id;
			auto prim_id_sz = obj_prmid.via.str.size;
			prim_id.resize(prim_id_sz);
			memcpy(&prim_id[0], obj_prmid.via.str.ptr, prim_id_sz);
			const auto& iprim = g_primitive_list.find(prim_id);
			assert(iprim != g_primitive_list.end() && "prim id is missed?");
			mesh_unit._ps_prm_id = iprim->second;
			auto obj_diffs_sz = obj_diffs.via.array.size;
			for (size_t id = 0; id < obj_diffs_sz;++id)
			{
				auto obj_diff = obj_diffs.via.array.ptr[id];
				auto obj_diff_sz = obj_diff.via.str.size;
				string diff;
				diff.resize(obj_diff_sz);
				memcpy(&diff[0], obj_diff.via.str.ptr, obj_diff_sz);
				mesh_unit._text_diffuse_list.emplace_back(diff);
				const auto& idiff = g_mtexture_list.find(diff);
				if (idiff!=g_mtexture_list.end())
				{
					mesh_unit._ps_text_diffuse_list.emplace_back(idiff->second);
				}
			}
			auto obj_speculars_sz = obj_speculars.via.array.size;
			for (size_t id = 0; id < obj_speculars_sz; ++id)
			{
				auto obj_specular = obj_speculars.via.array.ptr[id];
				auto obj_specular_sz = obj_specular.via.str.size;
				string specular;
				specular.resize(obj_specular_sz);
				memcpy(&specular[0], obj_specular.via.str.ptr, obj_specular_sz);
				mesh_unit._text_specular_list.emplace_back(specular);
				const auto& ispec = g_mtexture_list.find(specular);
				if (ispec!=g_mtexture_list.end())
				{
					mesh_unit._ps_text_specular_list.emplace_back(ispec->second);
				}
			}
			auto obj_heights_sz = obj_heights.via.array.size;
			for (size_t id = 0; id < obj_heights_sz; ++id)
			{
				auto obj_height = obj_heights.via.array.ptr[id];
				auto obj_height_sz = obj_height.via.str.size;
				string height;
				height.resize(obj_height_sz);
				memcpy(&height[0], obj_height.via.str.ptr, obj_height_sz);
				mesh_unit._text_height_list.emplace_back(height);
				const auto& iheight = g_mtexture_list.find(height);
				if (iheight!=g_mtexture_list.end())
				{
					mesh_unit._ps_text_height_list.emplace_back(iheight->second);
				}
			}
			auto obj_ambients_sz = obj_ambients.via.array.size;
			for (size_t id = 0; id < obj_ambients_sz; ++id)
			{
				auto obj_ambient = obj_ambients.via.array.ptr[id];
				auto obj_ambient_sz = obj_ambient.via.str.size;
				string ambient;
				ambient.resize(obj_ambient_sz);
				memcpy(&ambient[0], obj_ambient.via.str.ptr, obj_ambient_sz);
				mesh_unit._text_ambient_list.emplace_back(ambient);
				const auto& iamb = g_mtexture_list.find(ambient);
				if (iamb!=g_mtexture_list.end())
				{
					mesh_unit._ps_text_ambient_list.emplace_back(iamb->second);
				}
			}
		}
	}
	LOGI("load afb:%d,obj_modles_sz:%d\n",__LINE__,obj_modles_sz);
	auto obj_ui = obj_w.via.array.ptr[en_control_res];
	init_ui_component_by_mgo(_pj, obj_ui);
	auto obj_2_prp_pos = [this](msgpack::v2::object& okey,prop_ele_position&prp_epos){
		auto obin_sz = okey.via.bin.size;
		auto rsz = obin_sz / sizeof(unsigned short);
		prop_ele_pos_index prp_ep_idx;
		prp_ep_idx.resize(rsz);
		memcpy(&prp_ep_idx[0], okey.via.bin.ptr, obin_sz);
		int ii = rsz - 1;
		base_ui_component* pcontrol = _pj;
		for (; ii > 1;ii--)
		{
			pcontrol = pcontrol->get_child(prp_ep_idx[ii]);
		}
		prp_epos._pobj = pcontrol;
		prp_epos._page_index = prp_ep_idx[1];
		prp_epos._field_index = prp_ep_idx[0];
	};	
	auto obj_valiase = obj_w.via.array.ptr[en_aliase_dic];
	auto obj_aliase_sz = obj_valiase.via.array.size;
	for (size_t ix = 0; ix < obj_aliase_sz;ix++)
	{
		auto obj_aliase = obj_valiase.via.array.ptr[ix];
		auto obj_key = obj_aliase.via.array.ptr[0];
		auto obj_key_sz = obj_key.via.str.size;
		string aliase_key;
		aliase_key.resize(obj_key_sz);
		memcpy(&aliase_key[0], obj_key.via.str.ptr, obj_key_sz);
		auto obj_pep_pos = obj_aliase.via.array.ptr[1];
		auto ps_pep_pos = make_shared<prop_ele_position>();
		obj_2_prp_pos(obj_pep_pos, *ps_pep_pos);
		g_aliase_dic[aliase_key] = ps_pep_pos;
	}
	LOGI("load afb:%d,obj_aliase_sz:%d\n",__LINE__,obj_aliase_sz);
	auto obj_bind_dic = obj_w.via.array.ptr[en_bind_dic];
	auto obj_bind_dic_sz = obj_bind_dic.via.array.size;
	for (size_t ix = 0; ix < obj_bind_dic_sz;ix++)
	{
		auto oprp_ele = obj_bind_dic.via.array.ptr[ix];
		auto obind_key = oprp_ele.via.array.ptr[0];
		prop_ele_position prp_ele_pos;
		obj_2_prp_pos(obind_key, prp_ele_pos);
		auto ps_bind_unit = make_shared<prop_ele_bind_unit>();
		auto& vparam = ps_bind_unit->_param_list;
		auto opm_list = oprp_ele.via.array.ptr[1];
		auto opm_list_sz = opm_list.via.array.size;
		for (size_t iix = 0; iix < opm_list_sz;iix++)
		{
			auto opele_pos = opm_list.via.array.ptr[iix];
			vparam.emplace_back();	
			auto& sub_prp_ele_pos = vparam.back(); //vparam[iix];
			obj_2_prp_pos(opele_pos, sub_prp_ele_pos);
		}
		auto oexpr = oprp_ele.via.array.ptr[2];
		auto& expr = ps_bind_unit->_expression;
		auto exp_sz = oexpr.via.str.size;
		expr.resize(exp_sz);
		memcpy(&expr[0], oexpr.via.str.ptr, exp_sz);
		g_bind_dic[prp_ele_pos] = ps_bind_unit;
	}
	LOGI("load afb:%d,obj_bind_dic_sz:%d\n",__LINE__,obj_bind_dic_sz);
	auto obj_bind_ref_dic = obj_w.via.array.ptr[en_bind_ref_dic];
	auto bind_ref_dic_sz = obj_bind_ref_dic.via.array.size;
	for (size_t ix = 0; ix < bind_ref_dic_sz;ix++)
	{
		auto oprp_ele = obj_bind_ref_dic.via.array.ptr[ix];
		auto obind_key = oprp_ele.via.array.ptr[0];
		prop_ele_position prp_ele_pos;
		obj_2_prp_pos(obind_key, prp_ele_pos);
		auto ps_ref_list = make_shared<vprop_pos>();
		auto oref_list=oprp_ele.via.array.ptr[1];
		auto oref_list_sz = oref_list.via.array.size;
		for (size_t iix = 0; iix < oref_list_sz;iix++)
		{
			auto oref = oref_list.via.array.ptr[iix];
			ps_ref_list->emplace_back();
			auto& sub_prp_pos = (*ps_ref_list)[iix];
			obj_2_prp_pos(oref, sub_prp_pos);
		}
		g_bind_ref_dic[prp_ele_pos] = ps_ref_list;
	}
	LOGI("load afb:%d,bind_ref_dic_sz:%d\n",__LINE__,bind_ref_dic_sz);
	auto obj_state_manger = obj_w.via.array.ptr[en_state_manager];
	auto stm_sz = obj_state_manger.via.array.size;
	for (size_t ix = 0; ix < stm_sz;ix++)
	{
		auto omstm = obj_state_manger.via.array.ptr[ix];
		auto omkey = omstm.via.array.ptr[0];
		auto mkey_sz = omkey.via.str.size;
		string mskey;
		mskey.resize(mkey_sz);
		memcpy(&mskey[0], omkey.via.str.ptr, mkey_sz);
		auto ps_stm = make_shared<af_state_manager>();
		auto& stm = *ps_stm;
		auto& prop_list = stm._prop_list;
		auto oprop_list = omstm.via.array.ptr[1];
		auto oprp_lst_sz = oprop_list.via.array.size;
		for (size_t ii = 0; ii < oprp_lst_sz;ii++)
		{
			auto oprp_id = oprop_list.via.array.ptr[ii];
			prop_list.emplace_back();
			auto& prp_ele_pos = prop_list[ii];
			obj_2_prp_pos(oprp_id, prp_ele_pos);
		}
		auto oprp_value_list = omstm.via.array.ptr[2];
		auto oprp_value_list_sz = oprp_value_list.via.array.size;
		auto& prp_value_list = stm._prop_value_list;
		for (size_t ii = 0; ii < oprp_value_list_sz;ii++)
		{
			auto ovprp_value = oprp_value_list.via.array.ptr[ii];
			prp_value_list.emplace_back();
			auto& prp_value = prp_value_list[ii];
			auto ovprp_value_sz = ovprp_value.via.array.size;
			for (size_t jj = 0; jj< ovprp_value_sz;jj++)
			{
				auto ovalue_bock = ovprp_value.via.array.ptr[jj];
				auto valuesz = ovalue_bock.via.bin.size;
				prp_value.emplace_back();
				auto& value_bk = prp_value[jj];
				value_bk.resize(valuesz);
				memcpy(&value_bk[0], ovalue_bock.via.bin.ptr, valuesz);
			}
		}
		auto oany= omstm.via.array.ptr[3];
		auto& any_to_any = stm._any_to_any;
		any_to_any._start_time = oany.via.array.ptr[0].as<int>();
		any_to_any._duration=oany.via.array.ptr[1].as<int>();
		any_to_any._easing_func = oany.via.array.ptr[2].as<int>();
		stm._state_idx=omstm.via.array.ptr[4].as<unsigned char>();
		auto omtrans=omstm.via.array.ptr[5];
		auto& mtrans = stm._mtrans;
		auto mtrans_sz = omtrans.via.array.size;
		for (size_t jj = 0; jj < mtrans_sz;jj++)
		{
			auto otrans = omtrans.via.array.ptr[jj];
			auto ofrom = otrans.via.array.ptr[0];
			auto oto = otrans.via.array.ptr[1];
			trans_key tkey = { ofrom.as<int>(), oto.as<int>() };
			auto ostart_tm = otrans.via.array.ptr[2];
			auto oduration=otrans.via.array.ptr[3];
			auto oseasing_fun = otrans.via.array.ptr[4];
			auto ps_trans = make_shared<state_transition>();
			ps_trans->_start_time = ostart_tm.as<int>();
			ps_trans->_duration = oduration.as<int>();
			ps_trans->_easing_func = oseasing_fun.as<int>();
			mtrans[tkey] = ps_trans;
		}
          auto oplaylist_list = omstm.via.array.ptr[ 6 ];
          auto& playlist_list = stm._playlist_list;
          auto oplstlst_sz = oplaylist_list.via.array.size;
          for( size_t jj = 0; jj < oplstlst_sz; jj++ )
          {
               auto oplaylist = oplaylist_list.via.array.ptr[ jj ];
               playlist_list.emplace_back();
               auto& playlist = playlist_list[ jj ];
               auto plsz = oplaylist.via.array.size;
               for( size_t ix = 0; ix < plsz;ix++ )
               {
                    auto otran = oplaylist.via.array.ptr[ ix ];
                    playlist.emplace_back();
                    auto& plu = playlist[ ix ];
                    plu._from = otran.via.array.ptr[ 0 ].as<int>();
                    plu._to=otran.via.array.ptr[ 1 ].as<int>();
               }
          }
		g_mstate_manager[mskey] = ps_stm;
	}
	 LOGI("load afb:%d,stm_sz:%d\n",__LINE__,stm_sz);
     auto obj_common_value_dic = obj_w.via.array.ptr[ en_common_value ];
     auto cmv_sz = obj_common_value_dic.via.array.size;
     for( size_t ix = 0; ix < cmv_sz; ix++ )
     {
          auto ocmv_u = obj_common_value_dic.via.array.ptr[ ix ];
          auto ocmv_key = ocmv_u.via.array.ptr[ 0 ];
          auto mkey_sz = ocmv_key.via.str.size;
          string mskey,scmv_tp;
          mskey.resize( mkey_sz );
          memcpy( &mskey[ 0 ], ocmv_key.via.str.ptr, mkey_sz );
          auto cmv_tp = ocmv_u.via.array.ptr[ 1 ];
          auto tp_sz = cmv_tp.via.str.size;
          scmv_tp.resize( tp_sz );
          memcpy( &scmv_tp[ 0 ], cmv_tp.via.str.ptr, tp_sz );
          auto ps_cmv = make_shared<base_prp_type>( scmv_tp );
          auto ovalue = ocmv_u.via.array.ptr[ 2 ];
          auto ov_sz = ovalue.via.bin.size;
          memcpy( ps_cmv->_pbase, ovalue.via.bin.ptr, ov_sz );
          auto& prop_list = ps_cmv->_param_list;
          auto oprop_list=ocmv_u.via.array.ptr[ 3 ];
          auto oprop_sz = oprop_list.via.array.size;
          for( size_t ii = 0; ii < oprop_sz; ii++ )
          {
               auto oprp_id = oprop_list.via.array.ptr[ ii ];
               prop_list.emplace_back();
               auto& prp_ele_pos = prop_list[ ii ];
               obj_2_prp_pos( oprp_id, prp_ele_pos );
          }
          g_base_prp_dic[ mskey ] = ps_cmv;

    }
	LOGI("load afb:%d,cmv_sz:%d\n",__LINE__,cmv_sz);
	auto obj_feedback_list = obj_w.via.array.ptr[en_feedback_list];
	auto fdsz = obj_feedback_list.via.array.size;
	for (size_t ix = 0; ix < fdsz;ix++)
	{
		 auto ofdv_u = obj_feedback_list.via.array.ptr[ix];
		 auto omtl_key = ofdv_u.via.array.ptr[0];
		 auto oprm_key = ofdv_u.via.array.ptr[1];
		 string mtl_key, prm_key;
		 auto mtl_key_sz = omtl_key.via.str.size;
		 mtl_key.resize(mtl_key_sz);
		 memcpy(&mtl_key[0], omtl_key.via.str.ptr, mtl_key_sz);
		 const auto& imtl = g_material_list.find(mtl_key);
		 if (imtl == g_material_list.end())
		 {
			 LOGE("invalid material name:%s for the current feedback\n", mtl_key.c_str());
			 continue;
		 }
		 auto prm_key_sz = oprm_key.via.str.size;
		 prm_key.resize(prm_key_sz);
		 memcpy(&prm_key[0], oprm_key.via.str.ptr, prm_key_sz);
		 const auto& iprm = g_primitive_list.find(prm_key);
		 if (iprm == g_primitive_list.end())
		 {
			 LOGE("invalid primitive name:%s for the current feedback\n", prm_key.c_str());
			 continue;
		 }
		 feedback_key fkey = { mtl_key, prm_key };
		 g_feedback_list[fkey] = make_shared<af_feedback>(imtl->second, iprm->second);
	}
	LOGI("load afb:%d,fdsz:%d\n",__LINE__,fdsz);
	auto obj_playlist_group_list = obj_w.via.array.ptr[en_playlist_group_list];
	fdsz = obj_playlist_group_list.via.array.size;
	for (size_t ix = 0; ix < fdsz; ix++)
	{
		auto oplg = obj_playlist_group_list.via.array.ptr[ix];
		auto oplg_key = oplg.via.array.ptr[0];
		auto oplg_u = oplg.via.array.ptr[1];
		string plg_key;
		auto plg_key_sz = oplg_key.via.str.size;
		plg_key.resize(plg_key_sz);
		memcpy(&plg_key[0], oplg_key.via.str.ptr, plg_key_sz);
		auto ps_plg_list = make_shared<playlist_unit_list>();
		g_playlist_group_list[plg_key] = ps_plg_list;
		auto plg_u_sz = oplg_u.via.array.size;
		for (size_t iy = 0; iy < plg_u_sz;iy++)
		{
			 auto oplg = oplg_u.via.array.ptr[iy];
			 auto ostm = oplg.via.array.ptr[0];
			 string stm_name;
			 auto ostm_sz = ostm.via.str.size;
			 stm_name.resize(ostm_sz);
			 memcpy(&stm_name[0], ostm.via.str.ptr, ostm_sz);
			 auto oplaylist_id = oplg.via.array.ptr[1];
			 playlist_unit plu = { stm_name, oplaylist_id.as<int>() };
			 ps_plg_list->emplace_back(plu);
		}
	}
	LOGI("load afb:%d,playlist_group:%d\n",__LINE__,fdsz);
}
