#include <fstream>
#include <string>
#include "ft_slider.h"
#include "res_output.h"
#include "common_functions.h"
#include <sstream>
namespace auto_future
{
	enum en_slide_direticn
	{
		en_horizontal,
		en_vertical,
		en_custom,
		en_direction_cnt
	};
	float length_of_p2p(af_vec2& p0, af_vec2&p1)
	{
		af_vec2 delta = p1 - p0;
		return delta.norm();
	}
	const char *const direction_iitem[en_direction_cnt] = { "horizontal", "vertical", "custom" };
	bool ft_slider::read_point_position_file(const char *str)
	{
		auto ij = g_mfiles_list.find(str);
		if (ij == g_mfiles_list.end())
		{
			return  false;
		}
		char *_pData = (char *)ij->second->_pbin;
		string str_format(_pData);
		stringstream sstm_format(str_format);
		string line;
		_custom_trace_length = 0;
		auto& ft_rp_v= _custom_envelope;
		af_vec2 mp_prev = { -1.f, 0.f };
		while (getline(sstm_format,line))
		{
			ft_rp_v.emplace_back();
			auto& rp_unit = ft_rp_v.back();
			auto& utp = rp_unit._point0;
			auto& ubp = rp_unit._point1;
			sscanf(line.c_str(), "%f,%f/%f,%f", &utp.x, &utp.y, &ubp.x, &ubp.y);
			_custom_track.emplace_back();
			af_vec2& mid_point = _custom_track.back();
			mid_point = { (utp.x + ubp.x) / 2, (utp.y + ubp.y) / 2 };
			if (mp_prev.x>0)
			{
				auto unit_len = length_of_p2p(mp_prev,mid_point);
				_custom_track_segment.emplace_back(unit_len);
				_custom_trace_length += unit_len;
			}
			mp_prev = mid_point;
		}
		return true;
	}

	ft_slider::ft_slider()
		:ft_base()
	{
		_slider_pt._cbuffer_random_text[0] = '\0';
		//reg_property_handle(&_slider_pt, 0, [this](void*){});
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_slider_pt, 2, [this](void*){
			ImGui::Combo("direction", &_slider_pt._direction_item, direction_iitem, en_direction_cnt);
			if (en_custom == _slider_pt._direction_item)
			{
				if (_custom_envelope.size()>0)
				{
					ImGui::Text("custom path file:%s", _slider_pt._cbuffer_random_text);
					ImGui::SameLine();
					if (ImGui::Button("Delink##custom path"))
					{
						_custom_envelope.clear();
						_custom_track_segment.clear();
						_slider_pt._cbuffer_random_text[0] = '\0';
					}
				}
				else
				{
					ImGui::InputText("custom path file", _slider_pt._cbuffer_random_text, sizeof(_slider_pt._cbuffer_random_text));
					if (ImGui::Button("import"))
					{
						read_point_position_file(_slider_pt._cbuffer_random_text);
					}
				}
			}
		});
#endif
	}
	void ft_slider::link()
	{
		if ( en_custom== _slider_pt._direction_item) //如果保存的是random，这时候就需从文件中读出点数据
		{
			read_point_position_file(_slider_pt._cbuffer_random_text);
		}
	}
	ImVec2 operator +(ImVec2& imv2, af_vec2& afv2)
	{
		return ImVec2(imv2.x + afv2.x, imv2.y + afv2.y);
	}
	
	void ft_slider::draw()
	{
		ft_base::draw();
		int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id();
		vres_txt_cd& ptext_cd = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;

		if (_slider_pt._texture_bg_index_txt >= ptext_cd.size())
		{
			printf("invalid texture index:%d\n", _slider_pt._texture_bg_index_txt);
			_slider_pt._texture_bg_index_txt = 0;
		}
		int texture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
		int texture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;
		float sizew = _slider_pt._bg_txtw;
		float sizeh = _slider_pt._bg_txth;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 screen_base_pos = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos1 = screen_base_pos;
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };
		int bg_txt_id = _slider_pt._texture_bg_index_txt;
		ImVec2 uv0 = ImVec2(ptext_cd[bg_txt_id]._x0 / texture_width, ptext_cd[bg_txt_id]._y0 / texture_height);
		ImVec2 uv1 = ImVec2(ptext_cd[bg_txt_id]._x0 / texture_width, ptext_cd[bg_txt_id]._y1 / texture_height);
		ImVec2 uv2 = ImVec2((ptext_cd[bg_txt_id]._x1) / texture_width, ptext_cd[bg_txt_id]._y1 / texture_height);
		ImVec2 uv3 = ImVec2((ptext_cd[bg_txt_id]._x1) / texture_width, ptext_cd[bg_txt_id]._y0 / texture_height);

		float offsetx = abpos.x - base_pos().x;
		float offsety = abpos.y - base_pos().y;
		ImVec2 axisBasePos = { offsetx + _slider_pt._bg_aposx + winpos.x, offsety + _slider_pt._bg_aposy + winpos.y };
		if (_slider_pt._bg_angle_nml != 0.f)
		{
			pos1 = rotate_point_by_zaxis(pos1, _slider_pt._bg_angle_nml, axisBasePos);
			pos2 = rotate_point_by_zaxis(pos2, _slider_pt._bg_angle_nml, axisBasePos);
			pos3 = rotate_point_by_zaxis(pos3, _slider_pt._bg_angle_nml, axisBasePos);
			pos4 = rotate_point_by_zaxis(pos4, _slider_pt._bg_angle_nml, axisBasePos);
		}
		ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
		auto& txt_hd_id = _slider_pt._texture_head_index_txt;
		/***********************************************************progress*********************************************************/
		if (txt_hd_id >= ptext_cd.size())
		{
			printf("invalid texture index:%d\n", txt_hd_id);
			txt_hd_id = 0;
		}
		af_vec2 value_point0, value_point1,direction_thumb;
		if (en_custom == _slider_pt._direction_item) //任意轨道
		{
			if (0 == _custom_trace_length) return; //第一次进入random时判断
			float tmp_length = 0.f;
			int idx = 0;
			float real_length = _custom_trace_length*_slider_pt._progress_nml;
			ImVec2 uv_org = ImVec2((ptext_cd[txt_hd_id]._x0) / texture_width, (ptext_cd[txt_hd_id]._y0) / texture_height);
			for (auto seg_unit:_custom_track_segment)
			{
				auto test_len = tmp_length + seg_unit;
				if (test_len<=real_length)
				{
					tmp_length = test_len;
					idx++;
					auto& pp_prev = _custom_envelope[idx - 1];
					auto& pp_cur = _custom_envelope[idx];
					pos1 = screen_base_pos + pp_prev._point0;
					pos2 = screen_base_pos + pp_prev._point1;
					pos3 = screen_base_pos + pp_cur._point1;
					pos4 = screen_base_pos + pp_cur._point0;
					uv0 = uv_org + ImVec2(pp_prev._point0.x / texture_width, pp_prev._point0.y / texture_height);
					uv1 = uv_org + ImVec2(pp_prev._point1.x / texture_width, pp_prev._point1.y / texture_height);
					uv2 = uv_org + ImVec2(pp_cur._point1.x / texture_width, pp_cur._point1.y / texture_height);
					uv3 = uv_org + ImVec2(pp_cur._point0.x / texture_width, pp_cur._point0.y / texture_height);
					ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
					if (test_len==real_length)
					{
						value_point0 = pp_cur._point0;
						value_point1 = pp_cur._point1;
						direction_thumb = _custom_track[idx] - _custom_track[idx - 1];
						break;
					}
				}
				else
				{
					float delta = test_len-real_length ;
					auto& pp_cur = _custom_envelope[idx];
					auto& pp_next = _custom_envelope[idx + 1];
					auto cur_seg = _custom_track_segment[idx];
					delta = cur_seg - delta;
					float scale_unit = delta / cur_seg;
					auto dir0 = pp_next._point0 - pp_cur._point0;
					auto dir0_nm = dir0.norm();
					auto dir1 = pp_next._point1 - pp_cur._point1;
					auto dir1_nm = dir1.norm();
					value_point0 = pp_cur._point0 + dir0*(delta/dir0_nm);
					value_point1 = pp_cur._point1 + dir1*(delta/dir1_nm);
					direction_thumb = _custom_track[idx + 1] - _custom_track[idx];
					pos1 = screen_base_pos + pp_cur._point0;
					pos2 = screen_base_pos + pp_cur._point1;
					pos3 = screen_base_pos + value_point1;
					pos4 = screen_base_pos + value_point0;
					uv0 = uv_org + ImVec2(pp_cur._point0.x / texture_width, pp_cur._point0.y / texture_height);
					uv1 = uv_org + ImVec2(pp_cur._point1.x / texture_width, pp_cur._point1.y / texture_height);
					uv2 = uv_org + ImVec2(value_point1.x / texture_width, value_point1.y / texture_height);
					uv3 = uv_org + ImVec2(value_point0.x / texture_width, value_point0.y / texture_height);
					ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
					break;
				}
			}
		}
		else
		{
			if (en_horizontal == _slider_pt._direction_item)
			{
				sizew = _slider_pt._hd_txtw *_slider_pt._progress_nml;
				sizeh = _slider_pt._hd_txth;

				pos1 = screen_base_pos;
				pos2 = { pos1.x, pos1.y + sizeh };
				pos3 = { pos1.x + sizew, pos1.y + sizeh };
				pos4 = { pos1.x + sizew, pos1.y };
				direction_thumb = { 1.0, 0 };
			}
			else if (en_vertical == _slider_pt._direction_item)
			{
				sizew = _slider_pt._hd_txtw;
				sizeh = _slider_pt._hd_txth *_slider_pt._progress_nml;

				pos1 = { abpos.x + winpos.x, abpos.y + winpos.y - sizeh };
				pos2 = { pos1.x, abpos.y + winpos.y };
				pos3 = { pos1.x + sizew, abpos.y + winpos.y };
				pos4 = { pos1.x + sizew, abpos.y + winpos.y - sizeh };
				direction_thumb = { 0, 1.0 };
			}
			value_point0 = {pos3.x,pos3.y};
			value_point1 = {pos4.x,pos4.y};
			uv0 = ImVec2(ptext_cd[txt_hd_id]._x0 / texture_width, ptext_cd[txt_hd_id]._y0 / texture_height);
			uv1 = ImVec2(ptext_cd[txt_hd_id]._x0 / texture_width, (ptext_cd[txt_hd_id]._y1) / texture_height);
			uv2 = ImVec2((ptext_cd[txt_hd_id]._x1) / texture_width, (ptext_cd[txt_hd_id]._y1) / texture_height);
			uv3 = ImVec2((ptext_cd[txt_hd_id]._x1) / texture_width, (ptext_cd[txt_hd_id]._y0) / texture_height);
			if (en_horizontal == _slider_pt._direction_item)
			{
				uv2.x = uv1.x + _slider_pt._progress_nml*(uv2.x - uv1.x);
				uv3.x = uv0.x + _slider_pt._progress_nml*(uv3.x - uv0.x);
			}
			else if (en_vertical == _slider_pt._direction_item)
			{
				uv0.y = uv1.y - _slider_pt._progress_nml*(uv1.y - uv0.y);
				uv3.y = uv2.y - _slider_pt._progress_nml*(uv2.y - uv3.y);
			}
			ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
		}
		/************************************************thumb**************************************************/
		if (!_slider_pt._thumb_visible) return;
		auto tb_height = _slider_pt._tb_height;
		auto tb_dir_nm = direction_thumb.norm();
		auto tb_scale = tb_height / tb_dir_nm;
		af_vec2 dir_delta = direction_thumb*tb_scale;
		af_vec2 value_point2 = value_point1 + dir_delta;
		af_vec2 value_point3 = value_point0 + dir_delta;
		pos1 = screen_base_pos + value_point0;
		pos2 = screen_base_pos + value_point1;
		pos3 = screen_base_pos + value_point2;
		pos4 = screen_base_pos + value_point3;
		auto tb_id = _slider_pt._texture_thumb_index_txt;
		uv0 = ImVec2(ptext_cd[tb_id]._x0 / texture_width, ptext_cd[tb_id]._y0 / texture_height);
		uv1 = ImVec2(ptext_cd[tb_id]._x0 / texture_width, (ptext_cd[tb_id]._y1) / texture_height);
		uv2 = ImVec2((ptext_cd[tb_id]._x1) / texture_width, (ptext_cd[tb_id]._y1) / texture_height);
		uv3 = ImVec2((ptext_cd[tb_id]._x1) / texture_width, (ptext_cd[tb_id]._y0) / texture_height);
		ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
	}
}
