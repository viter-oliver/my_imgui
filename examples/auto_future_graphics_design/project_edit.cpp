#include "project_edit.h"
#include "user_control_imgui.h"
#include "res_output.h"
#include <GLFW/glfw3.h>
#include "command_element_delta.h"
#include "af_state_manager.h"
/*
1、如果是末端则flags|leaf，否则flags|openonarrow
2、所有node都是selectable,如果ctrl则保留上次的node的selected状态，如果此次!ctrl则遗弃上次的selected node状态
3、当有多个node处于selected状态，快捷菜单只有copy和addsiblings，执行add sibling时，自末端向父级寻找selected的node，执行复制行为
4、root被选中时不可以执行addsibling
*/
void project_edit::view_object(base_ui_component& fb)
{
     bool beparent = fb.get_child_count() > 0;
     if (fb._be_inner_use)
     {
          if (beparent)
          {
               for( size_t ix = 0; ix < fb.get_child_count(); ix++ )
               {
                    base_ui_component* pchild = fb.get_child( ix );
                    view_object( *pchild );
               }
          }
          return;
     }
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;// | ImGuiTreeNodeFlags_Selected;
	string cname = typeid(fb).name();
	cname = cname.substr(sizeof("class autofuture::"));
	string objname = fb.get_name();
	
	if (!beparent)
	{
		node_flags_root |= ImGuiTreeNodeFlags_Leaf;
	}
	if (fb.is_selected())
	{
		node_flags_root |= ImGuiTreeNodeFlags_Selected;
	}
    char* icon_postfix = NULL;
     if( !fb.is_visible() )
     {
          icon_postfix = "hide";
     }
     auto unflolded = IconTreeNode( cname, objname.c_str(), node_flags_root, (const char*)icon_postfix );
     
	if (unflolded)
	{
          //printf( "tree item unfolded\n" );
		if (ImGui::IsItemClicked())
		{
			if (_pcurrent_object)
			{
				_pcurrent_object->set_selected(false);
			}
			_pcurrent_object = &fb;		    
               fb.set_selected(true);
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
	if (!_pcurrent_object )
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
          if (ImGui::MenuItem("cut",NULL,false))
          {
               _pcut_object = _pcurrent_object;
          }
		if (ImGui::MenuItem("paste",NULL,false))
		{
			if (_pcopy_object)
			{
				auto a_copy = get_copy_of_object(_pcopy_object);
				string chld_name = _pcurrent_object->try_create_a_child_name(_pcopy_object->get_name());
				a_copy->set_name(chld_name);
				_pcurrent_object->add_child(a_copy);
                    _pcopy_object = nullptr;
			}
               if( _pcut_object )
               {
                    auto pparent = _pcut_object->get_parent();
                    pparent->remove_child( _pcut_object, false );
                    _pcurrent_object->add_child( _pcut_object );
                    _pcut_object = nullptr;
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
				
				if (ImGui::MenuItem(cname.c_str(), NULL, false,infun!=nullptr))
				{

					base_ui_component* pchild = infun();
					string chd_name = _pcurrent_object->try_create_a_child_name(cname);
					pchild->set_name(chd_name);
					_pcurrent_object->add_child(pchild);
					pchild->link();
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
				bool find_ref = false;
				string obj_name;
				auto find_ref_in_mstate_manager = [&](base_ui_component* pobj,string& obj_name){
					for (auto& istm : g_mstate_manager)
					{
						auto& stm = *istm.second;
						int ix = 0, isz = stm._prop_list.size();
						for (; ix < isz; ix++)
						{
							auto& prp_pos = stm._prop_list[ix];
							if (prp_pos._pobj == pobj)
							{
								obj_name = istm.first;
								return true;
							}
						}
					}
					return false;
				};
				auto find_ref_in_alias = [&](base_ui_component* pobj, string& obj_name){
					for (auto& ialias:g_aliase_dic)
					{
						auto& ele_pos = *ialias.second;
						if (ele_pos._pobj==pobj)
						{
							obj_name = ialias.first;
							return true;
						}
					}
					return false;
				};
				auto find_ref_in_binds = [&](base_ui_component* pobj, string& obj_name){
					for (auto& ibind : g_bind_dic)
					{
						if (ibind.first._pobj == pobj)
						{
							obj_name = ibind.first._pobj->get_name();
							return true;
						}
					}
					for (auto& ibindrec : g_bind_ref_dic)
					{
						if (ibindrec.first._pobj == pobj)
						{
							obj_name = ibindrec.first._pobj->get_name();
							return true;
						}
					}
					return false;
				};
				find_ref = find_ref_in_mstate_manager(_pcurrent_object, obj_name);
			
				if (find_ref)
				{
					string dlg_content = "you can't delete the node, because the node is referenced by state manager:";
					dlg_content += obj_name;
					MessageBox(GetForegroundWindow(), dlg_content.c_str(), "Warning", MB_OK);
					ImGui::EndPopup();
					return;
				}
				find_ref = find_ref_in_alias(_pcurrent_object, obj_name);
				if (find_ref)
				{
					string dlg_content = "you can't delete the node, because the node is referenced by alias:";
					dlg_content += obj_name;
					MessageBox(GetForegroundWindow(), dlg_content.c_str(), "Warning", MB_OK);
					ImGui::EndPopup();
					return;
				}
				find_ref = find_ref_in_binds(_pcurrent_object, obj_name);
				if (find_ref)
				{
					string dlg_content = "you can't delete the node, because the node is referenced by binds:";
					dlg_content += obj_name;
					MessageBox(GetForegroundWindow(), dlg_content.c_str(), "Warning", MB_OK);
					ImGui::EndPopup();
					return;
				}
				
				
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
