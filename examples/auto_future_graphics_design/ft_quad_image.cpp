#include "ft_quad_image.h"
#include "res_output.h"
#include "common_functions.h"
/*
x'=(x-a)cos¦Á+(y-b)sin¦Á+a
y'=-(x-a)sin¦Á+(y-b)cos¦Á+b
*/


namespace auto_future
{
	static inline ImVec2 operator + (const ImVec2& imPt, const af_vec2& afPt)
	{
		return ImVec2(imPt.x + afPt.x, imPt.y + afPt.y);
	}
	static inline af_vec2 operator + (const const af_vec2& afPt,ImVec2& imPt )
	{
		return{ imPt.x + afPt.x, imPt.y + afPt.y };
	}
	ft_quad_image::ft_quad_image()
		:ft_base()
	{
          _img_pt._texture_index_txt = { 0, 0 };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		_in_p._sizew = 200;
		_in_p._sizeh = 200;
		_img_pt._imge_pos[0] = { 0., 0. };
		_img_pt._imge_pos[1] = { 0., 100. };
		_img_pt._imge_pos[2] = { 100., 100. };
		_img_pt._imge_pos[3] = { 100., 0. };

		reg_property_handle(&_img_pt, 0, [this](void*){
			static int pt_id = 0;
			ImGui::SliderInt("Select a point:", &pt_id, 0, 3);
			if (pt_id<4)
			{
				ImGui::SliderFloat2("Selected point", &_img_pt._imge_pos[pt_id].x, 0, _in_p._sizew);
			}
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 20));
			ImVec2 chd_wd_sz(_in_p._sizew, _in_p._sizeh);
			ImGui::BeginChild("canvas", chd_wd_sz, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
			ImVec2 origin = ImGui::GetCursorScreenPos();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
			draw_list->AddLine(origin + _img_pt._imge_pos[0], origin + _img_pt._imge_pos[1], GRID_COLOR);
			draw_list->AddLine(origin + _img_pt._imge_pos[1], origin + _img_pt._imge_pos[2], GRID_COLOR);
			draw_list->AddLine(origin + _img_pt._imge_pos[2], origin + _img_pt._imge_pos[3], GRID_COLOR);
			draw_list->AddLine(origin + _img_pt._imge_pos[0], origin + _img_pt._imge_pos[3], GRID_COLOR);
			ImU32 pt_col = IM_COL32(200, 150, 150, 250);
			const float pt_radius = 4.;
			string btn_cap = "btn";
			for (int ix = 0; ix < 4;ix++)
			{
				ImVec2 pos_d = origin + _img_pt._imge_pos[ix];
				draw_list->AddCircleFilled(pos_d, pt_radius, pt_col);
				ImVec2 btn_pos = pos_d - ImVec2(4., 4.);
				ImGui::SetCursorScreenPos(btn_pos);
				btn_cap += "_b";
				ImGui::InvisibleButton(btn_cap.c_str(), ImVec2(8., 8.));
				if (ImGui::IsItemActive()&&ImGui::IsMouseDragging(0))
				{
					_img_pt._imge_pos[ix] = _img_pt._imge_pos[ix] + ImGui::GetIO().MouseDelta;
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		});
#endif
	}
	void ft_quad_image::draw()
	{
          auto& img_txt_id = _img_pt._texture_index_txt;
          auto& cur_res_list = *g_vres_texture_list[ img_txt_id.x ];
          int texture_id = cur_res_list.texture_id();
          vres_txt_cd& ptext_cd = cur_res_list.vtexture_coordinates;
		if (ptext_cd.size() == 0)
			return;
          if( img_txt_id.y >= ptext_cd.size() )
		{
               printf( "invalid texture index:%d\n", img_txt_id.y);
               return;
		}
          int texture_width = cur_res_list.texture_width;
          int texture_height = cur_res_list.texture_height;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 basepos= { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos0 =basepos+_img_pt._imge_pos[0];
		ImVec2 pos1 =basepos+_img_pt._imge_pos[1];
		ImVec2 pos2 = basepos + _img_pt._imge_pos[2];
		ImVec2 pos3 = basepos + _img_pt._imge_pos[3];

          ImVec2 uv0 = ImVec2( ptext_cd[ img_txt_id.y ]._x0 / texture_width, ptext_cd[ img_txt_id.y ]._y0 / texture_height );
          ImVec2 uv1 = ImVec2( ptext_cd[ img_txt_id.y ]._x0 / texture_width, ( ptext_cd[ img_txt_id.y ]._y1 ) / texture_height );
          ImVec2 uv2 = ImVec2( ( ptext_cd[ img_txt_id.y ]._x1 ) / texture_width, ( ptext_cd[ img_txt_id.y ]._y1 ) / texture_height );
          ImVec2 uv3 = ImVec2( ( ptext_cd[ img_txt_id.y ]._x1 ) / texture_width, ( ptext_cd[ img_txt_id.y ]._y0 ) / texture_height );



		ImGui::ImageQuad((ImTextureID)texture_id, pos0, pos1, pos2, pos3, uv0, uv1, uv2, uv3);
		//ft_base::draw();

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		if (is_selected())//draw envelope
		{
			ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
			ImVec2 editunit(edit_unit_len, edit_unit_len);
			ImVec2 pos1a = pos0 - editunit;
			ImVec2 pos1b = pos0 + editunit;
			ImGui::RenderFrame(pos1a, pos1b, col);
			ImVec2 pos2a = pos1 - editunit;
			ImVec2 pos2b = pos1 + editunit;
			ImGui::RenderFrame(pos2a, pos2b, col);
			ImVec2 pos3a = pos2 - editunit;
			ImVec2 pos3b = pos2 + editunit;
			ImGui::RenderFrame(pos3a, pos3b, col);
			ImVec2 pos4a = pos3 - editunit;
			ImVec2 pos4b = pos3 + editunit;
			ImGui::RenderFrame(pos4a, pos4b, col);

		}
#endif

	}
}