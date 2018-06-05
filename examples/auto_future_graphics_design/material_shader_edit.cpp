#include "material_shader_edit.h"
#include "af_shader.h"
#include "material.h"
#include "user_control_imgui.h"
#include <fstream>
#define FILE_NAME_LEN 50
extern string g_cureent_project_file_path;
const char* str_shaders_fold = "shaders\\";

struct shader_source
{
	string _name;
	string _vs_name, _fs_name;
	string _vs, _fs;
	bool _vs_selected{false};
	bool _fs_selected{false};
	shader_source(){}
	~shader_source(){}
	void reset_sel(){ _vs_selected = _fs_selected = false; }
};
typedef vector<shader_source> vsd_source;
vsd_source g_shader_source_list;
shader_source* pshd_source_sel = NULL;
shared_ptr<material> pmateral = nullptr;

void material_shader_edit::draw_shader()
{
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = "shader_list";
	if (IconTreeNode(icon_str, "shader list", node_flags_root))
	{
		for (auto& shd_ss : g_shader_source_list)
		{
            icon_str = "shader_source";
			if (IconTreeNode(icon_str, shd_ss._name.c_str(), node_flags_root))
			{
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
				icon_str = "vertex_shader_source";
				if (shd_ss._vs_selected)
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}
				IconTreeNode(icon_str, shd_ss._vs_name.c_str(), node_flags);
				if (ImGui::IsItemClicked())
				{
					if (pshd_source_sel)
					{
						pshd_source_sel->reset_sel();
					}
					shd_ss._vs_selected = true;
					pshd_source_sel = &shd_ss;
				}
				ImGui::TreePop();
				node_flags = ImGuiTreeNodeFlags_Leaf;
				icon_str = "fragment_shader_source";
				if (shd_ss._fs_selected)
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}
				IconTreeNode(icon_str, shd_ss._fs_name.c_str(), node_flags);
				if (ImGui::IsItemClicked())
				{
					if (pshd_source_sel)
					{
						pshd_source_sel->reset_sel();
					}
					shd_ss._fs_selected = true;
					pshd_source_sel = &shd_ss;
				}
				ImGui::TreePop();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

}
string compile_info;
void material_shader_edit::load_shader()
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
			g_af_shader_list.push_back(pshd);
			g_shader_source_list.emplace_back(shader_source());
			auto& nw_sd_ss = g_shader_source_list.at(g_shader_source_list.size() - 1);
			nw_sd_ss._vs_name = vs_name_str;
			nw_sd_ss._fs_name = fs_name_str;
			nw_sd_ss._name = shd_name_str;
			nw_sd_ss._vs = vs_file;
			nw_sd_ss._fs = fs_file;
			memset(vs_name_str, 0, FILE_NAME_LEN);
			memset(fs_name_str, 0, FILE_NAME_LEN);
			memset(shd_name_str, 0, FILE_NAME_LEN);
		}
	}
}
void material_shader_edit::load_shader_info()
{
	ImGui::TextWrapped(compile_info.c_str());
}
void material_shader_edit::draw_shader_item_property()
{
	if (pshd_source_sel)
	{
		if (pshd_source_sel->_vs_selected)
		{
			ImGui::TextWrapped(pshd_source_sel->_vs.c_str());
		}
		if (pshd_source_sel->_fs_selected)
		{
			ImGui::TextWrapped(pshd_source_sel->_fs.c_str());
		}
	}
}
void material_shader_edit::draw_material()
{
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = "material_list";
	if (IconTreeNode(icon_str, "material list", node_flags_root))
	{
		for (auto& mtl : g_material_list)
		{
			icon_str = "material";
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (mtl->_sel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, mtl->get_name().c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				if (pmateral)
				{
					pmateral->_sel = false;
				}
				mtl->_sel = true;
				pmateral = mtl;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void material_shader_edit::draw_material_item_property()
{
	if (pmateral)
	{
		pmateral->edit_ufs();
	}
}

