#include "project_edit.h"
#include "user_control_imgui.h"
#include "res_output.h"
#include <GLFW/glfw3.h>
#include "command_element_delta.h"
/*
1、如果是末端则flags|leaf，否则flags|openonarrow
2、所有node都是selectable,如果ctrl则保留上次的node的selected状态，如果此次!ctrl则遗弃上次的selected node状态
3、当有多个node处于selected状态，快捷菜单只有copy和addsiblings，执行add sibling时，自末端向父级寻找selected的node，执行复制行为
4、root被选中时不可以执行addsibling
*/
void project_edit::view_object(base_ui_component& fb)
{
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;// | ImGuiTreeNodeFlags_Selected;
	string cname = typeid(fb).name();
	cname = cname.substr(sizeof("class autofuture::"));
	string objname = fb.get_name();
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
			if (_pcurrent_object)
			{
				_pcurrent_object->set_selected(false);
			}
			_pcurrent_object = &fb;
		}
		if (beparent)
		{
			for (size_t ix = 0; ix < fb.get_child_count(); ix++)
			{
				base_ui_component* pchild = fb.get_child(ix);
				view_object(*pchild);
			}
		}
		ImGui::TreePop();
	}
	//if (ImGui::IsItemActive())
	//{
	//	// Draw a line between the button and the mouse cursor
	//	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	//	draw_list->PushClipRectFullScreen();
	//	ImGuiIO& io = ImGui::GetIO();
	//	draw_list->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
	//	draw_list->PopClipRect();
	//	//ImGui::Button("Drag Me");
	//}
}

void project_edit::objects_view()
{
	view_object(_root);
}

void project_edit::popup_context_menu()
{
	if (!_pcurrent_object || g_vres_texture_list.size()==0)
	{
		return;
	}
	if (ImGui::IsKeyReleased(GLFW_KEY_UP))
	{
		base_ui_component* pparent = _pcurrent_object->get_parent();
		if (pparent)
		{
			pparent->move_pre(_pcurrent_object);
		}
	}
	if (ImGui::IsKeyReleased(GLFW_KEY_DOWN))
	{
		base_ui_component* pparent = _pcurrent_object->get_parent();
		if (pparent)
		{
			pparent->move_next(_pcurrent_object);
		}
	}
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("copy", NULL, false))
		{
			_pcopy_object = _pcurrent_object;
		}
		if (ImGui::MenuItem("paste",NULL,false))
		{
			if (_pcopy_object)
			{
				auto a_copy = get_copy_of_object(_pcopy_object);
				string chld_name = _pcurrent_object->try_create_a_child_name(_pcopy_object->get_name());
				a_copy->set_name(chld_name);
				_pcurrent_object->add_child(a_copy);
			}
		}
		if (ImGui::BeginMenu("add child"))
		{
			string cur_cname = typeid(*_pcurrent_object).name();
			cur_cname = cur_cname.substr(sizeof("class autofuture::"));
			bool is_ft_scene = cur_cname == "ft_scene";
			factory::get().iterate_types([this, is_ft_scene](string cname, function<base_ui_component*()> infun){
				string ext_name = cname.substr(cname.size() - 2, 2);
				if (is_ft_scene&&ext_name != "3d"&&ext_name != "2d" || !is_ft_scene && (ext_name == "3d" || ext_name == "2d"))
				{
					return;
				}
				
				if (ImGui::MenuItem(cname.c_str(), NULL, false))
				{

					base_ui_component* pchild = infun();
					string chd_name = _pcurrent_object->try_create_a_child_name(cname);
					pchild->set_name(chd_name);
					_pcurrent_object->add_child(pchild);
				}
				
			});
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("add sibling", NULL, false))
		{
			if (auto pparent = _pcurrent_object->get_parent())
			{
				base_ui_component* psibling = get_copy_of_object(_pcurrent_object);
				string chd_name = pparent->try_create_a_child_name(_pcurrent_object->get_name());
				psibling->set_name(chd_name);
				pparent->add_child(psibling);
			}
			//ImGui::EndMenu();
		}
		if (ImGui::MenuItem("delete",NULL,false))
		{
			base_ui_component* pparent = _pcurrent_object->get_parent();
			if (pparent)
			{
				g_ui_edit_command_mg.clear_cmds_by_component(_pcurrent_object);
				pparent->remove_child(_pcurrent_object);
				_pcurrent_object = pparent;
			}
		}
		if (ImGui::MenuItem("up",NULL,false))
		{
			base_ui_component* pparent = _pcurrent_object->get_parent();
			if (pparent)
			{
				pparent->move_pre(_pcurrent_object);
			}
		}
		if (ImGui::MenuItem("down", NULL, false))
		{
			base_ui_component* pparent = _pcurrent_object->get_parent();
			if (pparent)
			{
				pparent->move_next(_pcurrent_object);
			}
		}

		ImGui::EndPopup();
	}
}
