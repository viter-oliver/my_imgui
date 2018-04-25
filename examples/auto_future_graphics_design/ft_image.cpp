#include "ft_image.h"
#include "res_output.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "common_functions.h"
/*
x'=(x-a)cos¦Á+(y-b)sin¦Á+a
y'=-(x-a)sin¦Á+(y-b)cos¦Á+b
*/
void ft_image::draw()
{
	ft_base::draw();
	int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id;
	vres_txt_cd& ptext_cd = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
	if (_texture_index>=ptext_cd.size())
	{
		printf("invalid texture index:%d\n", _texture_index);
		return;
		
	}
	int texture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
	int texture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;
	float sizew =_size.x;
	float sizeh = _size.y;
	ImVec3 abpos = absolute_coordinate_of_base_pos();
	ImVec2 winpos = ImGui::GetWindowPos();
	ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
	ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
	ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
	ImVec2 pos4 = { pos1.x + sizew, pos1.y };

	ImVec2 uv0 = ImVec2(ptext_cd[_texture_index]._x0 / texture_width, ptext_cd[_texture_index]._y0 / texture_height);
	ImVec2 uv1 = ImVec2(ptext_cd[_texture_index]._x0 / texture_width, (ptext_cd[_texture_index]._y1) / texture_height);
	ImVec2 uv2 = ImVec2((ptext_cd[_texture_index]._x1) / texture_width, (ptext_cd[_texture_index]._y1) / texture_height);
	ImVec2 uv3 = ImVec2((ptext_cd[_texture_index]._x1) / texture_width, (ptext_cd[_texture_index]._y0) / texture_height);
	
	float offsetx = abpos.x - _pos.x;
	float offsety = abpos.y - _pos.y;
	ImVec2 axisBasePos = { offsetx + _axis_pos.x + winpos.x, offsety + _axis_pos.y + winpos.y };
	if (_angle != 0.f)
	{
		pos1 = rotate_point_by_zaxis(pos1, _angle, axisBasePos);
		pos2 = rotate_point_by_zaxis(pos2, _angle, axisBasePos);
		pos3 = rotate_point_by_zaxis(pos3, _angle, axisBasePos);
		pos4 = rotate_point_by_zaxis(pos4, _angle, axisBasePos);
	}
	
	ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);

#if !defined(IMGUI_WAYLAND)
	if (is_selected())//draw envelope
	{
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
		ImVec2 posaa = axisBasePos - editunit;
		ImVec2 posab = axisBasePos + editunit;
		ImGui::RenderFrame(posaa, posab, col);

	}
#endif
	
}
#if !defined(IMGUI_WAYLAND)
static void ShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
void ft_image::draw_peroperty_page()
{
	ft_base::draw_peroperty_page();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("size:");
	ImGui::SliderFloat("w", &_size.x, 0.f, base_ui_component::screenw);
	ImGui::SliderFloat("h", &_size.y, 0.f, base_ui_component::screenh);
	ImGui::Text("axis pos:");
	ImGui::SliderFloat("ax", &_axis_pos.x, 1.f, base_ui_component::screenw);
	ImGui::SliderFloat("ay", &_axis_pos.y, 1.f, base_ui_component::screenh);
	ImGui::Separator();
	ImGui::Text("angle:");
	ImGui::SliderFloat("a", &_angle, 0.f, 1.f);
	ImGui::Text("image:");
	
	auto& res_coors = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
	int isize = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates.size();
	ImGui::Combo("combo", &_texture_index, g_vres_texture_list[g_cur_texture_id_index].file_name_sets, isize);
	ImGui::SameLine(); ShowHelpMarker("select a image from image resource!\n");
	ImGui::Spacing();
	float reswidth = res_coors[_texture_index].owidth();
	float resheight = res_coors[_texture_index].oheight();
	ImGui::Text("original size:%f,%f", reswidth, resheight);
	ImGui::Spacing();
	if (reswidth>0)
	{
		float draw_height = imge_edit_view_width*resheight / reswidth;
		ImVec2 draw_size(imge_edit_view_width, draw_height);
		int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id;
		float wtexture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
		float wtexture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;

		ImVec2 uv0(res_coors[_texture_index]._x0 / wtexture_width, res_coors[_texture_index]._y0 / wtexture_height);
		ImVec2 uv1(res_coors[_texture_index]._x1 / wtexture_width, res_coors[_texture_index]._y1 / wtexture_height);
		ImGui::Image((ImTextureID)texture_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
	}


}

#endif
/*
fields:
screen_pos,
texture_id_index
texture_index
*/
bool ft_image::init_from_json(Value& jvalue)
{
	ft_base::init_from_json(jvalue);
	g_cur_texture_id_index = jvalue["texture_id_index"].asInt();
	_texture_index = jvalue["texture_index"].asInt();
	Value& jsize = jvalue["size"];
	if (!jsize.isNull())
	{
		_size.x = jsize["w"].asDouble();
		_size.y = jsize["h"].asDouble();
	}
	if (_size.x==0.f||_size.y==0.f)
	{
		vres_txt_cd& ptext_cd = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
		_size.x = ptext_cd[_texture_index]._x1 - ptext_cd[_texture_index]._x0;
		_size.y = ptext_cd[_texture_index]._y1 - ptext_cd[_texture_index]._y0;
	}
	Value& jaxispos = jvalue["axipos"];
	if (!jaxispos.isNull())
	{
		_axis_pos.x = jaxispos["x"].asDouble();
		_axis_pos.y = jaxispos["y"].asDouble();
		_angle=jaxispos["angle"].asDouble();
	}
	else
	{
		float aw = _size.x / 2;
		float ah = _size.y / 2;
		_axis_pos.x = _pos.x + aw;
		_axis_pos.y = _pos.y + ah;
	}
	return true;
}
bool ft_image::init_json_unit(Value& junit)
{
	ft_base::init_json_unit(junit);
	junit["texture_id_index"] = g_cur_texture_id_index;
	junit["texture_index"] = _texture_index;
	Value jsize(objectValue);
	jsize["w"] = _size.x;
	jsize["h"] = _size.y;
	junit["size"] = jsize;
	Value jaxispos(objectValue);
	jaxispos["x"] = _axis_pos.x;
	jaxispos["y"] = _axis_pos.y;
	jaxispos["angle"] = _angle;
	junit["axipos"] = jaxispos;
	return true;

}

ft_image::ft_image(ft_image& bsource)
	:ft_base(bsource)
{
	_size = bsource._size;
	_texture_index = bsource._texture_index;
	_angle = bsource._angle;
	_axis_pos = bsource._axis_pos;
}

base_ui_component* ft_image::get_a_copy()
{
	return new ft_image(*this);
}

