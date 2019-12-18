#include "ft_textblock.h"
#include "common_functions.h"
#include <chrono>

namespace auto_future
{
	using namespace chrono;
	static bool get_font_item(void* data, int idx, const char** out_str)
	{
		auto& ft_nm_list = g_pfont_face_manager->get_dic_fonts();
		*out_str = ft_nm_list[idx]->_name.c_str();
		return true;
	}
	static const char* omit_show[en_omit_type_number] =
	{
		"no omit",
		"omit rest",
		"omit rest with ellipsis",
	};
	ft_textblock::ft_textblock()
		: _txt_area(0.f, 0.f, 0.f, 0.f)
	{
		//_pfont_res_set = make_shared<af_font_res_set>(*g_pfont_face_manager);
		memset(_txt_pt._content, 0, MAX_CONTENT_LEN);
		_txt_pt._txt_clr = { 1.f, 1.f, 1.f };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_txt_pt, 6, [this](void*){
			ImGui::Combo("omit:", &_txt_pt._omit_rest, omit_show, en_omit_type_number);
		});
		reg_property_handle(&_txt_pt, 9, [this](void*){
			auto& ft_nm_list = g_pfont_face_manager->get_dic_fonts();
			
			if(ImGui::Combo("font:", &_txt_pt._font_id, &get_font_item, 0, ft_nm_list.size()))
			{
				_pfont_unit = ft_nm_list[_txt_pt._font_id];
			}
		});
		reg_property_handle(&_txt_pt,10, [this](void*){
			ImGui::SliderInt("Font size", &_txt_pt._font_size, 8, 60); 
		});
		reg_property_handle(&_txt_pt,11, [this](void*){
			ImGui::DragFloat("Font scale", &_txt_pt._font_scale, 0.005f, 1.f, 10.0f, "%.1f");   // Scale only this font
			
		});
#endif
	}
	void ft_textblock::link()
	{
		auto& ft_nm_list = g_pfont_face_manager->get_dic_fonts();
		if (_txt_pt._font_id>=ft_nm_list.size())
		{
			_txt_pt._font_id = 0;
		}
		_pfont_unit = ft_nm_list[_txt_pt._font_id];
	}
	void ft_textblock::draw()
	{
	       if (!_pfont_unit)
		{
			return;
		}
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 dpos = abpos + winpos;
		auto& ft_nm_list = g_pfont_face_manager->get_dic_fonts();
		auto font_cnt = ft_nm_list.size();
		if (font_cnt==0)
		{
			return;
		}
		if (_txt_pt._font_id>=font_cnt)
		{
			_txt_pt._font_id = 0;
		}
		//string font_name = ft_nm_list[_txt_pt._font_id]->_name;
		float font_scale = _txt_pt._font_scale;

		const ImVec2 ctnt_size = _txt_area.Max - _txt_area.Min;
		dpos.x = dpos.x - ctnt_size.x*_txt_pt._txt_alignh_nml;
		dpos.y = dpos.y - ctnt_size.y*_txt_pt._txt_alignv_nml;
		af_vec2 draw_pos{ dpos.x, dpos.y };
		af_vec2 end_pos;
		wstring draw_content = utf8ToWstring(_txt_pt._content);
		bool be_new = false;
		if (draw_content != _str_bk)
		{
			be_new = true;
			_str_bk = draw_content;
		}
		auto str_sz = draw_content.size();
		float width = screenw*2;
		if (_txt_pt._width_limit)
		{
			width = _txt_pt._width;
		}
		if (str_sz > 0)
		{
			static int start_id = 0;
			static steady_clock::time_point  lastTime;
			if (be_new)
			{
				 g_pfont_face_manager->draw_wstring(_pfont_unit, _txt_pt._font_size, draw_pos, end_pos, _txt_pt._font_scale, draw_content, _txt_pt._txt_clr, width, _txt_pt._omit_rest, true);
				//real_size = end_pos - draw_pos;
				_txt_area.Min = dpos;
				_txt_area.Max = { end_pos.x, end_pos.y };
				start_id = 0;
				lastTime = steady_clock::now();
				/*const ImVec2 ctnt_size = _txt_area.Max - _txt_area.Min;
				dpos.x = dpos.x - ctnt_size.x*_txt_pt._txt_alignh_nml;
				dpos.y = dpos.y - ctnt_size.y*_txt_pt._txt_alignv_nml;
				draw_pos = { dpos.x, dpos.y };*/
			}
			if (_txt_pt._playing)
			{
				wstring sub_content = draw_content.substr(start_id);
				auto cnt_content= sub_content.size();
				auto cnt_draw_char = g_pfont_face_manager->draw_wstring(_pfont_unit, _txt_pt._font_size, draw_pos, end_pos, _txt_pt._font_scale, sub_content, _txt_pt._txt_clr, width, _txt_pt._omit_rest, false);
				auto currentTime = steady_clock::now();
				int delta = chrono::duration_cast<chrono::duration<int, std::milli>>(currentTime - lastTime).count();
				auto circle_time = delta * 0.001f;
				if (circle_time>_txt_pt._play_circle)
				{
					start_id++;
					if (cnt_content == cnt_draw_char)
						start_id = 0;
					lastTime = currentTime;
				}
			}
			else
			{
				g_pfont_face_manager->draw_wstring(_pfont_unit, _txt_pt._font_size, draw_pos, end_pos, _txt_pt._font_scale, draw_content, _txt_pt._txt_clr, width, _txt_pt._omit_rest, false);
			}
		}
		af_vec2 real_size = end_pos - draw_pos;
		_txt_area.Min = dpos;
		_txt_area.Max = {end_pos.x,end_pos.y};
		//ft_base::draw();

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		if (is_selected())
		{
			ImVec2 pos1 = dpos;
			ImVec2 pos2 = { pos1.x, pos1.y + real_size.y };
			ImVec2 pos3 = { pos1.x + real_size.x, pos1.y + real_size.y };
			ImVec2 pos4 = { pos1.x + real_size.x, pos1.y };

			ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
			ImVec2 editunit(edit_unit_len, edit_unit_len);
			ImVec2 pos1a = pos1 - editunit;
			ImVec2 pos1b = pos1 + editunit;
			ImGui::RenderFrame(pos1a, pos1b, col);
			ImVec2 pos2a = pos2 - editunit;
			ImVec2 pos2b = pos2 + editunit;
			ImGui::RenderFrame(pos2a, pos2b, col);
			ImVec2 pos3a = pos3 - editunit;
			ImVec2 pos3b = pos3 + editunit;
			ImGui::RenderFrame(pos3a, pos3b, col);
			ImVec2 pos4a = pos4 - editunit;
			ImVec2 pos4b = pos4 + editunit;
			ImGui::RenderFrame(pos4a, pos4b, col);
			
		}
#endif
	}
	bool ft_textblock::contains(float posx, float posy)
	{
		ImVec2 mouse_pos(posx, posy);
		bool be_contain = _txt_area.Contains(mouse_pos);
		return be_contain;
	}
	bool ft_textblock::relative_contain(af_vec2& point)
	{
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 base_pt = abpos + winpos;
		ImVec2 tar{ point.x, point.y };
		ImVec2 ctnt_size = _txt_area.Max - _txt_area.Min;
		ImVec2 origin_pt = _txt_area.Min - base_pt;
		ImVec2 omax = origin_pt + ctnt_size;
		ImRect rl_cover_area(origin_pt, omax);
		bool be_contain = rl_cover_area.Contains(tar);
		return be_contain;
	}
	bool ft_textblock::relative_contain(float pos, bool be_h)
	{
		ImVec2 bs_pos = base_pos();
		const ImVec2 ctnt_size = _txt_area.Max - _txt_area.Min;
		bool be_contain;
		if (be_h)
		{
			float orgx=- ctnt_size.x*_txt_pt._txt_alignh_nml;
			float bdx = orgx + ctnt_size.x;
			be_contain = pos >= orgx&&pos <= bdx;
		}
		else
		{
			float orgy=- ctnt_size.y*_txt_pt._txt_alignv_nml;
			float bdy = orgy + ctnt_size.y;
			be_contain = pos >= orgy&&pos <= bdy;
		}
		return be_contain;
	}
}
