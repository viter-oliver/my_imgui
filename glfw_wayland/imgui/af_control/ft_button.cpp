#include "ft_button.h"
#include "res_output.h"
namespace auto_future
{
	void ft_button::draw()
	{
		ft_base::draw();
		int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id;
		vres_txt_cd& ptext_cd = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
		int cur_txt_id = get_cur_txt_id();
		
		int texture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
		int texture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;
		float sizew = _pt._size.x;
		float sizeh = _pt._size.y;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };

		ImVec2 uv0 = ImVec2(ptext_cd[cur_txt_id]._x0 / texture_width, ptext_cd[cur_txt_id]._y0 / texture_height);
		ImVec2 uv1 = ImVec2(ptext_cd[cur_txt_id]._x0 / texture_width, (ptext_cd[cur_txt_id]._y1) / texture_height);
		ImVec2 uv2 = ImVec2((ptext_cd[cur_txt_id]._x1) / texture_width, (ptext_cd[cur_txt_id]._y1) / texture_height);
		ImVec2 uv3 = ImVec2((ptext_cd[cur_txt_id]._x1) / texture_width, (ptext_cd[cur_txt_id]._y0) / texture_height);



		ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);

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
			
		}
#endif
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void ft_button::draw_peroperty_page(int property_part)
	{
		ft_base::draw_peroperty_page();
		ImGui::Spacing();
		ImGui::SliderFloat("width", &_pt._size.x, 0.f, base_ui_component::screenw);
		ImGui::SliderFloat("height", &_pt._size.y, 0.f, base_ui_component::screenh);
		ImGui::Spacing();

		auto& res_coors = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
		int isize = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates.size();
		int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id;
		float wtexture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
		float wtexture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;

		ImGui::Combo("normal texture index:", &_pt._texture_indices[en_normal], &get_texture_item, &g_vres_texture_list[g_cur_texture_id_index], isize);
		ImGui::Spacing();
		float reswidth = res_coors[_pt._texture_indices[en_normal]].owidth();
		float resheight = res_coors[_pt._texture_indices[en_normal]].oheight();
		ImGui::Text("normal texture original size:%f,%f", reswidth, resheight);
		ImGui::Spacing();
		if (reswidth > 0)
		{
			float draw_height = imge_edit_view_width*resheight / reswidth;
			ImVec2 draw_size(imge_edit_view_width, draw_height);

			ImVec2 uv0(res_coors[_pt._texture_indices[en_normal]]._x0 / wtexture_width, res_coors[_pt._texture_indices[en_normal]]._y0 / wtexture_height);
			ImVec2 uv1(res_coors[_pt._texture_indices[en_normal]]._x1 / wtexture_width, res_coors[_pt._texture_indices[en_normal]]._y1 / wtexture_height);
			ImGui::Image((ImTextureID)texture_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		}

		ImGui::Combo("pressed texture index:", &_pt._texture_indices[en_pressed], &get_texture_item, &g_vres_texture_list[g_cur_texture_id_index], isize);
		ImGui::Spacing();
		reswidth = res_coors[_pt._texture_indices[en_pressed]].owidth();
		resheight = res_coors[_pt._texture_indices[en_pressed]].oheight();
		ImGui::Text("pressed texture original size:%f,%f", reswidth, resheight);
		ImGui::Spacing();
		if (reswidth > 0)
		{
			float draw_height = imge_edit_view_width*resheight / reswidth;
			ImVec2 draw_size(imge_edit_view_width, draw_height);
			
			ImVec2 uv0(res_coors[_pt._texture_indices[en_pressed]]._x0 / wtexture_width, res_coors[_pt._texture_indices[en_pressed]]._y0 / wtexture_height);
			ImVec2 uv1(res_coors[_pt._texture_indices[en_pressed]]._x1 / wtexture_width, res_coors[_pt._texture_indices[en_pressed]]._y1 / wtexture_height);
			ImGui::Image((ImTextureID)texture_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		}
		ImGui::Combo("selected texture index:", &_pt._texture_indices[en_selected], &get_texture_item, &g_vres_texture_list[g_cur_texture_id_index], isize);
		ImGui::Spacing();
		reswidth = res_coors[_pt._texture_indices[en_selected]].owidth();
		resheight = res_coors[_pt._texture_indices[en_selected]].oheight();
		ImGui::Text("selected texture original size:%f,%f", reswidth, resheight);
		ImGui::Spacing();
		if (reswidth > 0)
		{
			float draw_height = imge_edit_view_width*resheight / reswidth;
			ImVec2 draw_size(imge_edit_view_width, draw_height);
			ImVec2 uv0(res_coors[_pt._texture_indices[en_selected]]._x0 / wtexture_width, res_coors[_pt._texture_indices[en_selected]]._y0 / wtexture_height);
			ImVec2 uv1(res_coors[_pt._texture_indices[en_selected]]._x1 / wtexture_width, res_coors[_pt._texture_indices[en_selected]]._y1 / wtexture_height);
			ImGui::Image((ImTextureID)texture_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		}
	}

	bool ft_button::init_from_json(Value& jvalue)
	{
		ft_base::init_json_unit(jvalue);
		Value& texture_indices = jvalue["texture_indices"];
		_pt._texture_indices[en_normal] = texture_indices["normal"].asInt();
		_pt._texture_indices[en_pressed] = texture_indices["pressed"].asInt();
		_pt._texture_indices[en_selected] = texture_indices["selected"].asInt();
		Value& bsize = jvalue["size"];
		_pt._size.x = bsize["x"].asDouble();
		_pt._size.y = bsize["y"].asDouble();
		return true;
	}
	bool ft_button::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		Value jtexture_indices(objectValue);
		jtexture_indices["normal"] = _pt._texture_indices[en_normal];
		jtexture_indices["pressed"] = _pt._texture_indices[en_pressed];
		jtexture_indices["selected"] = _pt._texture_indices[en_selected];
		junit["texture_indices"] = jtexture_indices;
		Value jsize(objectValue);
		jsize["x"] = _pt._size.x;
		jsize["y"] = _pt._size.y;
		junit["size"] = jsize;
		return true;
	}
#endif
	bool ft_button::handle_mouse()
	{
		return true;
	}

	ft_button::ft_button()
		:ft_base()
	{

	}

}