#include "af_font_res_set.h"
#include "common_functions.h"
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

namespace auto_future
{
	const GLchar* font_shd_vt_code = R"(#version 330
attribute vec4 vertext;
uniform mat4 proj;
varying vec2 TextCoord;
void main()
{
	gl_Position = proj * vec4(vertext.xy, 0.0, 1.0);
	TextCoord = vertext.zw;
}
)";
	const GLchar* font_shd_fg_code = R"(#version 330
uniform vec3 textColor;
uniform sampler2D fontText;
varying vec2 TextCoord;
void main()
{   
   vec4 sample = vec4(1.0f,1.0f,1.0f,texture(fontText, TextCoord).r);
   gl_FragColor = vec4(textColor, 1.0f) * sample;
}
)";
	font_face_manager::font_face_manager()
	{
		assert(!FT_Init_FreeType(&_ft) && "fail to init freetype library!");
		_font_shd = make_shared<af_shader>(font_shd_vt_code, font_shd_fg_code);
		//_font_shd->vertex_att_pointer("vertext");
		glGenBuffers(1, &_font_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _font_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 4 * _char_max_cnt, NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	}
	void font_face_manager::use_font_vbo()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _font_vbo);
		_font_shd->vertex_att_pointer({ "vertext" });
		//glEnableVertexAttribArray(0);
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	}
	void font_face_manager::un_use_vbo()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	shared_ptr<font_face_manager> g_pfont_face_manager;

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

		_fm_buff._size = { 512, 512 };
		prepareFBO1(_fm_buff._col_txt_id, _fm_buff._depth_stencil_txt_id, _fm_buff._fb_id, _fm_buff._size.x, _fm_buff._size.y);
	}

	af_font_res_set::~af_font_res_set()
	{
		glDeleteTextures(1, &_txt_id);
		GLuint& fb_id = _fm_buff._fb_id;
		GLuint& col_txt_id = _fm_buff._col_txt_id;
		GLuint& depth_txt_id = _fm_buff._depth_stencil_txt_id;
		glDeleteFramebuffers(1, &fb_id);
		glDeleteTextures(1, &col_txt_id);
		glDeleteTextures(1, &depth_txt_id);
	}

	using namespace ImGui;

	void af_font_res_set::draw_wstring(string& fontFace, GLuint fontSize, af_vec2& start_pos, \
		af_vec2& end_pos, GLfloat scale, wstring& str_content, af_vec3& txt_col)
	{
		GLuint& fb_id = _fm_buff._fb_id;
		GLuint& col_txt_id = _fm_buff._col_txt_id;
		GLuint& depth_txt_id = _fm_buff._depth_stencil_txt_id;
		af_vi2& vsize = _fm_buff._size;
		GLint last_vp[4];
		glGetIntegerv(GL_VIEWPORT, last_vp);
		glBindFramebuffer(GL_FRAMEBUFFER, fb_id);
		glViewport(0, 0, vsize.x, vsize.y);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);
		glClearColor(1.f, 1.f, 1.f, 0.2f);
		glClear(GL_COLOR_BUFFER_BIT);
		auto& font_shd = _font_mg.font_shader();
		font_shd.use();
		font_shd.uniform("textColor", (float*)&txt_col);
		float ortho_proj[4][4] =
		{
			{ 2.0f / _fm_buff._size.x, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 2.0f / -_fm_buff._size.y, 0.0f, 0.0f },
			{ 0.0f, 0.0f, -1.0f, 0.0f },
			{ -1.0f, 1.0f, 0.0f, 1.0f },
		};
		font_shd.uniform("proj", &ortho_proj[0][0]);
		if (fontSize != _font_size)//!texture will be rebuilt
		{
			_font_size = fontSize;
		}
		_font_mg.load_chars(fontFace, _font_size, _txt_id, str_content, _dic_gly_txtc, _border, _txt_size);
		auto cnt_chars = str_content.size();
		float* ppos = new float[cnt_chars * 4 * 4];

		end_pos = { 0.f, 10.f };//start_pos
		float maxy = 0.f, miny = 10.f;
		int char_index = 0;
		for (auto& wstr_item : str_content)
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
				ImVec2 pos0{ end_pos.x + bearing.x*scale, end_pos.y - (tsize.y - bearing.y)*scale };
				ImVec2 pos1{ pos0.x, pos0.y + tsize.y*scale };
				ImVec2 pos2{ pos0.x + tsize.x*scale, pos1.y };
				ImVec2 pos3{ pos2.x, pos0.y };
				/*ImVec2 uv0{ x0, y0 };
				ImVec2 uv1{ x0, y1 };
				ImVec2 uv2{ x1, y1 };
				ImVec2 uv3{ x1, y0 };
				ImVec4 dcol{ txt_col.x, txt_col.y, txt_col.z, 1.f };*/
				float* pvtx = ppos + (char_index * 16);
				pvtx[0] = pos0.x;
				pvtx[1] = pos0.y;
				pvtx[2] = x0;
				pvtx[3] = y0;
				pvtx[4] = pos1.x;
				pvtx[5] = pos1.y;
				pvtx[6] = x0;
				pvtx[7] = y1;
				pvtx[8] = pos3.x;
				pvtx[9] = pos3.y;
				pvtx[10] = x1;
				pvtx[11] = y0;
				pvtx[12] = pos2.x;
				pvtx[13] = pos2.y;
				pvtx[14] = x1;
				pvtx[15] = y1;
				char_index++;

				//ImageQuad((ImTextureID)_txt_id, pos0, pos1, pos2, pos3, uv0, uv1, uv2, uv3, dcol);
				end_pos.x += (advance >> 6)*scale;// Bitshift by 6 to get value in pixels (2^6 = 64)
				if (maxy < pos1.y)
				{
					maxy = pos1.y;
				}
				if (miny > pos0.y)
				{
					miny = pos0.y;
				}
			}
		}
		float umin = 0;
		float vmin = 1 - miny / (float)vsize.y;
		float umx = end_pos.x / (float)vsize.x;
		float vmx = 1 - maxy / (float)vsize.y;

		end_pos.x += start_pos.x;
		end_pos.y = start_pos.y + (maxy - miny);
		_font_mg.use_font_vbo();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * 16 * char_index, ppos);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _txt_id);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 * char_index);
		delete[] ppos;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(last_vp[0], last_vp[1], (GLsizei)last_vp[2], (GLsizei)last_vp[3]);
		
		end_pos = { start_pos.x + 512.f, start_pos.y + 512.f };
		umin = 0; umx = 1;
		vmin = 0; vmx = 1;

		ImVec2 pos0{ start_pos.x, start_pos.y};
		ImVec2 pos1{ start_pos.x, end_pos.y };
		ImVec2 pos2{ end_pos.x, end_pos.y };
		ImVec2 pos3{ end_pos.x, start_pos.y };
		ImVec2 uv0{ umin, vmin };
		ImVec2 uv1{ umin, vmx };
		ImVec2 uv2{ umx, vmx };
		ImVec2 uv3{ umx, vmin};
		ImGui::ImageQuad((ImTextureID)col_txt_id, pos0, pos1, pos2, pos3, uv0, uv1, uv2, uv3);
	}
}