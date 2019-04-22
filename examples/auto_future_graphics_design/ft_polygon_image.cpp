#include "ft_polygon_image.h"
#include "res_output.h"
#include <sstream>
namespace auto_future
{
	ft_polygon_image::ft_polygon_image()
		:ft_base()
	{
		memset((char*)_img_pt._pos, 0, sizeof(_img_pt._pos));
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_img_pt, 0, [this](void*){
			ImGui::SliderInt("pos pair count", &_img_pt._pos_pair_cnt, 2, MAX_POS_PAIR_CNT);
		});
		reg_property_handle(&_img_pt, 1, [this](void*){
			stringstream stm;
			for (int ix = 0; ix < _img_pt._pos_pair_cnt;ix++)
			{
				stm << "pos pair" << ix;
				ImGui::SliderFloat4(stm.str().c_str(), (float*)&_img_pt._pos[ix * 2], 0., 400.);
				stm.str(string());
				stm.clear();
			}
		});

#endif
	}
	void ft_polygon_image::draw()
	{
		int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id();
		vres_txt_cd& ptext_cd = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
		if (_img_pt._texture_index_txt >= ptext_cd.size())
		{
			printf("invalid texture index:%d\n", _img_pt._texture_index_txt);
			_img_pt._texture_index_txt = 0;

		}
		int tw = g_vres_texture_list[g_cur_texture_id_index].texture_width;
		int th = g_vres_texture_list[g_cur_texture_id_index].texture_height;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		af_vec2 basePpos = { abpos.x + winpos.x, abpos.y + winpos.y };
		float x0 = ptext_cd[_img_pt._texture_index_txt]._x0;
		float y0 = ptext_cd[_img_pt._texture_index_txt]._y0;
		float x1 = ptext_cd[_img_pt._texture_index_txt]._x1;
		float y1 = ptext_cd[_img_pt._texture_index_txt]._y1;
		float delta_y = (y1 - y0) / (_img_pt._pos_pair_cnt-1);
		delta_y /= th;
		auto ipk = [](af_vec2& tar){return ImVec2(tar.x, tar.y); };
		ImVec2 uv0{ x0 / tw, y0 / th };
		ImVec2 uv3{ x1 / tw, y0 / th };
		int imax = (_img_pt._pos_pair_cnt - 1) * 2;
		for (int ix = 0; ix <imax ;ix+=2)
		{
			af_vec2 pos0 = basePpos + _img_pt._pos[ix];
			af_vec2 pos1 = basePpos + _img_pt._pos[ix + 2];
			af_vec2 pos2 = basePpos + _img_pt._pos[ix + 3];
			af_vec2 pos3 = basePpos + _img_pt._pos[ix + 1];
			ImVec2 uv1{ uv0.x, uv0.y + delta_y };
			ImVec2 uv2{ uv3.x, uv1.y };
			ImGui::ImageQuad((ImTextureID)texture_id, ipk(pos0), ipk(pos1),ipk(pos2), ipk(pos3), uv0, uv1, uv2, uv3);
			uv0 = uv1;
			uv3 = uv2;
		}


		ft_base::draw();

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		if (is_selected())//draw envelope
		{
			/*ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
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
			ImGui::RenderFrame(pos4a, pos4b, col);*/

		}
#endif
	}

}