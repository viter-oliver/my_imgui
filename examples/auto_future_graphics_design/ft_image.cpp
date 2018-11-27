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
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "command_element.h"
#endif
namespace auto_future
{
	void ft_image::draw()
	{
		int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id;
		vres_txt_cd& ptext_cd = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
		if (_img_pt._texture_index >= ptext_cd.size())
		{
			printf("invalid texture index:%d\n", _img_pt._texture_index);
			_img_pt._texture_index = 0;

		}
		int texture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
		int texture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;
		float sizew = _img_pt._sizew;
		float sizeh = _img_pt._sizeh;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };

		ImVec2 uv0 = ImVec2(ptext_cd[_img_pt._texture_index]._x0 / texture_width, ptext_cd[_img_pt._texture_index]._y0 / texture_height);
		ImVec2 uv1 = ImVec2(ptext_cd[_img_pt._texture_index]._x0 / texture_width, (ptext_cd[_img_pt._texture_index]._y1) / texture_height);
		ImVec2 uv2 = ImVec2((ptext_cd[_img_pt._texture_index]._x1) / texture_width, (ptext_cd[_img_pt._texture_index]._y1) / texture_height);
		ImVec2 uv3 = ImVec2((ptext_cd[_img_pt._texture_index]._x1) / texture_width, (ptext_cd[_img_pt._texture_index]._y0) / texture_height);

		float offsetx = abpos.x - base_pos().x;
		float offsety = abpos.y - base_pos().y;
		ImVec2 axisBasePos = { offsetx + _img_pt._aposx + winpos.x, offsety + _img_pt._aposy + winpos.y };
		if (_img_pt._angle != 0.f)
		{
			pos1 = rotate_point_by_zaxis(pos1, _img_pt._angle, axisBasePos);
			pos2 = rotate_point_by_zaxis(pos2, _img_pt._angle, axisBasePos);
			pos3 = rotate_point_by_zaxis(pos3, _img_pt._angle, axisBasePos);
			pos4 = rotate_point_by_zaxis(pos4, _img_pt._angle, axisBasePos);
		}

		ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
		ft_base::draw();

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
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
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
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
	
	void ft_image::draw_peroperty_page(int property_part)
	{
		if (property_part&en_parent_property)
		{
			ft_base::draw_peroperty_page();
			ImGui::Spacing();
		}
		if (property_part&en_geometry_property)
		{
			ImGui::Text("size:");
			ImGui::SliderFloat("w", &_edit_size.x, 0.f, screenw);
			ImGui::SliderFloat("h", &_edit_size.y, 0.f, screenh);
			if (_edit_size.x != _img_pt._sizew || _edit_size.y != _img_pt._sizeh)
			{
				set_size(_edit_size);
			}
			ImGui::Text("axis pos:");
			ImGui::SliderFloat("ax", &_img_pt._aposx, 1.f, screenw);
			ImGui::SliderFloat("ay", &_img_pt._aposy, 1.f, screenh);
			ImGui::Separator();
			ImGui::Text("angle:");
			ImGui::SliderFloat("a", &_img_pt._angle, 0.f, 1.f);
			ImGui::Separator();
			if (ImGui::Combo("anchor type:", &_img_pt._anchor_type, "top left\0top right\0bottom right\0bottom left\0center\0\0"))
			{
				g_ui_edit_command_mg.create_command\
					(edit_commd<base_ui_component>(this, command_elemment(string("ft_image"), en_pt_ac_type, command_value(_img_pt_bk._anchor_type))));
				_img_pt_bk._anchor_type = _img_pt._anchor_type;
			}
			ImGui::Spacing();
		}
		if (property_part&en_texture_property)
		{

			ImGui::Text("image:");
			auto& res_coors = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
			int isize = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates.size();
			if (ImGui::Combo("texture index:", &_img_pt._texture_index, &get_texture_item, &g_vres_texture_list[g_cur_texture_id_index], isize))
			{
				g_ui_edit_command_mg.create_command\
					(edit_commd<base_ui_component>(this, command_elemment(string("ft_image"), en_pt_txt_id, command_value(_img_pt_bk._texture_index))));
				_img_pt_bk._texture_index = _img_pt._texture_index;
			}
			ImGui::SameLine(); ShowHelpMarker("select a image from image resource!\n");
			ImGui::Spacing();
			float reswidth = res_coors[_img_pt._texture_index].owidth();
			float resheight = res_coors[_img_pt._texture_index].oheight();
			ImGui::Text("original size:%f,%f", reswidth, resheight);
			ImGui::Spacing();
			if (reswidth > 0)
			{
				float draw_height = imge_edit_view_width*resheight / reswidth;
				ImVec2 draw_size(imge_edit_view_width, draw_height);
				int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id;
				float wtexture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
				float wtexture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;

				ImVec2 uv0(res_coors[_img_pt._texture_index]._x0 / wtexture_width, res_coors[_img_pt._texture_index]._y0 / wtexture_height);
				ImVec2 uv1(res_coors[_img_pt._texture_index]._x1 / wtexture_width, res_coors[_img_pt._texture_index]._y1 / wtexture_height);
				ImGui::Image((ImTextureID)texture_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
			}
		}
		ImGuiContext& g = *GImGui;
		if (ImGui::IsMouseReleased(0) || g.IO.InputContentChanged)
		{
			bool pt_modified = false;
			if (_img_pt._sizew != _img_pt_bk._sizew)
			{
				g_ui_edit_command_mg.create_command\
					(edit_commd<base_ui_component>(this, command_elemment(string("ft_image"), en_pt_sz_x, command_value(_img_pt_bk._sizew))));
				_img_pt_bk._sizew = _img_pt._sizew;
				pt_modified = true;
			}
			if (_img_pt._sizeh != _img_pt_bk._sizeh)
			{
				g_ui_edit_command_mg.create_command\
					(edit_commd<base_ui_component>(this, command_elemment(string("ft_image"), en_pt_sz_y, command_value(_img_pt_bk._sizeh))));
				_img_pt_bk._sizeh = _img_pt._sizeh;
				pt_modified = true;
			}
			if (_img_pt._aposx != _img_pt_bk._aposx)
			{
				g_ui_edit_command_mg.create_command\
					(edit_commd<base_ui_component>(this, command_elemment(string("ft_image"), en_pt_ax_pos_x, command_value(_img_pt_bk._aposx))));
				_img_pt_bk._aposx = _img_pt._aposx;
				pt_modified = true;
			}
			if (_img_pt._aposy != _img_pt_bk._aposy)
			{
				g_ui_edit_command_mg.create_command\
					(edit_commd<base_ui_component>(this, command_elemment(string("ft_image"), en_pt_ax_pos_y, command_value(_img_pt_bk._aposy))));
				_img_pt_bk._aposy = _img_pt._aposy;
				pt_modified = true;
			}
			if (_img_pt._angle != _img_pt_bk._angle)
			{
				g_ui_edit_command_mg.create_command\
					(edit_commd<base_ui_component>(this, command_elemment(string("ft_image"), en_pt_angle, command_value(_img_pt_bk._angle))));
				_img_pt_bk._angle = _img_pt._angle;
				pt_modified = true;
			}
			if (pt_modified)
			{
				g.IO.InputContentChanged = false;
			}
		}
	}


	void ft_image::execute_command(command_elemment& ele_cmd)
	{
		if (ele_cmd._cmd_type == "ft_image")
		{
			switch (ele_cmd._cmd_id)
			{
			case en_pt_sz_x:
				_img_pt._sizew=ele_cmd._cmd_value._value._fvalue;
				_img_pt_bk._sizew = _img_pt._sizew;
				break;
			case en_pt_sz_y:
				_img_pt._sizeh = ele_cmd._cmd_value._value._fvalue;
				_img_pt_bk._sizeh = _img_pt._sizeh;
				break;
			case en_pt_ax_pos_x:
				_img_pt._aposx = ele_cmd._cmd_value._value._fvalue;
				_img_pt_bk._aposx = _img_pt._aposx;
				break;
			case en_pt_ax_pos_y:
				_img_pt._aposy = ele_cmd._cmd_value._value._fvalue;
				_img_pt_bk._aposy = _img_pt._aposy;
				break;
			case en_pt_ac_type:
				_img_pt._anchor_type = ele_cmd._cmd_value._value._ivalue;
				_img_pt_bk._anchor_type = _img_pt._anchor_type;

				break;
			case en_pt_txt_id:
				_img_pt._texture_index = ele_cmd._cmd_value._value._ivalue;
				_img_pt_bk._texture_index = _img_pt._texture_index;
				break;
			case en_pt_angle :
				_img_pt._angle = ele_cmd._cmd_value._value._fvalue;
				_img_pt_bk._angle = _img_pt._angle;
				break;
			
			default:
				break;
			}
		}
		else
		{
			ft_base::execute_command(ele_cmd);
		}
	}

	command_elemment ft_image::clone_cmd_ele(command_elemment&ele_cmd)
	{
		if (ele_cmd._cmd_type == "ft_image")
		{
			switch (ele_cmd._cmd_id)
			{
			case en_pt_sz_x:
				return command_elemment(string("ft_mage"), en_pt_sz_x, command_value(_img_pt._sizew));
			case en_pt_sz_y:
				return command_elemment(string("ft_mage"), en_pt_sz_y, command_value(_img_pt._sizeh));
			case en_pt_ax_pos_x:
				return command_elemment(string("ft_mage"), en_pt_ax_pos_x, command_value(_img_pt._aposx));
			case en_pt_ax_pos_y:
				return command_elemment(string("ft_mage"), en_pt_ax_pos_y, command_value(_img_pt._aposy));
			case en_pt_ac_type:
				return command_elemment(string("ft_mage"), en_pt_ac_type, command_value(_img_pt._anchor_type));
			case en_pt_txt_id:
				return command_elemment(string("ft_mage"), en_pt_txt_id, command_value(_img_pt._texture_index));
			case en_pt_angle:
				return command_elemment(string("ft_mage"), en_pt_angle, command_value(_img_pt._angle));
			default:
				break;
			}
		}
		return command_elemment();
	}

	/*
	fields:
	screen_pos,
	texture_id_index
	texture_index
	*/
	bool ft_image::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		//g_cur_texture_id_index = jvalue["texture_id_index"].asInt();
		_img_pt._texture_index = jvalue["texture_index"].asInt();
		_img_pt_bk._texture_index = jvalue["texture_index"].asInt();
	   Value& jsize = jvalue["size"];
		if (!jsize.isNull())
		{
			_img_pt._sizew = jsize["w"].asDouble();
			_img_pt._sizeh = jsize["h"].asDouble();
			_edit_size = ImVec2(_img_pt._sizew, _img_pt._sizeh);
		}
		if (_img_pt._sizew == 0.f || _img_pt._sizeh == 0.f)
		{
			vres_txt_cd& ptext_cd = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
			_img_pt._sizew = ptext_cd[_img_pt._texture_index]._x1 - ptext_cd[_img_pt._texture_index]._x0;
			_img_pt._sizeh = ptext_cd[_img_pt._texture_index]._y1 - ptext_cd[_img_pt._texture_index]._y0;
		}
		Value& jaxispos = jvalue["axipos"];
		if (!jaxispos.isNull())
		{
			_img_pt._aposx = jaxispos["x"].asDouble();
			_img_pt._aposy = jaxispos["y"].asDouble();
			_img_pt._angle = jaxispos["angle"].asDouble();
		}
		else
		{
			float aw = _img_pt._sizew / 2;
			float ah = _img_pt._sizeh / 2;
			_img_pt._aposx = base_pos().x + aw;
			_img_pt._aposy = base_pos().y + ah;
		}
		_img_pt._anchor_type = jvalue["anchor_type"].asInt();
		_img_pt_bk = _img_pt;
		return true;
	}
	bool ft_image::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		//junit["texture_id_index"] = g_cur_texture_id_index;
		junit["texture_index"] = _img_pt._texture_index;
		Value jsize(objectValue);
		jsize["w"] = _img_pt._sizew;
		jsize["h"] = _img_pt._sizeh;
		junit["size"] = jsize;
		Value jaxispos(objectValue);
		jaxispos["x"] = _img_pt._aposx;
		jaxispos["y"] = _img_pt._aposy;
		jaxispos["angle"] = _img_pt._angle;
		junit["axipos"] = jaxispos;
		junit["anchor_type"] = _img_pt._anchor_type;
		return true;
	}

#endif
}