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
#include <memory>
#include <ctime>
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
	GLuint txt_id{0};
	int texture_width;
	int texture_height;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	string texture_pack_file;
	string texture_data_file;
#else
	atomic<bool> _loaded{ false };
	string txt_buff;
#endif
	bool _is_separated{ false };	//string file_name_sets;
	vres_txt_cd vtexture_coordinates;
	unsigned int texture_id()
	{
#ifdef IMGUI_DISABLE_DEMO_WINDOWS
		if(!_is_separated)
		{
			return txt_id;
		}
		while (!_loaded)
		{
			this_thread::yield();
		}
		if (txt_buff.size()>0)
		{
			glBindTexture(GL_TEXTURE_2D, txt_id);
			int pix_sz = texture_width*texture_height * 4;
			if (txt_buff.size()<pix_sz)// texture is compressed
			{
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, texture_width, texture_height, 0, txt_buff.size(), &txt_buff[0]);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height,
					0, GL_RGBA, GL_UNSIGNED_BYTE, &txt_buff[0]);

			}
			glBindTexture(GL_TEXTURE_2D, 0);
			txt_buff.clear();
			txt_buff.shrink_to_fit();
		}
#endif
		return txt_id;
	}
#if 0
	res_texture_list()
		:txt_id(0)
	{
	}
	res_texture_list(res_texture_list& target)
	{
		
          txt_id = target.txt_id;
          texture_width = target.texture_width;
          texture_height = target.texture_height;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
          texture_pack_file=target.texture_pack_file;
          texture_data_file=target.texture_data_file;

#endif
          _is_separated = target._is_separated;
          vtexture_coordinates = target.vtexture_coordinates;
	}
	res_texture_list& operator =(res_texture_list& tar)
	{
		txt_id = tar.texture_id();
		return *this;
	}
#endif
	~res_texture_list()
	{
		glDeleteTextures(1, &txt_id);
	}
};
using sd_res_txt_list = shared_ptr<res_texture_list>;
using  vres_txt_list = vector<sd_res_txt_list>;
extern vres_txt_list g_vres_texture_list;
//enum res_output_model
//{
//	en_integrated,
//	en_discrete,
//};
struct af_texture
{
	bool _mip_map{ false };
	GLuint _atxt_id{ 0 };
	GLuint _width{ 0 }, _height{ 0 };
#ifdef IMGUI_DISABLE_DEMO_WINDOWS
	atomic<bool> _loaded{ false };
	string txt_buff;
#endif
	GLuint _txt_id()
	{
#ifdef IMGUI_DISABLE_DEMO_WINDOWS
		if(!_is_separated)
		{
			return _atxt_id;
		}
		while (!_loaded)
		{
			this_thread::yield();
		}
		if (txt_buff.size() > 0)
		{
			glBindTexture(GL_TEXTURE_2D, _atxt_id);
			int pix_sz = _width*_height * 4;
			if (_mip_map)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 为MipMap设定filter方法
			}
			if (txt_buff.size() < pix_sz)// texture is compressed
			{
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, _width, _height, 0, txt_buff.size(), &txt_buff[0]);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height,
					0, GL_RGBA, GL_UNSIGNED_BYTE, &txt_buff[0]);

			}
			if (_mip_map)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			txt_buff.clear();
			txt_buff.shrink_to_fit();
		}
#endif
		return _atxt_id;
	}
	bool _is_separated{ false };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	bool _sel{ false };
#endif
	~af_texture()
	{
		glDeleteTextures(1, &_atxt_id);
	}
};
struct af_file
{
	void* _pbin = {NULL};
	GLuint _fsize = { 0 };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	bool _sel{ false };
#endif
	af_file(GLuint fsize);
	//af_file(){}
	void re_alloc(GLuint fsize);
	~af_file();
};
#include<map>
#include <memory>
using ps_af_texture = shared_ptr<af_texture>;
using ps_af_file = shared_ptr<af_file>;
typedef map<string, ps_af_texture> mtexture_list;
typedef map<string, ps_af_file>mfile_list;
extern mtexture_list g_mtexture_list;
extern mfile_list g_mfiles_list;
void save_ojfile_to_file(string& key_name);

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
extern bool add_image_to_mtexure_list(string& imgPath,bool is_mipmap);
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
	//output_bin_format() :_txt_fmt(en_uncompressed_txt), _pgm_fmt(en_shader_code){}
};
extern bool get_texture_item( void* data, int idx, const char** out_str );
extern bool get_texture_group_name( void* data, int idx, const char** out_str );
extern output_bin_format g_output_bin_format;
enum en_backup_model
{
     en_manual_backup,
     en_periodic_backup,
     en_intelligent_backup,
     en_backup_model_cnt
};
extern const char* str_backup_model[ en_backup_model_cnt ];
struct af_file_unit
{
     string file_name;
     time_t _md_time;
};
struct project_backup_manager
{
     int backup_model = en_manual_backup;
     int backup_interval = 5;//mins
     int backup_max_cnt = 10;
     int max_back_id = 0;
     vector<af_file_unit> back_up_prj_list;
};
extern project_backup_manager g_prj_backup_mg;