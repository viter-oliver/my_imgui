#include "ft_textblock.h"
namespace auto_future
{
	ft_textblock::ft_textblock()
		: _txt_area(0.f, 0.f, 0.f, 0.f)
	{
		memset(_txt_pt._content, 0, MAX_CONTENT_LEN);
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
		abpos.x = abpos.x - ctnt_size.x*_txt_pt._txt_alignh_nm;
		abpos.y = abpos.y - ctnt_size.y*_txt_pt._txt_alignv_nm;
		_txt_area.Min = abpos;
		_txt_area.Max = abpos + ctnt_size;
		ImGui::SetCursorPos(abpos);

		if (_txt_pt._wrapped) ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + _txt_pt._width);
		ImGui::TextColored(ImVec4(_txt_pt._txtr, _txt_pt._txtg, _txt_pt._txtb, _txt_pt._txta), _txt_pt._content);
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
	static bool get_font_item(void* data, int idx, const char** out_str)
	{
		ImFontAtlas* atlas = ImGui::GetIO().Fonts;
		*out_str = atlas->ConfigData[idx].Name;
		return true;
	}
	void ft_textblock::draw_peroperty_page(int property_part)
	{
		ft_base::draw_peroperty_page();
		ImGui::InputText("content:", _txt_pt._content, MAX_CONTENT_LEN);
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("#size:");
		ImGui::SliderFloat("w", &_txt_pt._width, 0.f, base_ui_component::screenw);
		ImGui::SliderFloat("align h", &_txt_pt._txt_alignh_nm, 0.f, 1.f);
		ImGui::SliderFloat("align v", &_txt_pt._txt_alignv_nm, 0.f, 1.f);

		//ImGui::SliderFloat("h", &_txt_pt._size.y, 0.f, base_ui_component::screenh);
		ImGui::Checkbox("wrapped", &_txt_pt._wrapped);
		ImGui::ColorEdit3("text color:", (float*)&_txt_pt._txtr, ImGuiColorEditFlags_RGB);
		ImGui::DragFloat("Font scale", &_txt_pt._font_scale, 0.005f, 1.f, 10.0f, "%.1f");   // Scale only this font

		ImFontAtlas* atlas = ImGui::GetIO().Fonts;

		ImGui::Combo("font:", &_txt_pt._font_id, &get_font_item, 0, atlas->Fonts.size());
	}
	/*
	fields:
	txt_color,
	width
	content
	wrapped
	*/
	bool ft_textblock::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		Value& txt_color = jvalue["txt_color"];
		_txt_pt._txtr = txt_color["x"].asDouble();
		_txt_pt._txtg = txt_color["y"].asDouble();
		_txt_pt._txtb = txt_color["z"].asDouble();
		_txt_pt._txta = txt_color["w"].asDouble();
		_txt_pt._width = jvalue["width"].asDouble();
		Value& txt_align = jvalue["align"];
		_txt_pt._txt_alignh_nm = txt_align["h"].asDouble();
		_txt_pt._txt_alignv_nm = txt_align["v"].asDouble();
		Value& content = jvalue["content"];
		strcpy(_txt_pt._content, content.asCString());
		_txt_pt._wrapped = jvalue["wrapped"].asBool();
		_txt_pt._font_id = jvalue["font_id"].asDouble();
		_txt_pt._font_scale = jvalue["font_scale"].asDouble();
		return true;
	}
	bool ft_textblock::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		Value txt_color(objectValue);
		txt_color["x"] = _txt_pt._txtr;
		txt_color["y"] = _txt_pt._txtg;
		txt_color["z"] = _txt_pt._txtb;
		txt_color["w"] = _txt_pt._txta;
		junit["txt_color"] = txt_color;
		Value txt_align(objectValue);
		txt_align["h"] = _txt_pt._txt_alignv_nm;
		txt_align["v"] = _txt_pt._txt_alignh_nm;
		junit["align"] = txt_align;
		junit["width"] = _txt_pt._width;
		junit["content"] = _txt_pt._content;
		junit["wrapped"] = _txt_pt._wrapped;
		junit["font_id"] = _txt_pt._font_id;
		junit["font_scale"] = _txt_pt._font_scale;
		return true;
	}
#endif
}