#include "slider_path_picker.h"
#include "res_output.h"
#include <GLFW/glfw3.h>
#include <sstream>
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const float scl) { return ImVec2(lhs.x *scl, lhs.y*scl); }

void slider_path_picker::view()
{
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Slider path picker", &_be_open))
	{
		ImGui::End();
		return;
	}
	static float view_scale = 1.0;
	ImGui::SliderFloat("view scale", &view_scale, 0.2, 20);
	int _txt_index = g_cur_texture_id_index;
	auto isize = g_vres_texture_list[_txt_index].vtexture_coordinates.size();
	bool be_changed = ImGui::Combo("picker target", &_img_id, &get_texture_item, &g_vres_texture_list[_txt_index], isize);
	auto& res_coors = g_vres_texture_list[_txt_index].vtexture_coordinates;
	auto res_w = res_coors[_img_id].owidth();
	auto res_h = res_coors[_img_id].oheight();
	ImGui::Text("original size:%f,%f", res_w, res_h);
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
	int txt_id = g_vres_texture_list[_txt_index].texture_id();
	ImVec2 draw_size(res_w*view_scale, res_h*view_scale);
	float wtxt_w = g_vres_texture_list[_txt_index].texture_width;
	float wtxt_h = g_vres_texture_list[_txt_index].texture_height;
	ImVec2 uv0(res_coors[_img_id]._x0 / wtxt_w, res_coors[_img_id]._y0 / wtxt_h);
	ImVec2 uv1(res_coors[_img_id]._x1 / wtxt_w, res_coors[_img_id]._y1 / wtxt_h);
	ImVec2 offset = ImGui::GetCursorScreenPos();
	ImGui::Image((ImTextureID)txt_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
	//display tracks
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
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
				if (trk_itm.x>res_w)
				{
					trk_itm.x = res_w;
				}
				if (trk_itm.y < 0)
				{
					trk_itm.y = 0;
				}
				if (trk_itm.y > res_h)
				{
					trk_itm.y = res_h;
				}


			}
		}
	}
	ImGui::EndGroup();
	ImGui::End();

}
