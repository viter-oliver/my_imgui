#include "project_edit.h"
#include "user_control_imgui.h"
#include "res_output.h"
#include <GLFW/glfw3.h>
#include "command_element_delta.h"
#include "af_state_manager.h"
#include <map>
#include "common_functions.h"
#include "aliase_edit.h"
#include "base_prp_type_edit.h"
#include "bind_edit.h"
extern aliase_edit g_aliase_edit;
extern base_prp_type_edit g_common_value_edit;
extern bind_edit g_bind_edit;
/*
1、如果是末端则flags|leaf，否则flags|openonarrow
2、所有node都是selectable,如果ctrl则保留上次的node的selected状态，如果此次!ctrl则遗弃上次的selected node状态
3、当有多个node处于selected状态，快捷菜单只有copy和addsiblings，执行add sibling时，自末端向父级寻找selected的node，执行复制行为
4、root被选中时不可以执行addsibling
*/

void project_edit::view_object(base_ui_component& fb)
{
     bool beparent = fb.get_child_count() > 0;

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
     if( _trigger_focus_switch&&fb.is_selected() && !ImGui::IsItemVisible() )
     {
          ImGui::SetScrollHere();
          _trigger_focus_switch = false;
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
     _trigger_focus_switch = false;
}
void project_edit::move_item_pre()
{
     base_ui_component* pparent = _pcurrent_object->get_parent();
     if( pparent )
     {
          pparent->move_pre( _pcurrent_object );
     }
}
void project_edit::move_item_next()
{
     base_ui_component* pparent = _pcurrent_object->get_parent();
     if( pparent )
     {
          pparent->move_next( _pcurrent_object );
     }
}
void project_edit::copy_item()
{
     _pcopy_object = _pcurrent_object;
}
void project_edit::cut_item()
{
     _pcut_object = _pcurrent_object;
}
void project_edit::past_item()
{
     if( _pcopy_object )
     {
          auto a_copy = get_copy_of_object( _pcopy_object );
          string chld_name = _pcurrent_object->try_create_a_child_name( _pcopy_object->get_name() );
          a_copy->set_name( chld_name );
          _pcurrent_object->add_child( a_copy );
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
bool allow_add_item( string& parent_type_name, string& child_type_name )
{
     static multimap<string, string> dic_controls_child_control = {
          { "ft_scene", "ft_material_2d" },
          { "ft_scene", "ft_material_3d" },
          { "ft_scene", "ft_modeling_3d" },
          { "ft_scene", "ft_particles_3d" },
          { "ft_scene", "ft_particles_effect_3d" },
          { "ft_scene", "ft_particles1_3d" },
          { "ft_hud_projector", "ft_hud_obj" },
          { "ft_hud_projector", "ft_hud_4_time_curve" },
          { "ft_modeling_3d", "ft_trans" },
          { "ft_material_3d", "ft_trans" },
          { "ft_listbox_ex", "ft_block" },
          { "ft_listbox_ex", "ft_image" },
          { "ft_listbox_ex", "ft_image_file" },
          { "ft_listbox_ex", "ft_image_play" },
          { "ft_listbox_ex", "ft_button" },
          { "ft_listbox_ex", "ft_secne" },
     };
     if( parent_type_name == "ft_trans" 
         || parent_type_name == "ft_hud_obj" 
         || parent_type_name == "ft_hud_4_time_curve" )
     {
          return false;
     }
     auto irg = dic_controls_child_control.equal_range( parent_type_name );
     bool find_ptype = irg.first != irg.second;
     for( auto m = irg.first; m != irg.second;++m )
     {
          if (m->second==child_type_name)
          {
               return true;
          }
     }
     if (!find_ptype)
     {
          if( child_type_name =="ft_trans")
          {
               return false;
          }
          string ext_name = child_type_name.substr( child_type_name.size() - 2, 2 );
          if (ext_name=="3d"||ext_name=="2d")
          {
               return false;
          }
          return true;
     }
     return false;

}
void project_edit::add_item()
{
	string cur_cname = typeid(*_pcurrent_object).name();
	cur_cname = cur_cname.substr(sizeof("class autofuture::"));

     factory::get().iterate_types( [&]( string cname, function<base_ui_component*( )> infun )
     {
          if( !allow_add_item(cur_cname,cname))
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
void project_edit::insert_item()
{
     string cur_cname = typeid( *_pcurrent_object ).name();
     cur_cname = cur_cname.substr( sizeof( "class autofuture::" ) );
     bool is_ft_scene = cur_cname == "ft_scene";
     bool is_ft_listbox_ex = cur_cname == "ft_listbox_ex";
     bool is_ft_modeling = cur_cname == "ft_modeling_3d";
     bool is_trans = cur_cname == "ft_trans";
     bool is_material_3d = cur_cname == "ft_material_3d";
     factory::get().iterate_types( [&]( string cname, function<base_ui_component*( )> infun )
     {
          if( !allow_add_item( cur_cname, cname ) )
          {
               return;
          }

          if( ImGui::MenuItem( cname.c_str(), NULL, false, infun != nullptr ) )
          {

               base_ui_component* pchild = infun();
               auto pparent = _pcurrent_object->get_parent();
               string chd_name = pparent->try_create_a_child_name( cname );
               pchild->set_name( chd_name );
               pparent->insert_child( _pcurrent_object, pchild );
               pchild->link();
          }

     } );
     ImGui::EndMenu();
}
void project_edit::add_sibling()
{
     if( auto pparent = _pcurrent_object->get_parent() )
     {
          base_ui_component* psibling = get_copy_of_object( _pcurrent_object );
          string chd_name = pparent->try_create_a_child_name( _pcurrent_object->get_name() );
          psibling->set_name( chd_name );
          pparent->add_child( psibling );
     }
}
void project_edit::delete_item()
{
     base_ui_component* pparent = _pcurrent_object->get_parent();
     if( pparent )
     {
          /**
          bool find_ref = false;
          string obj_name;
          auto find_ref_in_mstate_manager = [&]( base_ui_component* pobj, string& obj_name )
          {
               for( auto& istm : g_mstate_manager )
               {
                    auto& stm = *istm.second;
                    int ix = 0, isz = stm._prop_list.size();
                    for( ; ix < isz; ix++ )
                    {
                         auto& prp_pos = stm._prop_list[ ix ];
                         if( prp_pos._pobj == pobj )
                         {
                              obj_name = istm.first;
                              return true;
                         }
                    }
               }
               return false;
          };
          auto find_ref_in_alias = [&]( base_ui_component* pobj, string& obj_name )
          {
               for( auto& ialias : g_aliase_dic )
               {
                    auto& ele_pos = *ialias.second;
                    if( ele_pos._pobj == pobj )
                    {
                         obj_name = ialias.first;
                         return true;
                    }
               }
               return false;
          };
          auto find_ref_in_binds = [&]( base_ui_component* pobj, string& obj_name )
          {
               for( auto& ibind : g_bind_dic )
               {
                    if( ibind.first._pobj == pobj )
                    {
                         obj_name = ibind.first._pobj->get_name();
                         return true;
                    }
               }
               for( auto& ibindrec : g_bind_ref_dic )
               {
                    if( ibindrec.first._pobj == pobj )
                    {
                         obj_name = ibindrec.first._pobj->get_name();
                         return true;
                    }
               }
               return false;
          };
          find_ref = find_ref_in_mstate_manager( _pcurrent_object, obj_name );

          if( find_ref )
          {
               string dlg_content = "you can't delete the node, because the node is referenced by state manager:";
               dlg_content += obj_name;
               MessageBox( GetForegroundWindow(), dlg_content.c_str(), "Warning", MB_OK );
               ImGui::EndPopup();
               return;
          }
          find_ref = find_ref_in_alias( _pcurrent_object, obj_name );
          if( find_ref )
          {
               string dlg_content = "you can't delete the node, because the node is referenced by alias:";
               dlg_content += obj_name;
               MessageBox( GetForegroundWindow(), dlg_content.c_str(), "Warning", MB_OK );
               ImGui::EndPopup();
               return;
          }
          find_ref = find_ref_in_binds( _pcurrent_object, obj_name );
          if( find_ref )
          {
               string dlg_content = "you can't delete the node, because the node is referenced by binds:";
               dlg_content += obj_name;
               MessageBox( GetForegroundWindow(), dlg_content.c_str(), "Warning", MB_OK );
               ImGui::EndPopup();
               return;
          }
          */
          g_aliase_edit.clear_sel();
          g_common_value_edit.clear_sel();
          g_bind_edit.clear_sel();
          g_ui_edit_command_mg.clear_cmds_by_component( _pcurrent_object );

          pparent->remove_child( _pcurrent_object );
          _pcurrent_object = pparent;

     }

}
void project_edit::popup_context_menu()
{
	if (!_pcurrent_object )
	{
		return;
	}
	if (ImGui::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)&&ImGui::IsKeyReleased(GLFW_KEY_UP))
	{
          move_item_pre();
	}
     if( ImGui::IsKeyPressed( GLFW_KEY_LEFT_CONTROL ) && ImGui::IsKeyReleased( GLFW_KEY_DOWN ) )
	{
          move_item_next();
	}
     if( ImGui::IsKeyPressed( GLFW_KEY_LEFT_CONTROL ) && ImGui::IsKeyReleased( GLFW_KEY_DELETE ) )
     {
          delete_item();
     }    
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("copy", "SHIFT+C", false))
		{
               copy_item();
		}
          if (ImGui::MenuItem("cut","SHIFT+X",false))
          {
               cut_item();
          }
		if (ImGui::MenuItem("paste","SHIFT+V",false))
		{
               past_item();
		}
		if (ImGui::BeginMenu("add child"))
		{
               add_item();
		}
          if( ImGui::BeginMenu( "insert child" ) )
          {
               insert_item();
          }		
          if( ImGui::MenuItem( "add sibling", "SHIFT+A", false ) )
		{
               add_sibling();
			//ImGui::EndMenu();
		}
		if (ImGui::MenuItem("delete","Del"))
		{
               delete_item();
		}
          //char str_up[] = {  0x21,0x91, 0 };
          string str_up = wstringToUtf8( wstring( L"↑" ) );
          if( ImGui::MenuItem( "up", str_up.c_str()) )
		{
               move_item_pre();
		}
          //char str_dn[] = { 0x21, 0x93, 0 };
          string str_dn = wstringToUtf8( wstring( L"↓" ) );
          if( ImGui::MenuItem( "down", str_dn.c_str() ) )
		{
               move_item_next();
		}

		ImGui::EndPopup();
	}
}

void project_edit::clear_sel_item()
{
     if( _pcurrent_object )
     {
          _pcurrent_object->set_selected( false );
     }
     _pcurrent_object = nullptr;
     _pcopy_object = nullptr;
}
