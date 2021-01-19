#include "ft_image.h"
#include "res_output.h"

#include "common_functions.h"
/*
x'=(x-a)cos��+(y-b)sin��+a
y'=-(x-a)sin��+(y-b)cos��+b
*/
namespace auto_future
{
	ft_image::ft_image()
		:ft_base()
	{
          _img_pt._tin_clr = { 1.f, 1.f, 1.f };
          _img_pt._texture_index_txt = { 0, 0 };
 #if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_img_pt, 2, [this](void*){
			ImGui::Combo("anchor type:", &_img_pt._anchor_type, "top left\0top right\0bottom right\0bottom left\0center\0\0");
		});
#endif
	}
	void ft_image::draw()
	{
          auto img_txt_id = _img_pt._texture_index_txt;
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
		float sizew = _in_p._sizew;
		float sizeh = _in_p._sizeh;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos1 = { pos0.x, pos0.y + sizeh };
		ImVec2 pos2 = { pos0.x + sizew, pos0.y + sizeh };
		ImVec2 pos3 = { pos0.x + sizew, pos0.y };
          ImVec2 uv0 = ImVec2( ptext_cd[ img_txt_id.y ]._x0 / texture_width, ptext_cd[ img_txt_id.y ]._y0 / texture_height );
          ImVec2 uv1 = ImVec2( ptext_cd[ img_txt_id.y ]._x0 / texture_width, ( ptext_cd[ img_txt_id.y ]._y1 ) / texture_height );
          ImVec2 uv2 = ImVec2( ( ptext_cd[ img_txt_id.y ]._x1 ) / texture_width, ( ptext_cd[ img_txt_id.y ]._y1 ) / texture_height );
          ImVec2 uv3 = ImVec2( ( ptext_cd[ img_txt_id.y ]._x1 ) / texture_width, ( ptext_cd[ img_txt_id.y ]._y0 ) / texture_height );

		float offsetx = abpos.x - base_pos().x;
		float offsety = abpos.y - base_pos().y;
		ImVec2 axisBasePos = { offsetx + _img_pt._aposx + winpos.x, offsety + _img_pt._aposy + winpos.y };
		if (_img_pt._angle_srd != 0.f)
		{
			pos0 = rotate_point_by_zaxis(pos0, _img_pt._angle_srd, axisBasePos);
			pos1 = rotate_point_by_zaxis(pos1, _img_pt._angle_srd, axisBasePos);
			pos2 = rotate_point_by_zaxis(pos2, _img_pt._angle_srd, axisBasePos);
			pos3 = rotate_point_by_zaxis(pos3, _img_pt._angle_srd, axisBasePos);
		}
          ImVec4 tin_clr( _img_pt._tin_clr.x, _img_pt._tin_clr.y, _img_pt._tin_clr.z, _img_pt._alpha_nml );
          ImGui::ImageQuad( (ImTextureID)texture_id, pos0, pos1, pos2, pos3, uv0, uv1, uv2, uv3, tin_clr );
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
			ImVec2 posaa = axisBasePos - editunit;
			ImVec2 posab = axisBasePos + editunit;
			ImGui::RenderFrame(posaa, posab, col);

		}
#endif

	}
}
