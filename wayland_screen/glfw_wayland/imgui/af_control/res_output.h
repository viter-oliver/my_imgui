#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
//ÎÆÀí×ÊÔ´
using namespace std;
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
	int texture_width;
	int texture_height;
	unsigned int texture_id;
	string texture_pack_file;
	string texture_data_file;
	//string file_name_sets;
	vres_txt_cd vtexture_coordinates;
	res_texture_list()
	{
	}
	~res_texture_list()
	{
	}
};
typedef vector<res_texture_list> vres_txt_list;
extern vres_txt_list g_vres_texture_list;
extern int g_cur_texture_id_index;
extern bool get_texture_item(void* data, int idx, const char** out_str);
struct af_texture
{
	GLuint _txt_id{ 0 }, _width{ 0 }, _height{ 0 };
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