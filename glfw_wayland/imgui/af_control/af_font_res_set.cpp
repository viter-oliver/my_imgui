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

	int font_face_manager::draw_wstring(ps_font_unit& pf_u, GLint fontSize,
                                          af_vec2& start_pos, af_vec2& end_pos, 
                                          GLfloat scale, wstring& str_content, 
                                          const af_vec4& txt_col, float width, 
                                          int omit_rest,bool be_new)
	{
		//if (fontSize != _font_rp._font_size)//!texture will be rebuilt
		//{
		//	_dic_gly_txtc.clear();
		//	_font_rp._be_full = false;
		//	_font_rp._border = { 0, 0 };
		//	_font_mg.clear_texture(_font_rp._txt_id);
		//	_font_rp._font_size = fontSize;
		//}

		txt_font_repository* pfrp = nullptr;
		GLint max_beary = 0;
		//auto& ifont = _dic_fonts.find(fontFace);
		auto& f_u = *pf_u;
		const auto& irep = f_u._ft_rep.find(fontSize);
		if (irep != f_u._ft_rep.end())
		{
			pfrp = &irep->second;
		}
		else
		{
			f_u._ft_rep[fontSize]._txt_size = { f_u._char_count_r*fontSize, f_u._char_count_c*fontSize };
			init_txt_font_repository(fontSize, f_u._ft_rep[fontSize]);
			pfrp = &f_u._ft_rep[fontSize];
		}
		if (!pfrp->_be_full)
		{
			load_chars(f_u._ft_face, *pfrp, str_content);
		}
                max_beary = pfrp->_max_bearingy;
		bool be_break = str_content[0] == L'O'&&str_content[1] == L'S';
		
		end_pos = start_pos;
		float base_line = start_pos.y + (float)max_beary;
		float str_most_right_edge = start_pos.x + width;
		float str_real_right_edg = start_pos.x;
		float maxy = 0.f;
		GLuint& txt_id = pfrp->_txt_id;
		dic_glyph_txt& txt_cd_container = pfrp->_dic_txt_cd;
		bool will_omit_test = false;
		int cnt_char = 0;
		int cnt_char_w = str_content.size();
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
				auto bearing_x = bearing.x*scale;
#if 0				
				auto bearing_x_n = bearing_x;

                    int w = tsize.x;
                    int ev = w % 2;
                    if (ev==0)
                    {
                         bearing_x_n--;
                    }
#endif
				float char_left_edge = end_pos.x + bearing_x;
				float char_right_edge =char_left_edge + tsize.x*scale;//+bearing_x_n;
				cnt_char++;
				if (char_right_edge>str_most_right_edge)
				{

					if (omit_rest != en_no_omit)
					{
						//if (cnt_char < cnt_char)
						{
							wstring omit_sign ={0x2026};
							if(omit_rest == en_omit_rest )
								omit_sign= L" " ;
							load_chars(pf_u->_ft_face, *pfrp, omit_sign);
							const auto& glyph_omit = txt_cd_container.find(omit_sign[0]);
							auto& glyph_omit_txt_cd = glyph_omit->second;
							bearing = glyph_omit_txt_cd._bearing;
							tsize = glyph_omit_txt_cd._size;
							x0 = glyph_omit_txt_cd._x0;
							x1 = glyph_omit_txt_cd._x1;
							y0 = glyph_omit_txt_cd._y0;
							y1 = glyph_omit_txt_cd._y1;
							advance = glyph_omit_txt_cd._advance;
							char_left_edge = end_pos.x + bearing.x*scale;
							char_right_edge = char_left_edge + tsize.x*scale;
						}
						
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
				ImVec2 pos0{ char_left_edge, base_line - bearing.y*scale };
				ImVec2 pos1{ pos0.x, pos0.y + tsize.y*scale };
				ImVec2 pos2{ char_right_edge, pos1.y };
				ImVec2 pos3{ pos2.x, pos0.y };
				ImVec2 uv0{ x0, y0 };
				ImVec2 uv1{ x0, y1 };
				ImVec2 uv2{ x1, y1 };
				ImVec2 uv3{ x1, y0 };
				ImVec4 dcol{ txt_col.x, txt_col.y, txt_col.z, txt_col.w };
				if (!be_new)
				ImageQuad((ImTextureID)txt_id, pos0, pos1, pos2, pos3, uv0, uv1, uv2, uv3, dcol);
				float shift_dis = (advance >> 6)*scale;// Bitshift by 6 to get value in pixels (2^6 = 64)
				//end_pos.x += bearing_x_n;
				end_pos.x += shift_dis;
				if( end_pos.x > str_real_right_edg )
				{
					str_real_right_edg = end_pos.x;
				}
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
		return cnt_char;
	}

}
