#include "ft_polygon_image.h"
#include "res_output.h"
#include <sstream>
#include "common_functions.h"
namespace auto_future
{
	
	extern float length_of_p2p(ImVec2& p0, ImVec2&p1);
	void ft_polygon_image::read_track_file(const char* track_file)
	{
		auto& ifile = g_mfiles_list.find(track_file);
		if (ifile != g_mfiles_list.end())
		{
			auto& p_trk_file=ifile->second;
			char* pData = (char*)p_trk_file->_pbin;
			auto buff_sz = p_trk_file->_fsize;
			auto alen = conver_track_buff_to_pair(pData, buff_sz, _track0, _track1);
			_track_segs0.clear();
			_track_segs1.clear();
			_seg_len0 = _seg_len1 = 0.;
			_ps_track_file = ifile->second;
			if (alen<2)
			{
				_ps_track_file = nullptr;
				return;
			}
			for (int ix = 0; ix < (int)alen - 1; ix++)
			{
				auto back0 = length_of_p2p(_track0[ix], _track0[ix + 1]);
				_seg_len0 += back0;
				_track_segs0.emplace_back(back0);

				auto back1 = length_of_p2p(_track1[ix], _track1[ix + 1]);
				_seg_len1 += back1;
				_track_segs1.emplace_back(back1);
			}

		}
	}
	void ft_polygon_image::link()
	{
		read_track_file(_img_pt._track_file);
	}
	ft_polygon_image::ft_polygon_image()
		:ft_base()
	{
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_img_pt, 0, [this](void*){
			if (_track0.size()<2)
			{
				ImGui::InputText("Track path file", _img_pt._track_file,name_len);
				if (ImGui::Button("Link"))
				{
					read_track_file(_img_pt._track_file);
				}
			}
			else
			{
				ImGui::Text("Track path file:%s", _img_pt._track_file);
				ImGui::SameLine();
				if (ImGui::Button("Delink##track"))
				{
					_track0.clear();
					_track1.clear();
					_ps_track_file = nullptr;
					_img_pt._track_file[0] = '\0';
				}
			}
		});
		

#endif
	}
	void ft_polygon_image::draw()
	{
		if (!_ps_track_file)
		{
			return;
		}
		int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id();
		vres_txt_cd& ptext_cd = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
		if (_img_pt._img_txt >= ptext_cd.size())
		{
			return;

		}
		int tw = g_vres_texture_list[g_cur_texture_id_index].texture_width;
		int th = g_vres_texture_list[g_cur_texture_id_index].texture_height;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 basePpos = { abpos.x + winpos.x, abpos.y + winpos.y };
		float x0 = ptext_cd[_img_pt._img_txt]._x0/tw;
		float y0 = ptext_cd[_img_pt._img_txt]._y0/th;
		float x1 = ptext_cd[_img_pt._img_txt]._x1/tw;
		float y1 = ptext_cd[_img_pt._img_txt]._y1/th;
		float wdelta = (y1 - y0) / 80.0;
		y0 += wdelta;
		y1 -= wdelta;
		float udelta = x1 - x0;
		float vdelta = y1 - y0;
		auto pt_cnt = _track0.size();
		if (pt_cnt<2)
		{
			return;
		}
		float prs_value0 = _img_pt._progrss_nml*_seg_len0;
		float prs_value1 = _img_pt._progrss_nml*_seg_len1;
		float prs_value = (prs_value0 + prs_value1) / 2.0;
		int i_cross = 0;
		float tmp_len = 0.;
		for (; i_cross < pt_cnt - 1; i_cross++)// find the cross segments
		{
			auto av_seg = (_track_segs0[i_cross] + _track_segs0[i_cross]) / 2.0;
			tmp_len += av_seg;
			if (tmp_len>prs_value)
			{
				break;
			}
		}

		float v_seg0 = 0., v_seg1 = 0.;
		float ybase = y0 +(1- _img_pt._progrss_nml)*vdelta;
		float v_p0 = 0, v_p1 = 0, v_n0 = 0, v_n1 = 0;
		for (int ix = 0; ix <i_cross;ix++)
		{
			ImVec2 pos0 = basePpos + _track0[ix];
			ImVec2 pos1 = basePpos + _track0[ix + 1];
			ImVec2 pos2 = basePpos + _track1[ix + 1];
			ImVec2 pos3 = basePpos + _track1[ix];
			v_p0 = v_seg0 / _seg_len0;
			v_p1 = v_seg1 / _seg_len1;
			v_seg0 += _track_segs0[ix];
			v_seg1 += _track_segs1[ix];

			v_n0 = v_seg0 / _seg_len0;
			v_n1 = v_seg1 / _seg_len1;
			ImVec2 uv0(x0, ybase + v_p0 * vdelta);
			ImVec2 uv1(x0, ybase + v_n0 * vdelta);
			ImVec2 uv2(x1, ybase + v_n1 * vdelta);
			ImVec2 uv3(x1, ybase + v_p1 * vdelta);
			ImGui::ImageQuad((ImTextureID)texture_id, pos0, pos1, pos2, pos3, uv0, uv1, uv2, uv3);
		}
		float delta_len0 = prs_value0 - v_seg0;
		float delta_len1 = prs_value1 - v_seg1;

		ImVec2 cs_pos0 = basePpos + _track0[i_cross];
		ImVec2 cs_uv0(x0, ybase + v_n0*vdelta);
		ImVec2 dir0 = _track0[i_cross + 1] - _track0[i_cross];
		float dir0_len = length_of_p2p(_track0[i_cross], _track0[i_cross+1]);
		ImVec2 dir0_nml = dir0 / dir0_len;
		ImVec2 cs_pos_c0 = cs_pos0+ dir0_nml*delta_len0;
		float vc0 = prs_value0 / _seg_len0;
		ImVec2 cs_uv_c0(x0, ybase + vc0*vdelta);

		ImVec2 cs_pos1 = basePpos + _track1[i_cross];
		ImVec2 cs_uv1(x1, ybase + v_n1*vdelta);
		ImVec2 dir1 = _track1[i_cross + 1] - _track1[i_cross];
		float dir1_len = length_of_p2p(_track1[i_cross], _track1[i_cross + 1]);
		ImVec2 dir1_nml = dir1 / dir1_len;
		ImVec2 cs_pos_c1 = cs_pos1 + dir1_nml*delta_len1;
		float vc1 = prs_value1 / _seg_len1;
		ImVec2 cs_uv_c1(x1, ybase + vc1*vdelta);
		if (delta_len0+delta_len1>0.0)
		{
			ImGui::ImageQuad((ImTextureID)texture_id, cs_pos0, cs_pos_c0, cs_pos_c1, cs_pos1, cs_uv0, cs_uv_c0, cs_uv_c1, cs_uv1);
		}
		cs_pos_c0 = cs_pos0 + dir0_nml*(delta_len0-delta_len0/10);
		cs_pos_c1 = cs_pos1 + dir1_nml*(delta_len1-delta_len1/10);
		ybase = y0;
		cs_uv_c0 = { x0, ybase };
		cs_uv_c1 = { x0, ybase };
		ImVec2 cs_pos_s0 = basePpos + _track0[i_cross+1];
		v_seg0 = dir0_len - delta_len0;
		float cs_v_p0 = v_seg0 / _seg_len0;
		ImVec2 cs_uv_s0(x0, ybase + cs_v_p0*vdelta);

		ImVec2 cs_pos_s1 = basePpos + _track1[i_cross+1];
		v_seg1 = dir1_len - delta_len1;
		float cs_v_p1 = v_seg1 / _seg_len1;
		ImVec2 cs_uv_s1(x1, ybase + cs_v_p1*vdelta);
		ImGui::ImageQuad((ImTextureID)texture_id, cs_pos_c0, cs_pos_s0, cs_pos_s1, cs_pos_c1, cs_uv_c0, cs_uv_s0, cs_uv_s1, cs_uv_c1);


		for (int ix = i_cross + 1; ix < pt_cnt - 1; ix++)
		{
			ImVec2 pos0 = basePpos + _track0[ix];
			ImVec2 pos1 = basePpos + _track0[ix + 1];
			ImVec2 pos2 = basePpos + _track1[ix + 1];
			ImVec2 pos3 = basePpos + _track1[ix];
			v_p0 = v_seg0 / _seg_len0;
			v_p1 = v_seg1 / _seg_len1;
			v_seg0 += _track_segs0[ix];
			v_seg1 += _track_segs1[ix];

			v_n0 = v_seg0 / _seg_len0;
			v_n1 = v_seg1 / _seg_len1;

			ImVec2 uv0(x0, ybase + v_p0 * vdelta);
			ImVec2 uv1(x0, ybase + v_n0 * vdelta);
			ImVec2 uv2(x1, ybase + v_n1 * vdelta);
			ImVec2 uv3(x1, ybase + v_p1 * vdelta);
			ImGui::ImageQuad((ImTextureID)texture_id, pos0, pos1, pos2, pos3, uv0, uv1, uv2, uv3);
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