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
	int texture_id = g_vres_texture_list[_texture_id_index].texture_id;
	vres_txt_cd& ptext_cd = g_vres_texture_list[_texture_id_index].vtexture_coordinates;
	int texture_width = g_vres_texture_list[_texture_id_index].texture_width;
	int texture_height = g_vres_texture_list[_texture_id_index].texture_height;
	float sizew =_size.x;
	float sizeh = _size.y;
	ImVec2 winpos = ImGui::GetWindowPos();
	ImVec2 basePos = { _axis_pos.x + winpos.x, _axis_pos.y + winpos.y };
	ImVec2 pos1 = { _pos.x + winpos.x, _pos.y + winpos.y };
	ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
	ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
	ImVec2 pos4 = { pos1.x + sizew, pos1.y };

	ImVec2 uv0 = ImVec2(ptext_cd[_texture_index]._x0 / texture_width, ptext_cd[_texture_index]._y0 / texture_height);
	ImVec2 uv1 = ImVec2(ptext_cd[_texture_index]._x0 / texture_width, (ptext_cd[_texture_index]._y1) / texture_height);
	ImVec2 uv2 = ImVec2((ptext_cd[_texture_index]._x1) / texture_width, (ptext_cd[_texture_index]._y1) / texture_height);
	ImVec2 uv3 = ImVec2((ptext_cd[_texture_index]._x1) / texture_width, (ptext_cd[_texture_index]._y0) / texture_height);
	if (_angle != 0.f)
	{
		pos1 = rotate_point_by_zaxis(pos1, _angle, basePos);
		pos2 = rotate_point_by_zaxis(pos2, _angle, basePos);
		pos3 = rotate_point_by_zaxis(pos3, _angle, basePos);
		pos4 = rotate_point_by_zaxis(pos4, _angle, basePos);
	}
	
	ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);

#if !defined(IMGUI_WAYLAND)
	if (is_selected())//draw envelope
	{
		ImU32 col = ImGui::GetColorU32(ImGuiCol_Separator);
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

		ImVec2 posaa = basePos - editunit;
		ImVec2 posab = basePos + editunit;
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
	ImGui::Text("base pos:");
	/*ImGui::InputFloat("x", &_pos.x, 0.f, base_ui_component::screenw);
	ImGui::SameLine();*/
	ImGui::SliderFloat("x", &_pos.x, 0.f, base_ui_component::screenw);

	/*ImGui::InputFloat("y", &_pos.y, 0.f, base_ui_component::screenh);
	ImGui::SameLine();*/
	ImGui::SliderFloat("y", &_pos.y, 0.f, base_ui_component::screenh);
	ImGui::Text("size:");
	ImGui::SliderFloat("w", &_size.x, 0.f, base_ui_component::screenw);
	ImGui::SliderFloat("h", &_size.y, 0.f, base_ui_component::screenh);
	ImGui::Text("axis pos:");
	ImGui::SliderFloat("x", &_axis_pos.x, 0.f, base_ui_component::screenw);
	ImGui::SliderFloat("y", &_axis_pos.y, 0.f, base_ui_component::screenh);

	ImGui::Text("angle:");
	ImGui::SliderFloat("a", &_angle, 0.f, 1.f);
	ImGui::Separator();
	{
		// Using the _simplified_ one-liner Combo() api here
		const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
		static int item_current = 0;
		ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
		ImGui::SameLine(); ShowHelpMarker("Refer to the \"Combo\" section below for an explanation of the full BeginCombo/EndCombo API, and demonstration of various flags.\n");
	}

			{
				static char str0[128] = "Hello, world!";
				static int i0 = 123;
				static float f0 = 0.001f;
				ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));
				ImGui::SameLine(); ShowHelpMarker("Hold SHIFT or use mouse to select text.\n" "CTRL+Left/Right to word jump.\n" "CTRL+A or double-click to select all.\n" "CTRL+X,CTRL+C,CTRL+V clipboard.\n" "CTRL+Z,CTRL+Y undo/redo.\n" "ESCAPE to revert.\n");

				ImGui::InputInt("input int", &i0);
				ImGui::SameLine(); ShowHelpMarker("You can apply arithmetic operators +,*,/ on numerical values.\n  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\nUse +- to subtract.\n");

				ImGui::InputFloat("input float", &f0, 0.01f, 1.0f);
				ImGui::InputFloat("screen x", &_pos.x, 1.0f, base_ui_component::screenw);

				static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
				ImGui::InputFloat3("input float3", vec4a);
			}

			{
				static int i1 = 50, i2 = 42;
				ImGui::DragInt("drag int", &i1, 1);
				ImGui::SameLine(); ShowHelpMarker("Click and drag to edit value.\nHold SHIFT/ALT for faster/slower edit.\nDouble-click or CTRL+click to input value.");

				ImGui::DragInt("drag int 0..100", &i2, 1, 0, 100, "%.0f%%");

				static float f1 = 1.00f, f2 = 0.0067f;
				ImGui::DragFloat("drag float", &f1, 0.005f);
				ImGui::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
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
	Value& jscreen_pos = jvalue["screen_pos"];
	_pos.x = jscreen_pos["x"].asDouble();
	_pos.y = jscreen_pos["y"].asDouble();
	_pos.z = jscreen_pos["z"].asDouble();
	_texture_id_index = jvalue["texture_id_index"].asInt();
	_texture_index = jvalue["texture_index"].asInt();
	Value& jsize = jvalue["size"];
	if (!jsize.isNull())
	{
		_size.x = jsize["w"].asDouble();
		_size.y = jsize["h"].asDouble();
	}
	if (_size.x==0.f||_size.y==0.f)
	{
		vres_txt_cd& ptext_cd = g_vres_texture_list[_texture_id_index].vtexture_coordinates;
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

void ft_image::offset(ImVec2& imof)
{
	_pos.x += imof.x;
	_pos.y += imof.y;
	_axis_pos.x += imof.x;
	_axis_pos.y += imof.y;
}
