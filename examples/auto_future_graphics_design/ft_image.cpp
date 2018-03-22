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
	float sizew = ptext_cd[_texture_index]._x1 - ptext_cd[_texture_index]._x0;
	float sizeh = ptext_cd[_texture_index]._y1 - ptext_cd[_texture_index]._y0;

	if (_angle != 0.0f)
	{
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 basePos = { _axis_pos.x + winpos.x, _axis_pos.y + winpos.y};
		ImVec2 pos1 = { _pos.x + winpos.x, _pos.y + winpos.y };
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };
		
		ImVec2 desPos1 = rotate_point_by_zaxis(pos1, _angle, basePos);
		ImVec2 desPos2 = rotate_point_by_zaxis(pos2, _angle, basePos);
		ImVec2 desPos3 = rotate_point_by_zaxis(pos3, _angle, basePos);
		ImVec2 desPos4 = rotate_point_by_zaxis(pos4, _angle, basePos);

		//des_verteices[0].x;
		ImVec2 uv0 = ImVec2(ptext_cd[_texture_index]._x0 / texture_width, ptext_cd[_texture_index]._y0 / texture_height);
		ImVec2 uv1 = ImVec2(ptext_cd[_texture_index]._x0 / texture_width, (ptext_cd[_texture_index]._y0 +sizeh)/ texture_height);
		ImVec2 uv2 = ImVec2((ptext_cd[_texture_index]._x0 +sizew)/ texture_width, (ptext_cd[_texture_index]._y0+sizeh) / texture_height);
		ImVec2 uv3 = ImVec2((ptext_cd[_texture_index]._x1 +sizew)/ texture_width, (ptext_cd[_texture_index]._y1)/ texture_height);

		ImGui::ImageQuad((ImTextureID)texture_id, desPos1, desPos2,desPos3,desPos4,	uv0, uv1, uv2, uv3);
	}
	else
	{
		ImVec2 precPos = ImGui::GetCursorPos();
		ImVec2 drawPos;
		drawPos.x = _pos.x;
		drawPos.y = _pos.y;
		ImGui::SetCursorPos(drawPos);
	
		ImVec2 uv0 = ImVec2(ptext_cd[_texture_index]._x0 / texture_width, ptext_cd[_texture_index]._y0 / texture_height);
		ImVec2 uv1 = ImVec2(ptext_cd[_texture_index]._x1 / texture_width, ptext_cd[_texture_index]._y1 / texture_height);

		ImGui::Image((ImTextureID)texture_id, ImVec2(sizew, sizeh), uv0, uv1);// , ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));

		ImGui::SetCursorPos(precPos);
	}
}
#if !defined(IMGUI_WAYLAND)
void ft_image::draw_peroperty_page()
{

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
	Value& jaxispos = jvalue["axipos"];
	if (!jaxispos.isNull())
	{
		_axis_pos.x = jaxispos["x"].asDouble();
		_axis_pos.y = jaxispos["y"].asDouble();
		_angle=jaxispos["angle"].asDouble();
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
