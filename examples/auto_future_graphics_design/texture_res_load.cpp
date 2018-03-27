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
extern string g_cureent_project_file_path;
void texture_res_load::load_res_from_json(Value& jroot)
{
	Value& texture_res_list = jroot["texture_res_list"];
	int isize = texture_res_list.size();
	for (int ix = 0; ix < isize;ix++)
	{
		Value& junit = texture_res_list[ix];
		Value& texture_pack_file = junit["texture_pack_file"];
		Value& texture_data_file = junit["texture_data_file"];
		_texture_res_tar.push_back(res_texture_list());
		string str_texture_pack_file = g_cureent_project_file_path.substr(0,g_cureent_project_file_path.find_last_of('\\')+1);
		string str_texture_data_file = str_texture_pack_file;
		str_texture_pack_file += texture_pack_file.asString();
		str_texture_data_file += texture_data_file.asString();
		res_texture_list& rtlist = _texture_res_tar[ix];
		rtlist.texture_id = TextureHelper::load2DTexture(str_texture_pack_file.c_str(), \
			rtlist.texture_width, rtlist.texture_height, GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA);
		ifstream fin;
		fin.open(str_texture_data_file);
		if (fin.is_open())
		{
			Reader reader;
			Value jvalue;
			if (reader.parse(fin,jvalue,false))
			{
				Value& frames = jvalue["frames"];
				int iisize = frames.size();
				for (int iix = 0; iix < iisize;iix++)
				{
					Value& jfm_unit = frames[iix];
					Value& frame = jfm_unit["frame"];
					Value& filename = jfm_unit["filename"];
					rtlist.vtexture_coordinates.push_back(res_texture_coordinate());
					res_texture_coordinate& res_txt_cd = rtlist.vtexture_coordinates[iix];
					res_txt_cd._file_name = filename.asString();
					res_txt_cd._x0 = frame["x"].asInt();
					res_txt_cd._y0 = frame["y"].asInt();
					res_txt_cd._x1 = frame["x"].asInt()+frame["w"].asInt();
					res_txt_cd._y1 = frame["y"].asInt()+frame["h"].asInt();

				}
			}
		}
	}
}

void load_internal_texture_res(mtxt_internal& mptxt, const char* texture_path, const char* texture_dataformat_path)
{
	g_txt_id_intl = TextureHelper::load2DTexture(texture_path, g_txt_width_intl, g_txt_height_intl, \
		GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA);
	ifstream fin;
	fin.open(texture_dataformat_path);
	if (fin.is_open())
	{
		Reader reader;
		Value jvalue;
		if (reader.parse(fin, jvalue, false))
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
