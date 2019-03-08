#include "state_manager_edit.h"
#include "res_internal.h"
#include "user_control_imgui.h"
#include "common_functions.h"
#include "ft_base.h"
#include <sstream>
//#include <algorithm>

state_manager_edit::state_manager_edit()
{
}


state_manager_edit::~state_manager_edit()
{
}

void state_manager_edit::view_state_managers()
{
	static char sm_name_str[FILE_NAME_LEN] = "";
	if (ImGui::Button("New state manager..."))
	{
		sm_name_str[0] = '\0';
		ImGui::OpenPopup("new state manager");
	}
	if (ImGui::BeginPopupModal("new state manager"))
	{
		ImGui::InputText("enter a name for the new state manger", sm_name_str, FILE_NAME_LEN);
		if (ImGui::Button("Add"))
		{
			string str_sm_nm = sm_name_str;
			string str_sm_key = find_a_key_from_mp(g_mstate_manager, str_sm_nm);
			auto ps_state_mg = make_shared<af_state_manager>();
			g_mstate_manager[sm_name_str] = ps_state_mg;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	string icon_str = icn_nm_state_manager;
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	if (IconTreeNode(icon_str, "State manager list", node_flags_root))
	{
		for (auto& ist_mg:g_mstate_manager)
		{
			auto& keyname = ist_mg.first;
			auto& pst_mg = ist_mg.second;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (pst_mg->_sel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, keyname.c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				if (_psel)
				{
					_psel->_sel = false;
				}
				pst_mg->_sel = true;
				_psel = pst_mg;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void state_manager_edit::view_state_manager_item_property()
{
	if (!_psel)
	{
		return;
	}
	auto& pplist = _psel->_prop_list;
	int idx = 0;
	for (auto itp = pplist.begin(); itp != pplist.end();idx++)
	{
		auto& prop_ele_p = *itp;
		string prop_ele_show;
		get_uic_path(prop_ele_p._pobj, prop_ele_show);
		auto& pgidx = prop_ele_p._page_index;
		auto& fidx = prop_ele_p._field_index;
		auto& pt_pg = prop_ele_p._pobj->get_prop_ele(pgidx);
		field_ele &fel = prop_ele_p._pobj->get_filed_ele(pgidx, fidx);
		prop_ele_show += "::";
		prop_ele_show += fel._name;
		ImGui::Text(prop_ele_show.c_str());
		ImGui::SameLine();
		stringstream stream_delbtn;
		stream_delbtn << "X##"<<idx;
		string str_delbtn = stream_delbtn.str();
		if (ImGui::Button(str_delbtn.c_str()))
		{
			itp = pplist.erase(itp);
		}
		else
		{
			itp++;
		}
	}
	auto& pp_value_list = _psel->_prop_value_list;
	auto& cur_state = _psel->_state_idx;
	auto state_cnt = pp_value_list.size;
	for (idx = 0; idx < state_cnt;idx++)
	{
		stringstream stm_sel;
		stm_sel << "State" << idx;
		string str_sel = stm_sel.str();
		if (ImGui::Selectable(str_sel.c_str(),cur_state==idx))
		{
			cur_state = idx;
		}
		ImGui::SameLine();
		stm_sel << L"¡ý";
		stm_sel << "##" << idx;
		str_sel = stm_sel.str();
		if (ImGui::Button(str_sel.c_str()))
		{
			auto ppcnt = pplist.size();
			for (auto& pvale:pp_value_list)
			{
				pvale.resize(ppcnt);
			}

		}
		if (idx<state_cnt-1)
		{
			stm_sel << "<-->##" << idx;
			str_sel = stm_sel.str();
			ImGui::SameLine();
			if (ImGui::Button(str_sel.c_str()))
			{

			}
			ImGui::SameLine();

		}
	}
	if (pplist.size()>0&& ImGui::Button("Add new state..."))
	{
		pp_value_list.emplace_back();
//		auto& nvprop_block = pp_value_list[state_cnt];
	}

}
