#pragma once
/*
1、get char encode format of font face
2、get glyph index by the code of char unit
3、generate bitmap of glyphs 
*/
#include <GLFW/glfw3.h>
#include <GL/glcorearb.h>
#include <map>
#include <vector>
#include <string>
#include <assert.h>
#include <ft2build.h>
#include <memory>
#define _CHECK_TIME_CONSUME
#ifdef _CHECK_TIME_CONSUME
#include <chrono>
#endif
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
		af_vi2 _size;
		af_vi2 _bearing;
		GLint _advance;
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

	struct txt_font_repository
	{
		GLuint _txt_id{ 0 };
		af_vi2 _txt_size;
		af_vui2 _border;
		GLuint _font_size{ 0 };
		bool _be_full{ false };
		dic_glyph_txt _dic_txt_cd;
	};
	using dic_font_rep = map<int, txt_font_repository>;

	static void convert_r_to_rgba(uint8_t* pred, uint32_t*prgba, uint32_t data_len)
	{
		for (size_t i = 0; i < data_len; i++)
		{
			//*prgba++ = IM_COL32(255, 255, 255, (unsigned int)(*pred++));
			uint8_t* pdst = (uint8_t*)(prgba);
			pdst += 3;
			*pdst = *pred++;
			prgba++;

		}
	}

	class font_face_manager
	{
		FT_Library _ft;
		dic_font_face _dic_face;
		dic_font_rep _dic_frep;
		vfont_face_name _font_face_names;
		GLuint _fmbf_id {0};
		/*void clear_texture(GLuint& txt_id)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fmbf_id);
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, txt_id, 0); //Only need to do this once.
			glDrawBuffer(GL_COLOR_ATTACHMENT0); //Only need to do this once.
			GLuint clearColor[4] = { 0, 0, 0, 0 };
			glClearBufferuiv(GL_COLOR, 0, clearColor);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}*/
		void init_txt_font_repository(GLint fontSize, txt_font_repository& newFrep)
		{
			newFrep._border = { 0, 0 };
			newFrep._font_size = fontSize;
			newFrep._txt_size = { fontSize * 32, fontSize * 32 };
			glGenTextures(1, &newFrep._txt_id);
			glBindTexture(GL_TEXTURE_2D, newFrep._txt_id);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newFrep._txt_size.x, newFrep._txt_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		void load_chars(string&fontFaceName, txt_font_repository&fp, wstring& wchar_list, GLuint& max_bearingy)
		{
			dic_glyph_txt& container = fp._dic_txt_cd;
			GLuint& txtid = fp._txt_id;
			assert(txtid&&"you must pass a valid texture id into the function load_chars!");
			af_vui2& border = fp._border;
			af_vi2& txt_size = fp._txt_size;
			GLuint& fontSize = fp._font_size;
			bool& be_full = fp._be_full;
			const auto& face_it = _dic_face.find(fontFaceName);
			if (face_it == _dic_face.end()){
				printf("fail to load chars from unknown font face %d!\n", fontFaceName.c_str());
				return;
			}
			glBindTexture(GL_TEXTURE_2D,txtid);
			auto& face = face_it->second;
			FT_Set_Pixel_Sizes(face, 0, fontSize);
			for (auto& str_it:wchar_list)
			{
				const auto& it_glyph = container.find(str_it);
				if (it_glyph!=container.end())
				{
					auto& ch_glhph = it_glyph->second;
					auto&by= ch_glhph._bearing.y;
					if (by>max_bearingy)
					{
						max_bearingy = by;
					}
					continue;
				}

				if (FT_Load_Char(face, str_it, FT_LOAD_RENDER))
				{
					printf("fail to find %c in font face %s\n", str_it, fontFaceName.c_str());
					continue;
				}
				auto tw = face->glyph->bitmap.width;
				auto th = face->glyph->bitmap.rows;
				auto lt = face->glyph->bitmap_left;
				auto tp = face->glyph->bitmap_top;
				if (tp>max_bearingy)
				{
					max_bearingy = tp;
				}
				auto ad = face->glyph->advance.x;
				float x0 = (float)border.x /(float)txt_size.x;
				float y0 = (float)border.y /(float)txt_size.y;

				const auto& tbuff = face->glyph->bitmap.buffer;
				auto txt_sz = tw*th*4;
				uint32_t* prgba = new uint32_t[txt_sz];
				memset(prgba, 0xff, txt_sz);
				convert_r_to_rgba(tbuff, prgba, txt_sz);

				float left_x = border.x + tw;
				float next_x = left_x + 2;
				if (next_x<=txt_size.x)
				{
					glTexSubImage2D(GL_TEXTURE_2D, 0, border.x, border.y, tw, th, GL_RGBA, GL_UNSIGNED_BYTE, prgba);
					border.x =next_x;
				}
				else
				{
					border.y += fontSize;
					border.y+=2;
					if ((border.y + th)>txt_size.y)// is full
					{
						fprintf(stderr, "font glyph repository of control is full!\n");
						be_full = true;
						delete[] prgba;
						return;
					}
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, border.y, tw, th, GL_RGBA, GL_UNSIGNED_BYTE, prgba);
					border.x = tw+2;
				}
				float x1 = (float)left_x / (float)txt_size.x;
				float y1 = (float)(th+border.y) / (float)txt_size.y;
				//border.x += 5;
				delete[] prgba;

				txt_coordinate txt_unit = {
					{tw,th},
					{lt,tp},
					ad,
					x0,y0,x1,y1
				};
				container[str_it] = txt_unit;
			}
		}
	public:
		font_face_manager()
		{
			//auto lastTime = std::chrono::high_resolution_clock::now();
			assert(!FT_Init_FreeType(&_ft) && "fail to init freetype library!");
			glGenFramebuffers(1, &_fmbf_id);
			//auto currentTime = std::chrono::high_resolution_clock::now();
			//int delta = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(currentTime - lastTime).count();
			//printf("FT_Init_FreeType consume %d milli secs\n", delta);

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
			glDeleteFramebuffers(1, &_fmbf_id);
		}
		
		vfont_face_name& get_font_name_list()
		{
			return _font_face_names;
		}
		bool load_font(string& fontFaceName, uint8_t* pfont_buff, unsigned long file_size)
		{
			const auto& fitem = _dic_face.find(fontFaceName);
			if (fitem != _dic_face.end())
			{
				printf("font %s have been loaded!\n", fontFaceName.c_str());
				return false;
			}
			FT_Face face;
			FT_Error err = FT_New_Memory_Face(_ft, pfont_buff, file_size, 0, &face);
			if (err)
			{
				printf("fail to load font from:%s!\n", fontFaceName.c_str());
				return false;
			}
			FT_Select_Charmap(face, FT_ENCODING_UNICODE);
			_dic_face[fontFaceName] = face;
			_font_face_names.emplace_back(fontFaceName);
			return true;
		}
		bool load_font(string& fontFaceName, string& fontPath)
		{
			//auto lastTime = std::chrono::high_resolution_clock::now();
			const auto& fitem = _dic_face.find(fontFaceName);
			if (fitem != _dic_face.end())
			{
				printf("font %s have been loaded!\n", fontFaceName.c_str());
				return false;
			}
			FT_Face face;
			if (FT_New_Face(_ft, fontPath.c_str(), 0, &face))
			{
				printf("fail to load font from:%s!\n", fontPath.c_str());
				return false;
			}
			FT_Select_Charmap(face, FT_ENCODING_UNICODE);
			_dic_face[fontFaceName] = face;
			_font_face_names.emplace_back(fontFaceName);
			//auto currentTime = std::chrono::high_resolution_clock::now();
			//int delta = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(currentTime - lastTime).count();
			//printf("load_font:%s consume %d milli secs\n", fontFaceName.c_str(), delta);
			return true;
		}

	
		void draw_wstring(string& fontFace, GLint fontSize, af_vec2& start_pos, af_vec2& end_pos, GLfloat scale, wstring& str_content, af_vec3& txt_col, float width, bool omit_rest);
	};

	extern shared_ptr<font_face_manager> g_pfont_face_manager;

	//struct frame_buff_unit
	//{
	//	GLuint _fb_id{ 0 };
	//	GLuint _col_txt_id{ 0 };
	//	GLuint _depth_stencil_txt_id{ 0 };
	//	af_vi2 _size;
	//};

	/*class af_font_res_set
	{
	font_face_manager& _font_mg;
	txt_font_repository _font_rp;
	dic_glyph_txt _dic_gly_txtc;
	bool _is_full{ false };
	public:
	af_font_res_set(font_face_manager& font_mg);
	~af_font_res_set();
	void draw_wstring(string& fontFace, GLuint fontSize, af_vec2& start_pos, af_vec2& end_pos,GLfloat scale, wstring& str_content,af_vec3& txt_col,float width,bool omit_rest);
	};*/

}