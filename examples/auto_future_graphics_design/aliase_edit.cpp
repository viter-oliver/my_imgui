#include "aliase_edit.h"
#include "user_control_imgui.h"
#include "common_functions.h"
#include "ft_base.h"
#include "res_internal.h"
static char sal_name_str[FILE_NAME_LEN] = "";
static char* new_aliase = "New alias";
static char* edit_aliase = "Edit alias";
void aliase_edit::aliase_dic_view()
{
	string icon_str = icn_aliase;
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	if (IconTreeNode(icon_str, "Aliases list", node_flags_root))
	{
		for (auto& ialiase : g_aliase_dic)
		{
			auto& keyname = ialiase.first;
			auto& ps_prp_pos = ialiase.second;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (_psel&&*ps_prp_pos == *_psel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, keyname.c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				_key_name = keyname;
				_psel =ps_prp_pos;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	if (_psel&&ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("delete",NULL,false))
		{
			auto ialias = g_aliase_dic.find(_key_name);
			if (ialias!=g_aliase_dic.end())
			{
				g_aliase_dic.erase(ialias);
				_psel = nullptr;
				_key_name = "";
			}
		}
		if (ImGui::MenuItem("rename",NULL,false))
		{
			_popup_cap = edit_aliase;
			_edit_prp_ele_pos = *_psel;
               if( !_key_name.empty() )
               {
                    strcpy( sal_name_str, _key_name.c_str() );
               }
		}
		ImGui::EndPopup();
	}
}

void aliase_edit::aliase_item_propoerty()
{
	if (_psel)
	{
          string item_key = _key_name + "->";
          ImGui::Text( item_key.c_str() );
          ImGui::SameLine();
		auto& pobj = _psel->_pobj;
		auto& pgidx = _psel->_page_index;
		auto& fdidx = _psel->_field_index;
		string cur_name;
		get_uic_path(pobj, cur_name);
		auto& cur_field = pobj->get_filed_ele(pgidx, fdidx);
		cur_name += "::";
		cur_name += cur_field._name;
		ImGui::Text(cur_name.c_str());
	}
}		

void aliase_edit::popup_new_aliase()
{
	if (_edit_prp_ele_pos._pobj)
	{
		ImGui::OpenPopup(_popup_cap.c_str());
          
	}
	if (ImGui::BeginPopupModal(_popup_cap.c_str()))
	{
		auto& pobj = _edit_prp_ele_pos._pobj;
		auto& pgidx = _edit_prp_ele_pos._page_index;
		auto& fdidx = _edit_prp_ele_pos._field_index;
		string cur_name;
		get_uic_path(pobj,cur_name);
		auto& cur_field = pobj->get_filed_ele(pgidx, fdidx);
		cur_name += "::";
		cur_name += cur_field._name;
		ImGui::Text(cur_name.c_str());
		ImGui::InputText("enter a name for the aliase", sal_name_str, FILE_NAME_LEN);
		string btn_cap = _key_name == "" ? "Add" : "Edit";
		if (ImGui::Button(btn_cap.c_str()))
		{
			if (_key_name=="")
			{
				string str_sal_nm = sal_name_str;
				string str_aliase_key = find_a_key_from_mp(g_aliase_dic, str_sal_nm);
				auto ps_new_aliase = make_shared<prop_ele_position>();
				*ps_new_aliase = _edit_prp_ele_pos;
				g_aliase_dic[str_aliase_key] = ps_new_aliase;
				_edit_prp_ele_pos._pobj = nullptr;
				_psel = ps_new_aliase;
				_key_name = str_aliase_key;
			}
			else
			{
				auto ialias = g_aliase_dic.find(_key_name);
				auto ps_alias = ialias->second;
				g_aliase_dic.erase(ialias);
				string str_sal_nm = sal_name_str;
				string str_aliase_key = find_a_key_from_mp(g_aliase_dic, str_sal_nm);
				g_aliase_dic[str_aliase_key] = ps_alias;
				_edit_prp_ele_pos._pobj = nullptr;
				_psel = ps_alias;
				_key_name = str_aliase_key;
			}


			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			_edit_prp_ele_pos._pobj = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
void aliase_edit::sel_aliase(base_ui_component* pobj, int pg_idx, int fd_idx)
{
	auto ialias = find_if(g_aliase_dic.begin(), g_aliase_dic.end(), [pobj,pg_idx,fd_idx](pair<string, ps_prp_ele_pos> tar){
		auto& tar_pos = *tar.second;
		return tar_pos._pobj == pobj&&tar_pos._page_index == pg_idx&&tar_pos._field_index == fd_idx;
	});
	if (ialias!=g_aliase_dic.end())
	{
		_psel = ialias->second;
		_key_name = ialias->first;
	}
	else
	{
		_edit_prp_ele_pos = { pobj, pg_idx, fd_idx };
		sal_name_str[0] = '\0';
		_popup_cap = new_aliase;
		_key_name = "";
	}
}
