#include "file_res_edit.h"
#include "res_output.h"
#include <string>
#include "user_control_imgui.h"
#include <windows.h>
#include <locale.h>  
#include <ShlObj.h>
#include <Commdlg.h>
#include <GL/gl3w.h> 
#include <fstream>
#include "res_internal.h"
shared_ptr<af_file> _pfile{ nullptr };
string _file_key_name;
void file_res_edit::draw_file_res_list()
{
	if (ImGui::Button("Load new file..."))
	{
		OPENFILENAME ofn = { sizeof(OPENFILENAME) };
		ofn.hwndOwner = GetForegroundWindow();
		ofn.lpstrFilter = "file:\0*.*\0\0";
		char strFileName[MAX_PATH] = { 0 };
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = strFileName;
		ofn.nMaxFile = sizeof(strFileName);
		ofn.lpstrTitle = "Loading file...";
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		if (GetOpenFileName(&ofn))
		{
			add_file_to_mfiles_list(string(strFileName));
		}
	}
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = icn_nm_file;
	if (IconTreeNode(icon_str, "file list", node_flags_root))
	{
		for (auto& mf_ut : g_mfiles_list)
		{
			//icon_str = "file";
			auto& keyname = mf_ut.first;
			auto& ttl = mf_ut.second;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (ttl->_sel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, keyname.c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				if (_pfile)
				{
					_pfile->_sel = false;
				}
				ttl->_sel = true;
				_pfile = ttl;
				_file_key_name = keyname;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	if (_pfile&&ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("delete", NULL, false,_pfile.use_count() == 2))
		{
			auto& item_del = g_mfiles_list.find(_file_key_name);
			g_mfiles_list.erase(item_del);
			_pfile = nullptr;
			_file_key_name = "";
		}
		ImGui::EndPopup();
	}
}

void file_res_edit::draw_file_res_item_property()
{
	if (_pfile)
	{
		ImGui::Text("file size:%d", _pfile->_fsize);
	}
}

void file_res_edit::clear_states()
{
     _pfile = nullptr;
     _file_key_name = "";
}
