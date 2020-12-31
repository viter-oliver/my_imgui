#include "slider_path_picker.h"
#include "common_functions.h"
#include "res_output.h"
#include <GLFW/glfw3.h>
#include <sstream>
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const float scl) { return ImVec2(lhs.x *scl, lhs.y*scl); }
static float leng_of_imv(ImVec2& vt)
{
	return sqrt(vt.x*vt.x+vt.y*vt.y);
}
static float length_of_p2p(ImVec2& p0, ImVec2&p1)
{
	ImVec2 delta = p1 - p0;
	return leng_of_imv(delta);
}
void slider_path_picker::view()
{
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
	auto vtx_sz = g_vres_texture_list.size();
	if (vtx_sz==0)
	{
		return;
	}
	static char track_file_name[FILE_NAME_LEN];
	if (_cur_tacks_file_name.empty())
	{
		ImGui::InputText("track file name", track_file_name, FILE_NAME_LEN);
		if (ImGui::Button("import"))
		{
			auto ifl = g_mfiles_list.find(track_file_name);
			if (ifl==g_mfiles_list.end())
			{
				track_file_name[0] = '\0';
				return;
			}
			char* pData = (char*)ifl->second->_pbin;
			auto buff_sz = ifl->second->_fsize;
			auto alen = conver_track_buff_to_pair(pData, buff_sz, _vtrack0, _vtrack1);
			if (alen==0)
			{
				track_file_name[0] = '\0';
				return;
			}
			_cur_tacks_file_name = track_file_name;
		}
	}
	else
	{
		ImGui::Text("track file name:%s", _cur_tacks_file_name.c_str());
	}
	auto alen = _vtrack0.size();

	if (ImGui::Button("Save to track file"))
	{
		auto asize = sizeof(ImVec2)*alen;
		GLuint file_sz = asize * 2 + 4;
		if (_cur_tacks_file_name.empty())
		{
			if (strlen(track_file_name)>0)
			{
				string key_tmp(track_file_name);
				auto file_key_name = find_a_key_from_mp(g_mfiles_list, key_tmp);
				auto ps_file = make_shared<af_file>(file_sz);
				int *file_head = (int*)ps_file->_pbin;
				*file_head = alen;
				char* buff_head = (char*)ps_file->_pbin + 4;
				memcpy(buff_head, &_vtrack0[0], asize);
				memcpy(buff_head + asize, &_vtrack1[0], asize);
				g_mfiles_list[file_key_name] = ps_file;
				save_ojfile_to_file(file_key_name);
				track_file_name[0] = '\0';
				_vtrack0.clear();
				_vtrack1.clear();
			}
		}
		else
		{
			auto ifl = g_mfiles_list.find(_cur_tacks_file_name);
			auto& objf=*ifl->second;
			if (objf._fsize!=file_sz)
			{
				objf.re_alloc(file_sz);
			}
			int *file_head = (int*)objf._pbin;
			*file_head = alen;
			char* buff_head = (char*)objf._pbin + 4;
			memcpy(buff_head, &_vtrack0[0], asize);
			memcpy(buff_head + asize, &_vtrack1[0], asize);
			save_ojfile_to_file(_cur_tacks_file_name);
			_cur_tacks_file_name.clear();
			_vtrack0.clear();
			_vtrack1.clear();
		}
	}	
	if (!_cur_tacks_file_name.empty())
	{
		ImGui::SameLine();
		if (ImGui::Button("Abandon"))
		{
			_cur_tacks_file_name.clear();
			_vtrack0.clear();
			_vtrack1.clear();
		}
	}
	static float view_scale = 1.0;
	ImGui::SliderFloat("view scale", &view_scale, 0.2, 20);
	static float range_w{ 0. }, range_h{0.};
	static bool trak_a_image = true;
     auto& res_gp = *g_vres_texture_list[ g_cur_texture_id_index ];
     auto isize = res_gp.vtexture_coordinates.size();
	ImGui::Checkbox("Track a image", &trak_a_image);
     auto& res_coors = res_gp.vtexture_coordinates;
	if (trak_a_image)
	{
          bool be_changed = ImGui::Combo( "picker target", &_img_id, &get_texture_item, &res_gp, isize );
		if (be_changed)
		{
               auto& icon_name = res_gp.vtexture_coordinates[ _img_id ]._file_name;
			string tr_strk_name = icon_name.substr(0, icon_name.find_last_of('.'));
			tr_strk_name += ".trk";
			strcpy(track_file_name, tr_strk_name.c_str());
		}
		range_w = res_coors[_img_id].owidth();
		range_h = res_coors[_img_id].oheight();
		ImGui::Text("original size:%f,%f", range_w, range_h);
	}
	else
	{
		ImGui::SliderFloat("Width of range", &range_w, 100, 2000);
		ImGui::SliderFloat("Height of range", &range_h, 100, 2000);

	}
	alen = _vtrack0.size();
	ImGui::SliderInt("Pair id", &_pair_id, 0, alen - 1);
	if (_pair_id<alen)
	{
		ImGui::SliderFloat2("left point", (float*)&_vtrack0[_pair_id], 0,range_w);
		ImGui::SliderFloat2("right point", (float*)&_vtrack1[_pair_id], 0, range_w);

	}
	static bool smooth_x=true;
	ImGui::Checkbox("Smooth x:", &smooth_x);
	static const int algorithm_cnt = 3;
	static int algorithm_id = 0;
	static bool(*smooth_algorithm[algorithm_cnt])(vector<ImVec2>&, bool) =
	{
		smooth_algorithm_5_points_3_times,
		smooth_algoritm_3_points_average,
		smooth_algorithm_5_points_average
	};
	static const char* smooth_algorithm_name[algorithm_cnt] =
	{
		"5 points and 3 times",
		"average of 3 points",
		"average of 5 points",
	};
	ImGui::Combo("smooth algorithm:", &algorithm_id, smooth_algorithm_name,algorithm_cnt);
	if (ImGui::Button("Smooth the track"))
	{
		vector<float> tk_seg_len;
		int tlen = _vtrack0.size();
		tk_seg_len.resize(tlen);
		if (smooth_x)
		{
			for (int ix = 0; ix < tlen;ix++)
			{
				tk_seg_len[ix] = _vtrack1[ix].x - _vtrack0[ix].x;
			}
			smooth_algorithm[algorithm_id](_vtrack0,true);
			for (int ix = 0; ix < tlen;ix++)
			{
				_vtrack1[ix].x = _vtrack0[ix].x + tk_seg_len[ix];
			}
		}
		else
		{
			for (int ix = 0; ix < tlen; ix++)
			{
				tk_seg_len[ix] = _vtrack1[ix].y - _vtrack0[ix].y;
			}
			smooth_algorithm[algorithm_id](_vtrack0, false);
			for (int ix = 0; ix < tlen; ix++)
			{
				_vtrack1[ix].y = _vtrack0[ix].y + tk_seg_len[ix];
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Level the track"))
	{
		int tlen = _vtrack0.size();
		if (tlen>2)
		{

			float wd0 = length_of_p2p(_vtrack1[0], _vtrack0[0]);
			float wd1 = length_of_p2p(_vtrack1[1], _vtrack0[1]);
			float wdelta = wd1 - wd0;
			float wdnm = 1 / wd0;
			ImVec2 dir = (_vtrack1[0] - _vtrack0[0])*wdnm;
			for (int ix = 1; ix < tlen; ix++)
			{
				wd0 += wdelta;
				_vtrack1[ix] = _vtrack0[ix] + dir*wd0;
			}
		}
	}
	
	ImGui::SameLine();
	if (ImGui::Button("Average the height"))
	{
		int tlen = _vtrack0.size();
		if (tlen > 2)
		{
			float hd0 = _vtrack0[1].y - _vtrack0[0].y;
			float hd1 = _vtrack0[2].y - _vtrack0[1].y;
			float hdelta = hd1 - hd0;
			float y0 = _vtrack0[2].y;
			float y1 = _vtrack1[2].y;
			for (int ix = 3; ix < tlen; ix++)
			{
				hd1 += hdelta;
				y0 += hd1;
				y1 += hd1;
				_vtrack0[ix].y = y0;
				_vtrack1[ix].y = y1;
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Straighten"))
	{
		int tlen = _vtrack0.size();
		if (tlen > 2)
		{
			ImVec2 wdir = _vtrack0[0] - _vtrack0[tlen - 1];
			float wlen = leng_of_imv(wdir);
			float plen = wlen / (tlen - 1);
			float wlen_nm = 1/wlen;
			ImVec2 wdir_nm = wdir*wlen_nm;
			for (int ix = 1; ix < tlen;ix++)
			{
				_vtrack0[ix] = _vtrack0[0] - wdir_nm*plen*ix;
				_vtrack1[ix] = _vtrack1[0] - wdir_nm*plen*ix;
			}
		}
	}
	bool be_short_cut_add = ImGui::GetIO().KeyCtrl&&ImGui::GetIO().KeysDown[GLFW_KEY_A];
	static bool be_adding = false;
	if (!be_short_cut_add)
	{
		be_adding = false;
	}
	if (ImGui::Button("add new pair of point") || be_short_cut_add&&!be_adding)
	{
		be_adding = true;
		_vtrack0.emplace_back();
		auto& pt0 = _vtrack0.back();
		_vtrack1.emplace_back();
		auto& pt1 = _vtrack1.back();
		auto tsz = _vtrack0.size();
		if (tsz == 1)
		{
			pt0 = { 0, 20 };
		    pt1 = { 20, 0 };
		}
		else
		{
			pt0 = _vtrack0[tsz - 2] - ImVec2(5.0, 5.0);
			if (pt0.x<0)
			{
				pt0.x = 0;
			}
			if (pt0.y<0)
			{
				pt0.y = 0;
			}
			pt1 = _vtrack1[tsz - 2] - ImVec2(5.0, 5.0);
			if (pt1.x < 0)
			{
				pt1.x = 0;
			}
			if (pt1.y < 0)
			{
				pt1.y = 0;
			}

		}

	}
	ImGui::BeginGroup();
	ImVec2 offset = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 draw_size(range_w*view_scale, range_h*view_scale);
	if (trak_a_image)
	{
          int txt_id = res_gp.texture_id();
          float wtxt_w = res_gp.texture_width;
          float wtxt_h = res_gp.texture_height;
		ImVec2 uv0(res_coors[_img_id]._x0 / wtxt_w, res_coors[_img_id]._y0 / wtxt_h);
		ImVec2 uv1(res_coors[_img_id]._x1 / wtxt_w, res_coors[_img_id]._y1 / wtxt_h);
		ImGui::Image((ImTextureID)txt_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
	}
	else
	{
		ImVec2 pos0 = offset;
		ImVec2 pos1 = offset + draw_size;
		draw_list->AddRect(pos0, pos1, IM_COL32(255, 255, 0, 255));
	}
	//display tracks
	int idm = _vtrack0.size() - 1;
	ImU32 GRID_COLOR = IM_COL32(200, 250, 250, 250);
	const float pt_radius = 4.;
	for (int ix = 0; ix < idm;++ix)
	{
		ImVec2 tracks[4];
		auto& pt0 = _vtrack0[ix];
		tracks[0] = pt0*view_scale + offset;
		auto& ptn0 = _vtrack0[ix + 1];
		tracks[1] = ptn0*view_scale + offset;
		auto& pt1 = _vtrack1[ix];
		tracks[2] = pt1*view_scale + offset;
		auto& ptn1 = _vtrack1[ix + 1];
		tracks[3] = ptn1*view_scale + offset;
		draw_list->AddQuad(tracks[0], tracks[1], tracks[3], tracks[2], GRID_COLOR,2.f);
	}
	for (int ix = 0; ix < idm+1; ix++)
	{
		ImVec2* tracks[2];
		tracks[0] = &_vtrack0[ix];
		tracks[1] =  &_vtrack1[ix];;
		for (int ii = 0; ii < 2;++ii)
		{
			ImVec2 trkPt = *tracks[ii] * view_scale + offset;
			draw_list->AddCircleFilled(trkPt, pt_radius, IM_COL32(200, 150, 150, 250));
			ImVec2 btnpos = trkPt - ImVec2(4.,4.);
			ImGui::SetCursorScreenPos(btnpos);
			stringstream stm_tmp;
			stm_tmp << ix << "_" << ii;
			string btn_cap = stm_tmp.str();
			ImGui::InvisibleButton(btn_cap.c_str(), ImVec2(8., 8.));
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				float rev_scl = 1 / view_scale;
				auto& trk_itm = *tracks[ii];
				trk_itm = trk_itm + ImGui::GetIO().MouseDelta*rev_scl;
				if (trk_itm.x<0)
				{
					trk_itm.x = 0;
				}
				if (trk_itm.x>range_w)
				{
					trk_itm.x = range_w;
				}
				if (trk_itm.y < 0)
				{
					trk_itm.y = 0;
				}
				if (trk_itm.y > range_h)
				{
					trk_itm.y = range_h;
				}


			}
		}
	}
	ImGui::EndGroup();
}
