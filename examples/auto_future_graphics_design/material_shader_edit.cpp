#include "material_shader_edit.h"
#include "af_shader.h"
#include "af_material.h"
#include "user_control_imgui.h"
#include <fstream>
#include "dir_output.h"
#include "res_internal.h"
#include "af_type.h"
#include "af_shader_source_code.h"
#define FILE_NAME_LEN 50
extern string g_cureent_directory;
static char vs_code[TXT_BUFF_SZ];
static char fs_code[TXT_BUFF_SZ];
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
				enum 
				{
					en_none_sel,
					en_vs_sel,
					en_fs_sel,
				};
				int sel_type = en_none_sel;
				IconTreeNode(icon_str, shd_ss->_vs_name.c_str(), node_flags);
				if (ImGui::IsItemClicked())
				{
					sel_type = en_vs_sel;
					shd_ss->_vs_selected = true;
					shd_ss->_fs_selected = false;

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
					shd_ss->_fs_selected = true;
					shd_ss->_vs_selected = false;

					sel_type = en_fs_sel;
				}
				if (sel_type!=en_none_sel&&pshd_sel!=shd_ss)
				{
					if (pshd_sel)
					{
						if (!pshd_sel->_read_only)
						{
							if (pshd_sel->_vs_selected)
							{
								pshd_sel->get_vs_code() = vs_code;
							}
							else
							if (pshd_sel->_fs_selected)
							{
								pshd_sel->get_fs_code() = fs_code;
							}
						}
						pshd_sel->reset_sel();
					}
					pshd_sel = shd_ss;
					if (!pshd_sel->_read_only)
					{
						strcpy(vs_code, shd_ss->get_vs_code().c_str());
						strcpy(fs_code, shd_ss->get_fs_code().c_str());
					}
					
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
		bool be_enabled = strlen(shd_name_str) > 0 && strlen(vs_name_str) > 0 && strlen(fs_name_str) > 0;
		if (!be_enabled)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button("add new shader"))
		{
			string str_shader_file = g_cureent_directory+shaders_fold;
			string vs_file = str_shader_file + vs_name_str;
			string fs_file = str_shader_file + fs_name_str;
			string vs_code, fs_code;
			ifstream if_sd;
			if_sd.open(vs_file);
			if (if_sd.is_open())
			{
				filebuf* pbuf = if_sd.rdbuf();
				size_t sz_code = pbuf->pubseekoff(0, if_sd.end, if_sd.in);
				pbuf->pubseekpos(0, if_sd.in);
				vs_code.reserve(sz_code);
				vs_code.resize(sz_code);
				pbuf->sgetn(&vs_code[0], sz_code);
				if_sd.close();
			}
			else
			{
				vs_code = single_txt_vs;
			}
			if_sd.open(fs_file);
			if (if_sd.is_open())
			{
				filebuf* pbuf = if_sd.rdbuf();
				size_t sz_code = pbuf->pubseekoff(0, if_sd.end, if_sd.in);
				pbuf->pubseekpos(0, if_sd.in);
				fs_code.reserve(sz_code);
				fs_code.resize(sz_code);
				pbuf->sgetn(&fs_code[0], sz_code);
				if_sd.close();
			}
			else
			{
				fs_code = single_txt_fs;
			}
		
COMPAIR_SHADER_END:
			shared_ptr<af_shader> pshd = make_shared<af_shader>(vs_code.c_str(), fs_code.c_str());
			pshd->set_name(shd_name_str);
			g_af_shader_list[shd_name_str] = pshd;
			pshd->_vs_name = vs_name_str;
			pshd->_fs_name = fs_name_str;
			memset(vs_name_str, 0, FILE_NAME_LEN);
			memset(fs_name_str, 0, FILE_NAME_LEN);
			memset(shd_name_str, 0, FILE_NAME_LEN);
			ImGui::CloseCurrentPopup();
		}
		if (!be_enabled)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
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
		if (pshd_sel->_read_only)
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
			if (ImGui::Button("Compile"))
			{
				string str_vs(vs_code);
				pshd_sel->build_vs_code(str_vs);

				string str_fs(fs_code);
				pshd_sel->build_fs_code(str_fs);
				if (pshd_sel->_vs_code_valid&&pshd_sel->_fs_code_valid)
				{
					pshd_sel->link();
					pshd_sel->refresh_viarable_list();
					refresh_material(pshd_sel);
				}

				
			}

			auto edit_ht = ImGui::GetTextLineHeight() * 32;
			auto txt_flag = ImGuiInputTextFlags_AllowTabInput;
			if (pshd_sel->_vs_selected)
			{
				if (!pshd_sel->_vs_code_valid)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.,0.,0.,1.));
				}
				ImGui::InputTextMultiline("vertex shader code:", vs_code, TXT_BUFF_SZ, ImVec2(-1.0f, edit_ht), txt_flag);
				if (!pshd_sel->_vs_code_valid)
				{
					ImGui::PopStyleColor();
				}
			}
			else
			if (pshd_sel->_fs_selected)
			{
				if (!pshd_sel->_fs_code_valid)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1., 0., 0., 1.));
				}
				ImGui::InputTextMultiline("fragment shader code:", fs_code, TXT_BUFF_SZ, ImVec2(-1.0f, edit_ht), txt_flag);
				if (!pshd_sel->_fs_code_valid)
				{
					ImGui::PopStyleColor();
				}
			}
		}

		if (!pshd_sel->is_valid())
		{
			if (pshd_sel->_vs_selected || pshd_sel->_fs_selected)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error info:");
				ImGui::TextWrapped(pshd_sel->compile_error_info.c_str());
			}
		}
		
		
	}
}
void material_shader_edit::draw_material()
{
	if (pshd_sel&&pshd_sel->is_valid()&& ImGui::Button("create material"))
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

