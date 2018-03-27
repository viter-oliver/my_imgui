#include "project_edit.h"
#include "user_control_imgui.h"

/*
1、如果是末端则flags|leaf，否则flags|openonarrow
2、所有node都是selectable,如果ctrl则保留上次的node的selected状态，如果此次!ctrl则遗弃上次的selected node状态
3、当有多个node处于selected状态，快捷菜单只有copy和addsiblings，执行add sibling时，自末端向父级寻找selected的node，执行复制行为
4、root被选中时不可以执行addsibling
*/
base_ui_component* psel = NULL;
base_ui_component* project_edit(base_ui_component& fb)
{
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;// | ImGuiTreeNodeFlags_Selected;
	string cname = typeid(fb).name();
	cname = cname.substr(sizeof("class"));
	string& objname = fb.get_name();
	bool beparent = fb.get_child_count() > 0;
	if (!beparent)
	{
		node_flags_root |= ImGuiTreeNodeFlags_Leaf;
	}
	if (fb.is_selected())
	{
		node_flags_root |= ImGuiTreeNodeFlags_Selected;
	}
	if (IconTreeNode(cname,objname.c_str(), node_flags_root))
	{
		if (ImGui::IsItemClicked())
		{
		    fb.set_selected(true);
			if (psel)
			{
				psel->set_selected(false);
			}
			psel = &fb;
		}
		if (beparent)
		{
			for (size_t ix = 0; ix < fb.get_child_count(); ix++)
			{
				base_ui_component* pchild = fb.get_child(ix);
				psel=project_edit(*pchild);
			}
		}
		
		ImGui::TreePop();

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
