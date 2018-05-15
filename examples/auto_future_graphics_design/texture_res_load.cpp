#include "texture_res_load.h"
#ifdef IMGUI_WAYLAND
#include "../../deps/glad/glad.h"
#else
#include <GL/gl3w.h> 
#endif

 
#include <GLFW/glfw3.h>
#include "SOIL.h"
#include "texture.h"
#include <fstream>
/*
field:
texture_res_list
  texture_pack_file
  texture_data_file
*/
void texture_res_load::load_res_from_json(Value& jroot)
{
	g_cur_texture_id_index = jroot["texture_id_index"].asInt();
	Value& texture_res_list = jroot["texture_res_list"];
	int isize = texture_res_list.size();
	for (int ix = 0; ix < isize;ix++)
	{
		Value& junit = texture_res_list[ix];
		Value& texture_pack_file = junit["texture_pack_file"];
		Value& texture_data_file = junit["texture_data_file"];
		_texture_res_tar.emplace_back();
		res_texture_list& rtlist = _texture_res_tar[ix];
		rtlist.texture_pack_file = texture_pack_file.asString();
		rtlist.texture_data_file = texture_data_file.asString();
		load_texture_info(rtlist, rtlist.texture_pack_file, rtlist.texture_data_file);
		
	}
}

extern string g_cureent_project_file_path;
bool load_texture_info(res_texture_list& rtlist, string& str_txt_pack_file, string& str_txt_data_file)
{
	string str_texture_pack_file = g_cureent_project_file_path.substr(0,g_cureent_project_file_path.find_last_of('\\')+1);
	str_texture_pack_file += "texture_res_list\\";
	string str_texture_data_file = str_texture_pack_file;
		
	str_texture_pack_file += str_txt_pack_file;
	str_texture_data_file += str_txt_data_file;
		
	rtlist.texture_id = TextureHelper::load2DTexture(str_texture_pack_file.c_str(), \
	rtlist.texture_width, rtlist.texture_height, GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA);
	if (rtlist.texture_id==0)
	{
		return false;
	}
	ifstream fin;
	fin.open(str_texture_data_file);
	if (fin.is_open())
	{
		Reader reader;
		Value jvalue;
		if (reader.parse(fin, jvalue, false))
		{
			Value& frames = jvalue["frames"];
			int iisize = frames.size();
			if (iisize == 0)
			{
				return false;
			}
			for (int iix = 0; iix < iisize; iix++)
			{
				Value& jfm_unit = frames[iix];
				Value& frame = jfm_unit["frame"];
				Value& filename = jfm_unit["filename"];
				bool rotated = jfm_unit["rotated"].asBool();
				rtlist.vtexture_coordinates.emplace_back();
				res_texture_coordinate& res_txt_cd = rtlist.vtexture_coordinates[iix];
				res_txt_cd._file_name = filename.asString();
				/*rtlist.file_name_sets += sfilename;
				rtlist.file_name_sets += "\0";*/
					res_txt_cd._x0 = frame["x"].asInt();
					res_txt_cd._y0 = frame["y"].asInt();
					res_txt_cd._x1 = frame["x"].asInt() + frame["w"].asInt() ;
					res_txt_cd._y1 = frame["y"].asInt() + frame["h"].asInt() ;

				if (rotated)
				{
					res_txt_cd._x1 = frame["x"].asInt()+ frame["h"].asInt();  ;
					res_txt_cd._y1 = frame["y"].asInt() + frame["w"].asInt() ;
				}
				
			}
		}
		fin.close();
	}
	else
		return false;
	return true;
}

void load_internal_texture_res(mtxt_internal& mptxt, unsigned int txtresid, unsigned int txtformatid)
{
	//MAKEINTRESOURCE
	HRSRC hrTxt = FindResource(NULL, MAKEINTRESOURCE(txtresid), "PNG");
	if (NULL==hrTxt)
	{
		printf("fail to get internal txture resource!\n");
		return;
	}
	DWORD dwSize = SizeofResource(NULL, hrTxt);
	if (dwSize==0)
	{
		printf("this size of internal texture resource is zero!\n");
		return;
	}
	HGLOBAL htxtGlobal = LoadResource(NULL, hrTxt);
	LPVOID pbuffer = LockResource(htxtGlobal);
	g_txt_id_intl = TextureHelper::transferMemory2Texture((unsigned char*)pbuffer,dwSize, g_txt_width_intl, g_txt_height_intl, \
		GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA);
	
	HRSRC hrData = FindResource(NULL, MAKEINTRESOURCE(txtformatid), "dataformat");
	if (NULL == hrData)
	{
		printf("fail to get internal txture resource!\n");
		return;
	}
	DWORD dwFSize = SizeofResource(NULL, hrData);
	if (dwFSize == 0)
	{
		printf("this size of internal texture resource is zero!\n");
		return;
	}
	HGLOBAL hdataGb = LoadResource(NULL, hrData);
	LPVOID pdtBuffer = LockResource(hdataGb);
	if (pdtBuffer)
	{
		string jdoc((char*)pdtBuffer);
		Reader reader;
		Value jvalue;
		if (reader.parse(jdoc, jvalue, false))
		{
			Value& frames = jvalue["frames"];
			int iisize = frames.size();
			for (int iix = 0; iix < iisize; iix++)
			{
				Value& jfm_unit = frames[iix];
				Value& frame = jfm_unit["frame"];
				Value& filename = jfm_unit["filename"];
				string cname = filename.asString();
				cname = cname.substr(0, cname.find_last_of('.'));
				mptxt[cname]._x0 = frame["x"].asInt();
				mptxt[cname]._y0 = frame["y"].asInt();
				mptxt[cname]._x1 = frame["x"].asInt() + frame["w"].asInt();
				mptxt[cname]._y1 = frame["y"].asInt() + frame["h"].asInt();
			}
		}
	}
}
