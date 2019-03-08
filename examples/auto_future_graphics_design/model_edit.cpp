#include "model_edit.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
model_edit::model_edit()
{
	
}


model_edit::~model_edit()
{
}

void model_edit::draw_model_list()
{

	bool model_list_open = ImGui::TreeNode("model", "count:%d", g_mmodel_list.size());
	if (model_list_open)
	{
		int ix = 0;
		for (auto& mdid:g_mmodel_list)
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (_pmodel==mdid.second)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			ImGui::TreeNodeEx((void*)(intptr_t)ix, node_flags, mdid.first.c_str());
			if (ImGui::IsItemClicked())
			{
				_pmodel = mdid.second;
			}
			ImGui::TreePop();
			ix++;
		}
		ImGui::TreePop();
	}
}

void model_edit::draw_model_item_property()
{
	if (_pmodel)
	{
		auto& mld = *_pmodel;
		ImGui::Text("mesh count:%d", mld.size());
	}
}
