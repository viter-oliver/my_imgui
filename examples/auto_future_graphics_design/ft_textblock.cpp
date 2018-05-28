#include "ft_textblock.h"

void ft_textblock::draw()
{
	ft_base::draw();
	ImVec3 abpos = absolute_coordinate_of_base_pos();
	//ImVec2 winpos = ImGui::GetWindowPos();
	ImGui::SetCursorPosX(abpos.x );
	ImGui::SetCursorPosY(abpos.y);
	if (_txt_pt._wrapped) ImGui::PushTextWrapPos(ImGui::GetCursorPos().x +_txt_pt._width);
	ImGui::TextColored(_txt_pt._txt_color, _txt_pt._content);
	//ImGui::Text(_txt_pt._content);

	if (_txt_pt._wrapped) ImGui::PopTextWrapPos();

}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void ft_textblock::draw_peroperty_page()
{
	ft_base::draw_peroperty_page();
	ImGui::InputText("content:", _txt_pt._content, MAX_CONTENT_LEN);
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("#size:");
	ImGui::SliderFloat("w", &_txt_pt._width, 0.f, base_ui_component::screenw);
	//ImGui::SliderFloat("h", &_txt_pt._size.y, 0.f, base_ui_component::screenh);
	ImGui::Checkbox("wrapped", &_txt_pt._wrapped);
	ImGui::ColorEdit3("text color:", (float*)&_txt_pt._txt_color, ImGuiColorEditFlags_RGB);
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
	_txt_pt._txt_color.x = txt_color["x"].asDouble();
	_txt_pt._txt_color.y = txt_color["y"].asDouble();
	_txt_pt._txt_color.z = txt_color["z"].asDouble();
	_txt_pt._txt_color.w = txt_color["w"].asDouble();
	_txt_pt._width = jvalue["width"].asDouble();
	Value& content = jvalue["content"];
	strcpy(_txt_pt._content, content.asCString());
	_txt_pt._wrapped = jvalue["wrapped"].asBool();
	return true;
}
bool ft_textblock::init_json_unit(Value& junit)
{
	ft_base::init_json_unit(junit);
	Value txt_color(objectValue);
	txt_color["x"] = _txt_pt._txt_color.x;
	txt_color["y"] = _txt_pt._txt_color.y;
	txt_color["z"] = _txt_pt._txt_color.z;
	txt_color["w"] = _txt_pt._txt_color.w;
	junit["txt_color"] = txt_color;
	junit["width"] = _txt_pt._width;
	junit["content"] = _txt_pt._content;
	junit["wrapped"] = _txt_pt._wrapped;
	return true;
}
#endif
