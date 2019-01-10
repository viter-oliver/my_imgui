#pragma once
/*
1、get char encode format of font face
2、get glyph index by the code of char unit
3、generate bitmap of glyphs 
*/
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif 
#include <map>
#include <vector>
#include <string>
#include <assert.h>
#include <ft2build.h>
#include "af_type.h"
#include FT_FREETYPE_H
namespace auto_future
{
	using namespace std;
	using dic_font_face = map<string, FT_Face>;
	using vfont_face_name = vector<string>;
	/*!
	* \class txt_coordinate
	*
	* \brief coordinates of font unit
	* 
	* \memberof _size,_bearing,_advace,x0,y0,x1,y1
	*
	* \author Viter
	* \date 一月 2019
	*/
	struct txt_coordinate
	{
		/*! \member of txt_coordinate*/
		af_vui2 _size, _bearing;
		GLuint _advance;
		float _x0, _y0, _x1, _y1;
	};
	/*!
	* \class dic_glyph_txt
	*
	* \brief dictionary,by that you can find the coordinate through the code of char
	*
	* \author Viter
	* \date 一月 2019
	*/
	using dic_glyph_txt = map<wchar_t, txt_coordinate>;

	class font_face_manager
	{
		FT_Library _ft;
		dic_font_face _dic_face;
		vfont_face_name _font_face_names;
	public:
		font_face_manager()
		{
			assert(!FT_Init_FreeType(&_ft) && "fail to init freetype library!");
		}
		~font_face_manager()
		{
			for (auto& itm : _dic_face)
			{
				if (itm.second)
				{
					FT_Done_Face(itm.second);
				}
			}
		}
		vfont_face_name& get_font_name_list()
		{
			return _font_face_names;
		}
		void load_font(string& fontFaceName, string& fontPath)
		{
			auto& fitem = _dic_face.find(fontFaceName);
			if (fitem != _dic_face.end())
			{
				printf("font %s have been loaded!\n", fontFaceName.c_str());
				return;
			}
			FT_Face face;
			if (FT_New_Face(_ft, fontPath.c_str(), 0, &face))
			{
				printf("fail to load font from:%s!\n", fontPath.c_str());
				return;
			}
			FT_Select_Charmap(face, FT_ENCODING_UNICODE);
			_dic_face[fontFaceName] = face;
			_font_face_names.emplace_back(fontFaceName);
		}
		void load_chars(string&fontFaceName, GLuint fontSize, GLuint txtid, wstring& wchar_list, \
			dic_glyph_txt& container, af_vui2& border, af_vui2& txt_size)
		{
			assert(txtid&&"you must pass a valid texture id into the function load_chars!");
			auto& face_it = _dic_face.find(fontFaceName);
			if (face_it == _dic_face.end()){
				printf("fail to load chars from unknown font face %d!\n", fontFaceName.c_str());
				return;
			}
			glBindTexture(GL_TEXTURE_2D,txtid);
			auto& face = face_it->second;
			FT_Set_Pixel_Sizes(face, 0, fontSize);
			for (auto& str_it:wchar_list)
			{
				auto& it_glyph = container.find(str_it);
				if (it_glyph!=container.end())
				{
					continue;
				}
				if (FT_Load_Char(face, str_it, FT_LOAD_RENDER))
				{
					wprintf(L"fail to find %c in font face %s\n", str_it, fontFaceName.c_str());
					continue;
				}
				auto tw = face->glyph->bitmap.width;
				auto th = face->glyph->bitmap.rows;
				auto lt = face->glyph->bitmap_left;
				auto tp = face->glyph->bitmap_top;
				auto ad = face->glyph->advance.x;
				float x0 = (float)border.x /(float)txt_size.x;
				float y0 = (float)border.y /(float)txt_size.y;

				auto& tbuff = face->glyph->bitmap.buffer;
				glTexSubImage2D(GL_TEXTURE_2D, 0, border.x, border.y, tw, th, GL_RED, GL_UNSIGNED_BYTE, tbuff);				
				if ((border.x+tw)<=txt_size.x)
				{
					border.x += tw;
				}
				else
				{
					border.x = 0;
					border.y += fontSize;
				}
				float x1 = (float)border.x / (float)txt_size.x;
				float y1 = (float)(th+border.y) / (float)txt_size.y;
				

				txt_coordinate txt_unit = {
					{tw,th},
					{lt,tp},
					ad,
					x0,y0,x1,y1
				};
				container[str_it] = txt_unit;
			}
		}
	};

	extern font_face_manager g_font_face_manager;

	class af_font_res_set
	{
		font_face_manager& _font_mg;
		GLuint _txt_id{ 0 };
		af_vui2 _txt_size;
		af_vui2 _border;
		GLuint _font_size { 16 };
		dic_glyph_txt _dic_gly_txtc;
	public:
		af_font_res_set(font_face_manager& font_mg);
		~af_font_res_set();
		void draw_wstring(string& fontFace,GLuint fontSize,af_vec2& start_pos,\
			GLfloat scale, wstring& str_content,af_vec3& txt_col);
	};

}