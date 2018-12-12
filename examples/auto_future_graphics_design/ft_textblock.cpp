#include "ft_textblock.h"
namespace auto_future
{
	static bool get_font_item(void* data, int idx, const char** out_str)
	{
		ImFontAtlas* atlas = ImGui::GetIO().Fonts;
		*out_str = atlas->ConfigData[idx].Name;
		return true;
	}
	ft_textblock::ft_textblock()
		: _txt_area(0.f, 0.f, 0.f, 0.f)
	{
		memset(_txt_pt._content, 0, MAX_CONTENT_LEN);
		_txt_pt._txt_clr = { 1.f, 1.f, 1.f };
		reg_property_handle(&_txt_pt, 6, [this](void*){
			ImFontAtlas* atlas = ImGui::GetIO().Fonts;
			ImGui::Combo("font:", &_txt_pt._font_id, &get_font_item, 0, atlas->Fonts.size());
		});
		reg_property_handle(&_txt_pt, 7, [this](void*){
			ImGui::DragFloat("Font scale", &_txt_pt._font_scale, 0.005f, 1.f, 10.0f, "%.1f");   // Scale only this font
			
		});
	}
	void ft_textblock::draw()
	{
		ft_base::draw();
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		//ImVec2 winpos = ImGui::GetWindowPos();
		//ImGui::SetCursorPosY(abpos.y);
		ImFontAtlas* atlas = ImGui::GetIO().Fonts;
		ImFont* font = atlas->Fonts[_txt_pt._font_id];
		float font_scale = font->Scale;
		font->Scale = _txt_pt._font_scale;
		ImGui::PushFont(font);
		const ImVec2 ctnt_size = ImGui::CalcTextSize(_txt_pt._content);
		abpos.x = abpos.x - ctnt_size.x*_txt_pt._txt_alignh_nml;
		abpos.y = abpos.y - ctnt_size.y*_txt_pt._txt_alignv_nml;
		ImVec2 winpos = ImGui::GetWindowPos();
		_txt_area.Min = abpos+winpos;
		_txt_area.Max = _txt_area.Min + ctnt_size;
		ImGui::SetCursorPos(abpos);

		if (_txt_pt._wrapped) ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + _txt_pt._width);
		ImVec4 txtclr(_txt_pt._txt_clr.x, _txt_pt._txt_clr.y, _txt_pt._txt_clr.z, 1);
		ImGui::TextColored(txtclr, _txt_pt._content);
		if (_txt_pt._wrapped) ImGui::PopTextWrapPos();
		font->Scale = font_scale;
		ImGui::PopFont();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		if (is_selected())
		{
			ImVec2 winpos = ImGui::GetWindowPos();
			ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
			ImVec2 pos2 = { pos1.x, pos1.y + ctnt_size.y };
			ImVec2 pos3 = { pos1.x + ctnt_size.x, pos1.y + ctnt_size.y };
			ImVec2 pos4 = { pos1.x + ctnt_size.x, pos1.y };

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