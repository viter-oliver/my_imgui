#include "af_font_res_set.h"
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
namespace auto_future
{
	font_face_manager g_font_face_manager;
	af_font_res_set::af_font_res_set(font_face_manager& font_mg)
		:_font_mg(font_mg)
	{
		_txt_size = { 512, 512 };
		_border = { 0, 0 };
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &_txt_id);
		glBindTexture(GL_TEXTURE_2D, _txt_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _txt_size.x, _txt_size.y, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	af_font_res_set::~af_font_res_set()
	{
		glDeleteTextures(1, &_txt_id);
	}
	using namespace ImGui;
	void af_font_res_set::draw_wstring(string& fontFace, GLuint fontSize, \
		af_vec2& start_pos, GLfloat scale, wstring& str_content, af_vec3& txt_col)
	{
		if (fontSize!=_font_size)//!texture will be rebuilt
		{
			_font_size = fontSize;
		}
		_font_mg.load_chars(fontFace, _font_size, _txt_id, str_content, _dic_gly_txtc, _border, _txt_size);
		af_vec2 draw_pos = start_pos;
		for (auto& wstr_item:str_content)
		{
			auto& glyph_txt_it = _dic_gly_txtc.find(wstr_item);
			if (glyph_txt_it != _dic_gly_txtc.end())
			{
				auto& glyph_txt_cd = glyph_txt_it->second;
				auto& bearing = glyph_txt_cd._bearing;
				auto tsize = glyph_txt_cd._size;
				float x0 = glyph_txt_cd._x0;
				float x1 = glyph_txt_cd._x1;
				float y0 = glyph_txt_cd._y0;
				float y1 = glyph_txt_cd._y1;
				auto advance = glyph_txt_cd._advance;
				ImVec2 pos0{ draw_pos.x + bearing.x*scale, draw_pos.y - (tsize.y-bearing.y)*scale };
				ImVec2 pos1{ pos0.x, pos0.y + tsize.y*scale };
				ImVec2 pos2{ pos0.x + tsize.x*scale, pos1.y };
				ImVec2 pos3{ pos2.x, pos0.y };
				ImVec2 uv0{ x0, y0 };
				ImVec2 uv1{ x0, y1 };
				ImVec2 uv2{ x1, y1 };
				ImVec2 uv3{ x1, y0 };
				ImVec4 dcol{ txt_col.x, txt_col.y, txt_col.z, 1.f };
				ImageQuad((ImTextureID)_txt_id, pos0, pos1, pos2, pos3, uv0, uv1, uv2, uv3, dcol);
				draw_pos.x += (advance >> 6)*scale;// Bitshift by 6 to get value in pixels (2^6 = 64)

			}
		}
	}

}