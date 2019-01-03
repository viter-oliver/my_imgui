#pragma once
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif 
#include <vector>
#include <string>
#include <atomic>
#include <thread>
//纹理资源
using namespace std;
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
struct res_texture_coordinate
{
	string _file_name;
	float _x0, _y0, _x1, _y1;
	res_texture_coordinate() :_x0(0.f), _y0(0.f), _x1(0.f), _y1(0.f){}
	float owidth(){ return _x1 - _x0; }
	float oheight(){ return _y1 - _y0; }
};
typedef vector<res_texture_coordinate> vres_txt_cd;
struct res_texture_list
{
	atomic<bool> _loaded{ false };
	GLuint txt_id{0};
	int texture_width;
	int texture_height;
	string txt_buff;
	string texture_pack_file;
	string texture_data_file;
	bool _is_separated{ false };	//string file_name_sets;
	vres_txt_cd vtexture_coordinates;
	unsigned int texture_id()
	{
		while (!_loaded)
		{
			this_thread::yield();
		}
		if (txt_buff.size()>0)
		{
			glBindTexture(GL_TEXTURE_2D, txt_id);
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, texture_width, texture_height, 0, txt_buff.size(), &txt_buff[0]);
			glBindTexture(GL_TEXTURE_2D, 0);
			txt_buff.clear();
		}
		return txt_id;
	}
	res_texture_list()
		:txt_id(0)
	{
	}
	res_texture_list(res_texture_list& target)
	{
		if (target.txt_id==0)
		{
			txt_id = 0;
		}
		else
		{
			txt_id = texture_id();
		}
	}
	res_texture_list& operator =(res_texture_list& tar)
	{
		txt_id = tar.texture_id();
		return *this;
	}
	~res_texture_list()
	{
	}
};
typedef vector<res_texture_list> vres_txt_list;
extern vres_txt_list g_vres_texture_list;
extern int g_cur_texture_id_index;
//enum res_output_model
//{
//	en_integrated,
//	en_discrete,
//};
extern bool get_texture_item(void* data, int idx, const char** out_str);
struct af_texture
{
	atomic<bool> _loaded{ false };
	GLuint _atxt_id{ 0 };
	string txt_buff;
	GLuint _width{ 0 }, _height{ 0 };
	GLuint _txt_id()
	{
		while (!_loaded)
		{
			this_thread::yield();
		}
		if (txt_buff.size() > 0)
		{
			glBindTexture(GL_TEXTURE_2D, _atxt_id);
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, _width, _height, 0, txt_buff.size(), &txt_buff[0]);
			glBindTexture(GL_TEXTURE_2D, 0);
			txt_buff.clear();
		}
		return _atxt_id;
	}
	bool _is_separated{ false };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	bool _sel{ false };
#endif
};
struct af_file
{
	void* _pbin = {NULL};
	GLuint _fsize = { 0 };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	bool _sel{ false };
#endif
	af_file(GLuint fsize);
	~af_file();
};
#include<map>
#include <memory>
typedef map<string, shared_ptr<af_texture>> mtexture_list;
typedef map<string, shared_ptr<af_file>>mfile_list;
extern mtexture_list g_mtexture_list;
extern mfile_list g_mfiles_list;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
extern bool add_image_to_mtexure_list(string& imgPath);
extern void add_file_to_mfiles_list(string& file_path);
#endif
enum texture_format
{
	en_uncompressed_txt,
	en_dxt5,
	en_atc,
	en_etc2,
	en_pvrtc,
	en_bptc,
	en_pallet_txt,
};
enum program_format
{
	en_shader_code,
	en_shader_bin_general,
	en_shader_bin_vivante,
};

struct output_bin_format
{
	texture_format _txt_fmt;
	program_format _pgm_fmt;
	output_bin_format() :_txt_fmt(en_uncompressed_txt), _pgm_fmt(en_shader_code){}
};
extern output_bin_format g_output_bin_format;