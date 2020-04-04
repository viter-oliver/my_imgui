#include "playlist_group_edit.h"
#include "ft_base.h"
#include "common_functions.h"
#include "user_control_imgui.h"
#include "res_internal.h"
#include <sstream>
void playlist_group_edit::set_dragging(bool be_dragging, string stm_name,int playlist_id)
{
	if (!_sl_playlist_group_unit)
	{
		return;
	}
	if (!be_dragging&&_hit_sm_window)
	{
		auto& cur_pgu = *_sl_playlist_group_unit;
		auto ifd = find_if(cur_pgu.begin(), cur_pgu.end(),\
			[stm_name](playlist_unit pu){return pu._st_name == stm_name; });
		if (ifd==cur_pgu.end())
		{
			playlist_unit pu(_sel_statemanager_name, _playlist_id);
			cur_pgu.emplace_back(pu);
		}
		_hit_sm_window = false;
	}
	if (be_dragging)
	{
		_sel_statemanager_name = stm_name;
		_playlist_id = playlist_id;
	}
	_be_draging = be_dragging;
}
static char sal_name_str[FILE_NAME_LEN] = "";
void playlist_group_edit::view_playlit_group_list()
{
	if (ImGui::Button("Create new playlist group..."))
	{
		ImGui::OpenPopup("Create new playlist group");
		memset(sal_name_str, 0, FILE_NAME_LEN);
	}
	if (ImGui::BeginPopupModal("Create new playlist group"))
	{
		ImGui::InputText("The name of new playlist group", sal_name_str, FILE_NAME_LEN);
		if (ImGui::Button("Save"))
		{
			auto ipl = make_shared<playlist_unit_list>();
			string prekn = sal_name_str;
			string key_cv = find_a_key_from_mp(g_playlist_group_list, prekn);
			g_playlist_group_list[key_cv] = ipl;
			_key_name = key_cv;
			_sl_playlist_group_unit = ipl;

			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	string icon_str = icn_shared_value;
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	if (IconTreeNode(icon_str, "playlist group list", node_flags_root))
	{
		for (auto& icv : g_playlist_group_list)
		{
			auto& keyname = icv.first;
			auto& sp_pgu = icv.second;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (_sl_playlist_group_unit&&sp_pgu == _sl_playlist_group_unit)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, keyname.c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				_key_name = keyname;
				_sl_playlist_group_unit = sp_pgu;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	if (_sl_playlist_group_unit&&ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("delete", NULL, false))
		{
			auto icv = g_playlist_group_list.find(_key_name);
			if (icv != g_playlist_group_list.end())
			{
				g_playlist_group_list.erase(icv);
				_sl_playlist_group_unit = nullptr;
				_key_name = "";
			}
		}

		ImGui::EndPopup();
	}
}

void playlist_group_edit::playlist_group_item()
{
	if (!_sl_playlist_group_unit)
	{
		return;
	}
	if (ImGui::Button("play the playlist group"))
	{
		play_playlist_group(_key_name);
	}
	_hit_sm_window = _be_draging&&ImGui::IsMouseHoveringWindow();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	auto& play_gp_lis = *_sl_playlist_group_unit;
	int idx = 0;
	for (auto itp = play_gp_lis.begin(); itp != play_gp_lis.end(); idx++)
	{
		auto&  play_gp = *itp;
		string play_gp_ele_show;
		play_gp_ele_show = play_gp._st_name;
		play_gp_ele_show += "--";
		stringstream stm_pgp;
		stm_pgp << play_gp._playlist_id;
		play_gp_ele_show += stm_pgp.str();
		ImGui::Text(play_gp_ele_show.c_str());
		ImGui::SameLine();
		stringstream stream_delbtn;
		stream_delbtn << "X##" << idx;
		string str_delbtn = stream_delbtn.str();
		if (ImGui::Button(str_delbtn.c_str()))
		{
			itp = play_gp_lis.erase(itp);
		}
		else
		{
			itp++;
		}
	}
}
