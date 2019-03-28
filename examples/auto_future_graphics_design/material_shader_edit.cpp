#include "material_shader_edit.h"
#include "af_shader.h"
#include "af_material.h"
#include "user_control_imgui.h"
#include <fstream>
#include "dir_output.h"
#include "res_internal.h"
#define FILE_NAME_LEN 50
extern string g_cureent_directory;

shared_ptr<af_shader> pshd_sel = nullptr;
shared_ptr<material> pmateral_sel = nullptr;

void material_shader_edit::draw_shader()
{
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = icn_nm_shader;
	if (IconTreeNode(icon_str, "shader list", node_flags_root))
	{
		for (auto& shd_ut : g_af_shader_list)
		{
            icon_str = icn_nm_shader_source;
			auto& shd_ss = shd_ut.second;
			if (IconTreeNode(icon_str, shd_ss->get_name().c_str(), node_flags_root))
			{
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
				//icon_str = "vertex_shader_source";
				if (shd_ss->_vs_selected)
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}
				IconTreeNode(icon_str, shd_ss->_vs_name.c_str(), node_flags);
				if (ImGui::IsItemClicked())
				{
					if (pshd_sel)
					{
						pshd_sel->reset_sel();
					}
					shd_ss->_vs_selected = true;
					pshd_sel = shd_ss;
				}
				ImGui::TreePop();
				node_flags = ImGuiTreeNodeFlags_Leaf;
				//icon_str = "fragment_shader_source";
				if (shd_ss->_fs_selected)
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}
				IconTreeNode(icon_str, shd_ss->_fs_name.c_str(), node_flags);
				if (ImGui::IsItemClicked())
				{
					if (pshd_sel)
					{
						pshd_sel->reset_sel();
					}
					shd_ss->_fs_selected = true;
					pshd_sel = shd_ss;
				}
				ImGui::TreePop();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::BeginPopupContextWindow())
	{
		if (pshd_sel&&ImGui::MenuItem("create material", NULL, false))
		{
			ImGui::OpenPopup("create material");
			if (ImGui::BeginPopupModal("create material"))
			{
				static char mt_name_str[FILE_NAME_LEN] = "";
				ImGui::InputText("enter a name for the new material", mt_name_str, FILE_NAME_LEN);
				if (ImGui::Button("Add"))
				{
					shared_ptr<material> pmaterial = make_shared<material>(pshd_sel);
					pmaterial->set_name(mt_name_str);
					g_material_list[mt_name_str] = pmaterial;
					memset(mt_name_str, 0, FILE_NAME_LEN);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
		ImGui::EndPopup();
	}
}
string compile_info;
void material_shader_edit::load_shader()
{
	if (ImGui::Button("create shader"))
		ImGui::OpenPopup("create shader");
	if (ImGui::BeginPopupModal("create shader"))
	{
		static char shd_name_str[FILE_NAME_LEN] = "";
		static char vs_name_str[FILE_NAME_LEN] = "";
		static char fs_name_str[FILE_NAME_LEN] = "";
		ImGui::InputText("shader name", shd_name_str, FILE_NAME_LEN);
		ImGui::InputText("vertex shader file", vs_name_str, FILE_NAME_LEN);
		ImGui::InputText("fragment shader file", fs_name_str, FILE_NAME_LEN);			
		if (ImGui::Button("add new shader"))
		{
			string str_shader_file = g_cureent_directory+shaders_fold;
			string vs_file = str_shader_file + vs_name_str;
			string fs_file = str_shader_file + fs_name_str;
			string vs_code, fs_code;
			ifstream ifs;
			ifs.open(vs_file);
			if (!ifs)
			{
				ifs.close();
				MessageBox(GetForegroundWindow(), vs_file.c_str(), "compair vertex file failed", MB_OK);
				goto COMPAIR_SHADER_END;
			}
			filebuf* pbuf = ifs.rdbuf();
			size_t sz_code = pbuf->pubseekoff(0, ifs.end, ifs.in);
			pbuf->pubseekpos(0, ifs.in);
			vs_code.reserve(sz_code);
			vs_code.resize(sz_code);
			pbuf->sgetn(&vs_code[0], sz_code);
			ifs.close();
			ifs.open(fs_file);
			if (!ifs)
			{
				ifs.close();
				MessageBox(GetForegroundWindow(), fs_file.c_str(), "compair fragment file failed", MB_OK);
				goto COMPAIR_SHADER_END;
			}
			pbuf = ifs.rdbuf();
			sz_code = pbuf->pubseekoff(0, ifs.end, ifs.in);
			pbuf->pubseekpos(0, ifs.in);
			fs_code.reserve(sz_code);
			fs_code.resize(sz_code);
			pbuf->sgetn(&fs_code[0], sz_code);
			ifs.close();
COMPAIR_SHADER_END:
			shared_ptr<af_shader> pshd = make_shared<af_shader>(vs_code.c_str(), fs_code.c_str());
			if (pshd->is_valid())
			{
				//compile_info = "";
				pshd->set_name(shd_name_str);
				g_af_shader_list[shd_name_str]=pshd;
				pshd->_vs_name = vs_name_str;
				pshd->_fs_name = fs_name_str;
				
			}
			else
			{
				MessageBox(GetForegroundWindow(), "compair shader file failed, please compair again after check!", "error", MB_OK);
			}
			memset(vs_name_str, 0, FILE_NAME_LEN);
			memset(fs_name_str, 0, FILE_NAME_LEN);
			memset(shd_name_str, 0, FILE_NAME_LEN);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
void material_shader_edit::load_shader_info()
{
	ImGui::TextWrapped(compile_info.c_str());
}
void material_shader_edit::draw_shader_item_property()
{
	if (pshd_sel)
	{
		if (ImGui::Button("Refresh"))
		{
			string str_shader_file = g_cureent_directory+shaders_fold;
			string vs_file = str_shader_file + pshd_sel->_vs_name;
			string fs_file = str_shader_file + pshd_sel->_fs_name;
			string vs_code, fs_code;
			ifstream ifs;
			ifs.open(vs_file);
			filebuf* pbuf = ifs.rdbuf();
			size_t sz_code = pbuf->pubseekoff(0, ifs.end, ifs.in);
			pbuf->pubseekpos(0, ifs.in);
			vs_code.reserve(sz_code);
			vs_code.resize(sz_code);
			pbuf->sgetn(&vs_code[0], sz_code);
			ifs.close();
			ifs.open(fs_file);
			pbuf = ifs.rdbuf();
			sz_code = pbuf->pubseekoff(0, ifs.end, ifs.in);
			pbuf->pubseekpos(0, ifs.in);
			fs_code.reserve(sz_code);
			fs_code.resize(sz_code);
			pbuf->sgetn(&fs_code[0], sz_code);
			ifs.close();
			pshd_sel->refresh_sourcecode(vs_code, fs_code);
			refresh_material(pshd_sel);
		}
		if (pshd_sel->is_valid())
		{
			if (pshd_sel->_vs_selected)
			{
				ImGui::TextWrapped(pshd_sel->get_vs_code().c_str());
			}
			else
			if (pshd_sel->_fs_selected)
			{
				ImGui::TextWrapped(pshd_sel->get_fs_code().c_str());
			}
		}
		else
		{
			if (pshd_sel->_vs_selected || pshd_sel->_fs_selected)
			{
				ImGui::TextWrapped(pshd_sel->compile_error_info.c_str());
			}
		}
		
		
	}
}
void material_shader_edit::draw_material()
{
	if (pshd_sel&&ImGui::Button("create material"))
		ImGui::OpenPopup("create material");
	if (ImGui::BeginPopupModal("create material"))
	{
		static char mt_name_str[FILE_NAME_LEN] = "";
		ImGui::InputText("enter a name for the new material", mt_name_str, FILE_NAME_LEN);
		if (ImGui::Button("Add"))
		{
			shared_ptr<material> pmaterial = make_shared<material>(pshd_sel);
			pmaterial->set_name(mt_name_str);
			g_material_list[mt_name_str] = pmaterial;
			memset(mt_name_str, 0, FILE_NAME_LEN);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = icn_nm_material;
	if (IconTreeNode(icon_str, "material list", node_flags_root))
	{
		for (auto& mtl_ut : g_material_list)
		{
			auto& mtl = mtl_ut.second;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (mtl->_sel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, mtl->get_name().c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				if (pmateral_sel)
				{
					pmateral_sel->_sel = false;
				}
				mtl->_sel = true;
				pmateral_sel = mtl;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

}

void material_shader_edit::draw_material_item_property()
{
	if (pmateral_sel)
	{
		pmateral_sel->edit_ufs();
	}
}

