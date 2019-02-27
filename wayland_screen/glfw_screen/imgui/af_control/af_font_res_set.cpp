#include "af_font_res_set.h"
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
namespace auto_future
{
	shared_ptr<font_face_manager> g_pfont_face_manager;
	/*af_font_res_set::af_font_res_set(font_face_manager& font_mg)
		:_font_mg(font_mg)
	{
		_font_rp._txt_size = { 512, 512 };
		_font_rp._border = { 0, 0 };
		glGenTextures(1, &_font_rp._txt_id);
		glBindTexture(GL_TEXTURE_2D, _font_rp._txt_id);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _font_rp._txt_size.x, _font_rp._txt_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	af_font_res_set::~af_font_res_set()
	{
		glDeleteTextures(1, &_font_rp._txt_id);
	}
	*/
	using namespace ImGui;

	void font_face_manager::draw_wstring(string& fontFace, GLint fontSize, af_vec2& start_pos, af_vec2& end_pos, GLfloat scale, wstring& str_content, af_vec3& txt_col, float width, bool omit_rest)
	{
		//if (fontSize != _font_rp._font_size)//!texture will be rebuilt
		//{
		//	_dic_gly_txtc.clear();
		//	_font_rp._be_full = false;
		//	_font_rp._border = { 0, 0 };
		//	_font_mg.clear_texture(_font_rp._txt_id);
		//	_font_rp._font_size = fontSize;
		//}
		auto ifrp = _dic_frep.find(fontSize);
		txt_font_repository* pfrp = nullptr;
		if (ifrp==_dic_frep.end())
		{
			init_txt_font_repository(fontSize, _dic_frep[fontSize]);
			pfrp = &_dic_frep[fontSize];
		}
		else
		{
			pfrp =& ifrp->second;
		}
		GLuint max_beary = 0;
		
		if (!pfrp->_be_full)
		{
			load_chars(fontFace, *pfrp, str_content, max_beary);
		}
		end_pos = start_pos;
		float base_line = start_pos.y + (float)max_beary;
		float str_most_right_edge = start_pos.x + width;
		float str_real_right_edg = start_pos.x;
		float maxy = 0.f;
		GLuint& txt_id = pfrp->_txt_id;
		dic_glyph_txt& txt_cd_container = pfrp->_dic_txt_cd;
		bool will_omit_test = false;
		for (auto& wstr_item:str_content)
		{
			const auto& glyph_txt_it = txt_cd_container.find(wstr_item);
			if (glyph_txt_it != txt_cd_container.end())
			{
				auto& glyph_txt_cd = glyph_txt_it->second;
				auto bearing = glyph_txt_cd._bearing;
				auto tsize = glyph_txt_cd._size;
				float x0 = glyph_txt_cd._x0;
				float x1 = glyph_txt_cd._x1;
				float y0 = glyph_txt_cd._y0;
				float y1 = glyph_txt_cd._y1;
				auto advance = glyph_txt_cd._advance;
				float char_left_edge = end_pos.x + bearing.x*scale;
				float char_right_edge = char_left_edge + tsize.x*scale;
				if (char_right_edge>str_most_right_edge)
				{
					if (omit_rest)
					{
						wstring omit_sign = L"…";
						load_chars(fontFace, *pfrp, omit_sign, max_beary);
						const auto& glyph_omit = txt_cd_container.find(omit_sign[0]);
						const auto& glyph_omit_txt_cd = glyph_omit->second;
						bearing = glyph_omit_txt_cd._bearing;
						tsize = glyph_omit_txt_cd._size;
						x0 = glyph_omit_txt_cd._x0;
						x1 = glyph_omit_txt_cd._x1;
						y0 = glyph_omit_txt_cd._y0;
						y1 = glyph_omit_txt_cd._y1;
						advance = glyph_omit_txt_cd._advance;
						char_left_edge = end_pos.x + bearing.x*scale;
						char_right_edge = char_left_edge + tsize.x*scale;
						will_omit_test = true;
					}
					else
					{
						end_pos.x = start_pos.x;
						base_line = maxy + max_beary;
						char_left_edge = end_pos.x + bearing.x*scale;
						char_right_edge = char_left_edge + tsize.x*scale;
					}
				}
				if (char_right_edge>str_real_right_edg)
				{
					str_real_right_edg = char_right_edge;
				}
				ImVec2 pos0{ char_left_edge, base_line - bearing.y*scale };
				ImVec2 pos1{ pos0.x, pos0.y + tsize.y*scale };
				ImVec2 pos2{ char_right_edge, pos1.y };
				ImVec2 pos3{ pos2.x, pos0.y };
				ImVec2 uv0{ x0, y0 };
				ImVec2 uv1{ x0, y1 };
				ImVec2 uv2{ x1, y1 };
				ImVec2 uv3{ x1, y0 };
				ImVec4 dcol{ txt_col.x, txt_col.y, txt_col.z, 1.f };
				ImageQuad((ImTextureID)txt_id, pos0, pos1, pos2, pos3, uv0, uv1, uv2, uv3, dcol);
				float shift_dis = (advance >> 6)*scale;// Bitshift by 6 to get value in pixels (2^6 = 64)
				end_pos.x += shift_dis;
				if (maxy<pos1.y)
				{
					maxy = pos1.y;
				}
				if (will_omit_test)
				{
					break;
				}
			}
		}
		end_pos.x = str_real_right_edg;
		end_pos.y = maxy;
	}

}