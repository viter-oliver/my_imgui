#include "primitve_edit.h"
#include <string>
#include "user_control_imgui.h"
#include "res_internal.h"
#ifdef IMGUI_WAYLAND
#include "../../deps/glad/glad.h"
#else
#include <GL/gl3w.h> 
#endif

void primitve_edit::draw_primitive_list()
{

	if (ImGui::Button("New primitive object..."))
	{

	}
	ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = icn_primitive;
	if (IconTreeNode(icon_str,"primitive objects",node_flag))
	{
		for (auto& iprm:g_primitive_list)
		{
			auto& key_name = iprm.first;
			auto& prm = iprm.second;
			node_flag = ImGuiTreeNodeFlags_Leaf;
			if (prm->_sel)
			{
				node_flag |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, key_name.c_str(), node_flag);
			if (ImGui::IsItemClicked())
			{
				if (_pmobj)
				{
					_pmobj->_sel = false;
				}
				_pmobj = prm;
				_pmobj->_sel = true;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void primitve_edit::draw_primitive_item_property()
{
	if (_pmobj)
	{
		ImGui::Text("Vertex buffer length:%d ,element buffer length:%d", _pmobj->_vertex_buf_len, _pmobj->_ele_buf_len);
		auto& fmts = _pmobj->_ele_format;
		static char str_numb[0xa] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
		string str_plus = "+##";
		string str_minus = "-##";
		string str_fmt;
		for (auto& fmt_u:fmts)
		{
			str_fmt += str_numb[fmt_u];
			str_fmt += ':';
			//ImGui::Text(str_numb[fmt_u]);
		}
		str_fmt.back() = '\0';
		ImGui::Text("Element format:%s",str_fmt.c_str());
		//if (ImGui::Button("new"))
		//{
		//	fmts.emplace_back();
		//	fmts.back() = 1;
		//}
		//ImGui::SameLine();
		//auto fsz = fmts.size();
		//if (fsz>1&& ImGui::Button("del"))
		//{
		//	fmts.resize(fsz - 1);
		//}
		
	}
}
