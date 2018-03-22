#include "project_edit.h"
#include "imgui.h"
/*
1、如果是末端则flags|leaf，否则flags|openonarrow
2、所有node都是selectable,如果ctrl则保留上次的node的selected状态，如果此次!ctrl则遗弃上次的selected node状态
3、当有多个node处于selected状态，快捷菜单只有copy和addsiblings，执行add sibling时，自末端向父级寻找selected的node，执行复制行为
4、root被选中时不可以执行addsibling
*/
base_ui_component* psel = NULL;
base_ui_component* project_edit(base_ui_component& fb)
{
	//ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;// | ImGuiTreeNodeFlags_Selected;
	/*
	if (ImGui::TreeNodeEx("root", node_flags_root))
	{
		static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
		int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
		//ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
		for (int i = 0; i < 6; i++)
		{
			// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
			if (ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Node %d", i))
			{
				ImGui::TreePop();
			}
			//ImGui::SameLine();
			//ImGui::Text("test %d",i);
			if (ImGui::IsItemClicked())
				node_clicked = i;
		}
		if (node_clicked != -1)
		{
			// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
			if (ImGui::GetIO().KeyCtrl)
				selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
			else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
				selection_mask = (1 << node_clicked);           // Click to single-select
		}
		if (MyTreeNode("test"))
		{
			ImGui::TreePop();

		}
		ImGui::TreePop();
	}*/
	
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;// | ImGuiTreeNodeFlags_Selected;
	string cname = typeid(fb).name();
	cname = cname.substr(sizeof("nameclass "));
	string& oname = fb.get_name();
	bool beparent = fb.get_child_count() > 0;
	if (!beparent)
	{
		node_flags_root |= ImGuiTreeNodeFlags_Leaf;
	}
	if (fb.is_selected())
	{
		node_flags_root |= ImGuiTreeNodeFlags_Selected;
	}
	if (ImGui::TreeNodeEx(oname.c_str(), node_flags_root))
	{
		if (beparent)
		{
			for (size_t ix = 0; ix < fb.get_child_count(); ix++)
			{
				base_ui_component* pchild = fb.get_child(ix);
				psel=project_edit(*pchild);
			}
		}
		ImGui::TreePop();
		if (ImGui::IsItemClicked())
		{
		    fb.set_selected(true);
			if (psel)
			{
				psel->set_selected(false);
			}
			psel = &fb;
		}

	}
	
	if (fb.get_parent()==NULL)
	{
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("copy", NULL, false))
			{
			}

			if (ImGui::BeginMenu("add child"))
			{
				if (ImGui::MenuItem("base", NULL, false))
				{
				}
				if (ImGui::MenuItem("image", NULL, false))
				{
				}
				if (ImGui::MenuItem("button", NULL, false))
				{
				}
				if (ImGui::MenuItem("listbox", NULL, false))
				{
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("add sibling", NULL, false))
			{
			}

			ImGui::EndPopup();
		}
	}
	return psel;
}
