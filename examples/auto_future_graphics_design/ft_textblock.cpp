#include "ft_textblock.h"
#include "common_functions.h"
namespace auto_future
{
	static bool get_font_item(void* data, int idx, const char** out_str)
	{
		vfont_face_name& ft_nm_list = g_pfont_face_manager->get_font_name_list();
		*out_str = ft_nm_list[idx].c_str();
		return true;
	}
	ft_textblock::ft_textblock()
		: _txt_area(0.f, 0.f, 0.f, 0.f)
	{
		//_pfont_res_set = make_shared<af_font_res_set>(*g_pfont_face_manager);
		memset(_txt_pt._content, 0, MAX_CONTENT_LEN);
		_txt_pt._txt_clr = { 1.f, 1.f, 1.f };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_txt_pt, 7, [this](void*){
			vfont_face_name& ft_nm_list = g_pfont_face_manager->get_font_name_list();
			if (_txt_pt._font_id >= ft_nm_list.size())
			{
				_txt_pt._font_id = 0;
			}
			ImGui::Combo("font:", &_txt_pt._font_id, &get_font_item, 0, ft_nm_list.size());
		});
		reg_property_handle(&_txt_pt,8, [this](void*){
			ImGui::SliderInt("Font size", &_txt_pt._font_size, 8, 42); 
		});
		reg_property_handle(&_txt_pt,9, [this](void*){
			ImGui::DragFloat("Font scale", &_txt_pt._font_scale, 0.005f, 1.f, 10.0f, "%.1f");   // Scale only this font
			
		});
#endif
	}
	void ft_textblock::draw()
	{
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 dpos = abpos + winpos;
		vfont_face_name& ft_nm_list = g_pfont_face_manager->get_font_name_list();
		string font_name = ft_nm_list[_txt_pt._font_id];
		float font_scale = _txt_pt._font_scale;

		const ImVec2 ctnt_size = _txt_area.Max - _txt_area.Min;
		dpos.x = dpos.x - ctnt_size.x*_txt_pt._txt_alignh_nml;
		dpos.y = dpos.y - ctnt_size.y*_txt_pt._txt_alignv_nml;
		af_vec2 draw_pos{ dpos.x, dpos.y };
		af_vec2 end_pos;
		wstring draw_content = utf8ToWstring(_txt_pt._content);
		auto str_sz = draw_content.size();
		float width = screenw*2;
		if (_txt_pt._width_limit)
		{
			width = _txt_pt._width;
		}
		if (str_sz > 0)
		{
			//const GLuint max_pixel_size = 512 * 512;
			
			g_pfont_face_manager->draw_wstring(font_name, _txt_pt._font_size, draw_pos, end_pos, _txt_pt._font_scale, draw_content, _txt_pt._txt_clr, width, _txt_pt._omit_rest);
		}
		af_vec2 real_size = end_pos - draw_pos;
		_txt_area.Min = dpos;
		_txt_area.Max = {end_pos.x,end_pos.y};
		ft_base::draw();

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
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	base_ui_component* ft_textblock::get_hit_ui_object(float posx, float posy)
	{
		base_ui_component* hit_opt = ft_base::get_hit_ui_object(posx, posy);
		if (hit_opt)
		{
			return hit_opt;
		}
		ImVec2 mouse_pos(posx, posy);
		if (_txt_area.Contains(mouse_pos))
		{
			return this;
		}
		else
		{
			return nullptr;
		}
	}

#endif
}