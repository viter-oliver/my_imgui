#include "ft_button.h"
#include "res_output.h"
namespace auto_future
{
	ft_button::ft_button()
		:ft_base()
	{

	}
	void ft_button::draw()
	{
		//ft_base::draw();
		af_vi2 cur_txt_id = get_cur_txt_id();
          int texture_id = g_vres_texture_list[ cur_txt_id.x].texture_id();
          vres_txt_cd& ptext_cd = g_vres_texture_list[ cur_txt_id.x].vtexture_coordinates;
		printf("cur_txt_id=%d\n", cur_txt_id);
          int texture_width = g_vres_texture_list[ cur_txt_id.x ].texture_width;
          int texture_height = g_vres_texture_list[ cur_txt_id.x ].texture_height;
		float sizew = _in_p._sizew;
		float sizeh = _in_p._sizeh;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };

          ImVec2 uv0 = ImVec2( ptext_cd[ cur_txt_id.y ]._x0 / texture_width, ptext_cd[ cur_txt_id.y ]._y0 / texture_height );
          ImVec2 uv1 = ImVec2( ptext_cd[ cur_txt_id.y ]._x0 / texture_width, ( ptext_cd[ cur_txt_id.y ]._y1 ) / texture_height );
          ImVec2 uv2 = ImVec2( ( ptext_cd[ cur_txt_id.y ]._x1 ) / texture_width, ( ptext_cd[ cur_txt_id.y ]._y1 ) / texture_height );
          ImVec2 uv3 = ImVec2( ( ptext_cd[ cur_txt_id.y ]._x1 ) / texture_width, ( ptext_cd[ cur_txt_id.y ]._y0 ) / texture_height );



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

#endif
	bool ft_button::handle_mouse()
	{
		return true;
	}
}