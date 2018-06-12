#include "material_shader_edit.h"
#include "af_shader.h"
#include "material.h"
#include "user_control_imgui.h"
#include <fstream>
#define FILE_NAME_LEN 50
extern string g_cureent_project_file_path;
const char* str_shaders_fold = "shaders\\";

shared_ptr<af_shader> pshd_sel = nullptr;
shared_ptr<material> pmateral_sel = nullptr;

void material_shader_edit::draw_shader()
{
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = "shader_list";
	if (IconTreeNode(icon_str, "shader list", node_flags_root))
	{
		for (auto& shd_ut : g_af_shader_list)
		{
            icon_str = "shader_source";
			auto& shd_ss = shd_ut.second;
			if (IconTreeNode(icon_str, shd_ss->get_name().c_str(), node_flags_root))
			{
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
				icon_str = "vertex_shader_source";
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
				icon_str = "fragment_shader_source";
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
			string str_shader_file = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
			str_shader_file += str_shaders_fold;
			string vs_file = str_shader_file + vs_name_str;
			string fs_file = str_shader_file + fs_name_str;
			ifstream ifs;
			ifs.open(vs_file);
			filebuf* pbuf = ifs.rdbuf();
			size_t sz_code = pbuf->pubseekoff(0, ifs.end, ifs.in);
			pbuf->pubseekpos(0, ifs.in);
			vs_file.reserve(sz_code);
			vs_file.resize(sz_code);
			pbuf->sgetn(&vs_file[0], sz_code);
			ifs.close();
			ifs.open(fs_file);
			pbuf = ifs.rdbuf();
			sz_code = pbuf->pubseekoff(0, ifs.end, ifs.in);
			pbuf->pubseekpos(0, ifs.in);
			fs_file.reserve(sz_code);
			fs_file.resize(sz_code);
			pbuf->sgetn(&fs_file[0], sz_code);
			ifs.close();
			shared_ptr<af_shader> pshd = make_shared<af_shader>(vs_file.c_str(), fs_file.c_str());
			if (!pshd->is_valid())
			{
				compile_info = pshd->compile_error_info;
			}
			else
			{
				compile_info = "";
				pshd->set_name(shd_name_str);
				g_af_shader_list[shd_name_str]=pshd;
				pshd->_vs_name = vs_name_str;
				pshd->_fs_name = fs_name_str;
				memset(vs_name_str, 0, FILE_NAME_LEN);
				memset(fs_name_str, 0, FILE_NAME_LEN);
				memset(shd_name_str, 0, FILE_NAME_LEN);
			}
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
		if (pshd_sel->_vs_selected)
		{
			ImGui::TextWrapped(pshd_sel->get_vs_code().c_str());
		}
		if (pshd_sel->_fs_selected)
		{
			ImGui::TextWrapped(pshd_sel->get_fs_code().c_str());
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
		ImGui::EndPopup();
	}
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = "material_list";
	if (IconTreeNode(icon_str, "material list", node_flags_root))
	{
		for (auto& mtl_ut : g_material_list)
		{
			icon_str = "material";
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

