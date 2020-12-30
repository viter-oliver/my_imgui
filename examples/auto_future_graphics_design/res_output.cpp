#pragma once
#include "res_output.h"
#include <stdlib.h>
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "dir_output.h"
const char* mesh_fold = "mesh_res\\";
const char* font_fold = "fonts\\";
const char* files_fold = "files\\";
const char* image_fold = "images\\";
const char* shaders_fold = "shaders\\";
const char* text_res_fold = "texture_res_list\\";
const char* afb_fold = "afb\\";
#endif
vres_txt_list  g_vres_texture_list;
int g_cur_texture_id_index=0;
mtexture_list g_mtexture_list;
bool get_texture_group_name( void* data, int idx, const char** out_str )
{
     *out_str = g_vres_texture_list[ idx ].texture_pack_file.c_str();
     return true;
}
bool get_texture_item(void* data, int idx, const char** out_str)
{
     int image_id = *(int *)data;
     *out_str = g_vres_texture_list[ image_id ].vtexture_coordinates[ idx ]._file_name.c_str();
	return true;
}

af_file::af_file(GLuint fsize)
	:_fsize(fsize)
{
	_pbin = malloc(fsize);
}
void af_file::re_alloc(GLuint fsize)
{
	free(_pbin);
	_pbin = malloc(fsize);
}
af_file::~af_file()
{
	free(_pbin);
}
mfile_list g_mfiles_list;

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "SOIL.h"
#include <fstream>
#include "dir_output.h"
extern string g_cureent_directory;

bool add_image_to_mtexure_list(string& imgPath, bool is_mipmap)
{
	string img_file_name = imgPath.substr(imgPath.find_last_of('\\') + 1);
	auto itimg = g_mtexture_list.find(img_file_name);
	if (itimg!=g_mtexture_list.end())
	{
		printf("%s has already exist in the texture list!\n", img_file_name.c_str());
		return false;
	}
	string img_file_path = imgPath.substr(0, imgPath.find_last_of('\\') + 1);

	GLubyte* imgdata = NULL;
	int width, height, channels;

	imgdata = SOIL_load_image(imgPath.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
	if (imgdata == NULL)
	{
		printf("Fail to load texture file:%s\n", imgPath.c_str());
		return false;
	}
	string str_img_path = g_cureent_directory+image_fold;
	if (img_file_path != str_img_path)
	{
		string str_cmd = "copy ";
		str_cmd += imgPath;
		str_cmd += " ";
		str_cmd += str_img_path;
		system(str_cmd.c_str());

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

	// Step4 加载纹理
	if (is_mipmap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); // 为MipMap设定filter方法
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);	
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);
	}
	// Step5 释放纹理图片资源
	SOIL_free_image_data(imgdata);
	auto pimge = make_shared<af_texture>();
	pimge->_atxt_id = textureId;
	pimge->_mip_map = is_mipmap;
	pimge->_width = width;
	pimge->_height = height;
	g_mtexture_list[img_file_name] = pimge;

	return true;
}
void add_file_to_mfiles_list(string& file_path)
{
	string str_file = file_path;
	string strfile_name = str_file.substr(str_file.find_last_of('\\') + 1);
	string str_file_path = str_file.substr(0, str_file.find_last_of('\\') + 1);
	string af_file_path = g_cureent_directory+files_fold;
	if (str_file_path != af_file_path)
	{
		string str_cmd = "copy ";
		str_cmd += str_file;
		str_cmd += " ";
		str_cmd += af_file_path;
		system(str_cmd.c_str());

	}
	ifstream ifs;
	ifs.open(file_path);
	if (!ifs.is_open())
	{
		MessageBox(GetForegroundWindow(), "fail to load file", "Error info", MB_OK);
		return;
	}
	filebuf* pbuf = ifs.rdbuf();
	size_t sz_file = pbuf->pubseekoff(0, ifs.end, ifs.in);
	pbuf->pubseekpos(0, ifs.in);
	g_mfiles_list[strfile_name] = make_shared<af_file>(sz_file);
	pbuf->sgetn((char*)g_mfiles_list[strfile_name]->_pbin, sz_file);
	ifs.close();
}
void save_ojfile_to_file(string& key_name)
{
	auto ifl = g_mfiles_list.find(key_name);
	if (ifl == g_mfiles_list.end())
	{
		printf("invalid file key:%s\n", key_name.c_str());
		return;
	}
	auto& ofl = *ifl->second;
	string af_file_path = g_cureent_directory + files_fold;
	string file_full_path = af_file_path + "\\";
	file_full_path += key_name;
	ofstream ofs;
	ofs.open(file_full_path, ios::out | ios::binary);
	ofs.write((char*)ofl._pbin, ofl._fsize);
	ofs.close();
}
#endif
output_bin_format g_output_bin_format;