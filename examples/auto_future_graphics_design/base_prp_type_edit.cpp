#include "ft_base.h"
#include "user_control_imgui.h"
#include "base_prp_type_edit.h"
#include "res_internal.h"
#include "common_functions.h"
#include "res_output.h"
#include <sstream>
static char sal_name_str[ FILE_NAME_LEN ] = "";
static char* new_common_value = "New common value";
static char* vtyps[] = { "float", "double", "int", "bool", "af_vi2", "af_vi3", "af_vi4", "af_vec2", "af_vec3", "af_vec4" };
static int tpidx = 0;


void base_prp_type_edit::set_dragging( bool be_dragging, base_ui_component* pobj, uint16_t page_idx, uint16_t off_idx )
{
     if( !be_dragging&&_hit_sm_window )
     {
          auto& fd_ele = pobj->get_filed_ele( _pgidx, _fdidx );
          if( fd_ele._type == _sl_bs_prp->_type)
          {
               auto& pplist = _sl_bs_prp->_param_list;
               prop_ele_position new_prp_pos = { pobj, _pgidx, _fdidx };
               auto ifd = find( pplist.begin(), pplist.end(), new_prp_pos );
               if( ifd == pplist.end() )
               {

                    pplist.emplace_back();
                    auto pm_idx = pplist.size() - 1;
                    auto& prop_ele_p = pplist[ pm_idx ];
                    prop_ele_p._pobj = pobj;
                    prop_ele_p._page_index = _pgidx;
                    prop_ele_p._field_index = _fdidx;
               }
          }
          _hit_sm_window = false;
     }
     if( be_dragging )
     {
          _pgidx = page_idx;
          _fdidx = off_idx;
     }
     _be_draging = be_dragging;
}
void base_prp_type_edit::view_base_prp_list()
{
     if (ImGui::Button("Create new common value..."))
     {
          ImGui::OpenPopup( "Create new common value" );
          memset( sal_name_str, 0, FILE_NAME_LEN );
          tpidx = 0;
     }
     if (ImGui::BeginPopupModal("Create new common value"))
     {
          ImGui::InputText( "The name of new common value", sal_name_str, FILE_NAME_LEN );
          ImGui::Combo( "The type of common value", &tpidx, vtyps, IM_ARRAYSIZE( vtyps ) );
          if (ImGui::Button("Save"))
          {
               auto icv = make_shared<base_prp_type>( vtyps[ tpidx ] );
               string prekn = sal_name_str;
               string key_cv = find_a_key_from_mp( g_base_prp_dic, prekn );
               g_base_prp_dic[ key_cv ] = icv;
               _key_name = key_cv;
               _sl_bs_prp = icv;

               ImGui::CloseCurrentPopup();
          }
          if (ImGui::Button("Cancel"))
          {
               ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
     }
     ImGui::BeginChild( "common_value_list", ImVec2( 0, 0 ), true );
     string icon_str = icn_shared_value;
     ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
     if( IconTreeNode( icon_str, "Common value list", node_flags_root ) )
     {
          for( auto& icv : g_base_prp_dic )
          {
               auto& keyname = icv.first;
               auto& ps_prp_pos = icv.second;
               ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
               if( _sl_bs_prp&&ps_prp_pos == _sl_bs_prp )
               {
                    node_flags |= ImGuiTreeNodeFlags_Selected;
               }
               IconTreeNode( icon_str, keyname.c_str(), node_flags );
               if( ImGui::IsItemClicked() )
               {
                    _key_name = keyname;
                    _sl_bs_prp = ps_prp_pos;
               }
               ImGui::TreePop();
          }
          ImGui::TreePop();
     }
     if( _sl_bs_prp&&ImGui::BeginPopupContextWindow() )
     {
          if( ImGui::MenuItem( "delete", NULL, false ) )
          {
               auto icv = g_base_prp_dic.find( _key_name );
               if( icv != g_base_prp_dic.end() )
               {
                    g_base_prp_dic.erase( icv );
                    _sl_bs_prp = nullptr;
                    _key_name = "";
               }
          }
          
          ImGui::EndPopup();
     }
     ImGui::EndChild();
}
static const float _minf = -base_ui_component::screenw;
static const float _maxf = base_ui_component::screenw;
static const double _mind = -base_ui_component::screenw;
static const double _maxd = base_ui_component::screenw;
static const int _mini = -100;
static const int _maxi = 100;

void base_prp_type_edit::base_prp_item()
{
     if (!_sl_bs_prp)
     {
          return;
     }
     _hit_sm_window = _be_draging&&ImGui::IsMouseHoveringWindow();
     ImGui::Spacing();
     ImGui::Spacing();
     ImGui::Spacing();
     ImGui::Spacing();
     ImGui::Spacing();
     ImGui::Spacing();
     ImGui::Spacing();
     auto& pplist = _sl_bs_prp->_param_list;
     int idx = 0;
     for( auto itp = pplist.begin(); itp != pplist.end(); idx++ )
     {
          auto& prop_ele_p = *itp;
          string prop_ele_show;
          get_uic_path( prop_ele_p._pobj, prop_ele_show );
          auto& pgidx = prop_ele_p._page_index;
          auto& fidx = prop_ele_p._field_index;
          field_ele &fel = prop_ele_p._pobj->get_filed_ele( pgidx, fidx );
          prop_ele_show += "::";
          prop_ele_show += fel._name;
          ImGui::Text( prop_ele_show.c_str() );
          ImGui::SameLine();
          stringstream stream_delbtn;
          stream_delbtn << "X##" << idx;
          string str_delbtn = stream_delbtn.str();
          if( ImGui::Button( str_delbtn.c_str() ) )
          {
               itp = pplist.erase( itp );
          }
          else
          {
               itp++;
          }
     }

     auto& _type = _sl_bs_prp->_type;
     auto& _pbase = _sl_bs_prp->_pbase;
     string rg = _key_name.substr( _key_name.length() - 3, 3 );
     if( _type == "int" )
     {
          if (rg=="txt")
          {
               auto& res_coors = g_vres_texture_list[ g_cur_texture_id_index ].vtexture_coordinates;
               int isize = g_vres_texture_list[ g_cur_texture_id_index ].vtexture_coordinates.size();
               bool be_changed = ImGui::Combo( "value", (int*)_pbase, &get_texture_item, &g_vres_texture_list[ g_cur_texture_id_index ], isize );

               ImGui::SameLine(); ShowHelpMarker( "select a image from image resource!\n" );
               int txt_idx = *(int*)_pbase;
               float reswidth = res_coors[ txt_idx ].owidth();
               float resheight = res_coors[ txt_idx ].oheight();
               ImGui::Text( "original size:%f,%f", reswidth, resheight );
               ImGui::Spacing();
               if( reswidth > 0 )
               {
                    float draw_height = imge_edit_view_width*resheight / reswidth;
                    ImVec2 draw_size( imge_edit_view_width, draw_height );
                    int texture_id = g_vres_texture_list[ g_cur_texture_id_index ].texture_id();
                    float wtexture_width = g_vres_texture_list[ g_cur_texture_id_index ].texture_width;
                    float wtexture_height = g_vres_texture_list[ g_cur_texture_id_index ].texture_height;

                    ImVec2 uv0( res_coors[ txt_idx ]._x0 / wtexture_width, res_coors[ txt_idx ]._y0 / wtexture_height );
                    ImVec2 uv1( res_coors[ txt_idx ]._x1 / wtexture_width, res_coors[ txt_idx ]._y1 / wtexture_height );
                    ImGui::Image( (ImTextureID)texture_id, draw_size, uv0, uv1, ImColor( 255, 255, 255, 255 ), ImColor( 255, 255, 255, 128 ) );
               }
          }
          else
               ImGui::SliderInt( "value", (int*)_pbase, _mini, _maxi );
     }
     else if( _type == "float" || _type == "double" )
     {
          ImGui::SliderFloat( "value", (float*)_pbase, _minf, _maxf );
     }
     else if( _type == "bool" )
     {
          ImGui::Checkbox( "value", (bool*)_pbase );
     }
     else if( _type == "af_vi2" )
     {
          if( rg == "txt" )// atexture
          {
               af_vi2* ptxt_idx = (af_vi2*)_pbase;
               int igsize = g_vres_texture_list.size();
               ImGui::Combo( "Texture group list", &ptxt_idx->x, get_texture_group_name, &g_vres_texture_list, igsize );
               int img_gp_id = ptxt_idx->x;
               auto& res_coors = g_vres_texture_list[ img_gp_id ].vtexture_coordinates;
               int isize = g_vres_texture_list[ img_gp_id ].vtexture_coordinates.size();
               int txt_idx = ptxt_idx->y;
               bool be_changed = ImGui::Combo( "Texture list", &ptxt_idx->y, &get_texture_item, &img_gp_id, isize );

               ImGui::SameLine(); ShowHelpMarker( "select a image from image resource!\n" );
               float reswidth = res_coors[ txt_idx ].owidth();
               float resheight = res_coors[ txt_idx ].oheight();
               ImGui::Text( "original size:%f,%f", reswidth, resheight );
               ImGui::Spacing();
               if( reswidth > 0 )
               {
                    float draw_height = imge_edit_view_width*resheight / reswidth;
                    ImVec2 draw_size( imge_edit_view_width, draw_height );
                    int texture_id = g_vres_texture_list[ img_gp_id ].texture_id();
                    float wtexture_width = g_vres_texture_list[ img_gp_id ].texture_width;
                    float wtexture_height = g_vres_texture_list[ img_gp_id ].texture_height;

                    ImVec2 uv0( res_coors[ txt_idx ]._x0 / wtexture_width, res_coors[ txt_idx ]._y0 / wtexture_height );
                    ImVec2 uv1( res_coors[ txt_idx ]._x1 / wtexture_width, res_coors[ txt_idx ]._y1 / wtexture_height );
                    ImGui::Image( (ImTextureID)texture_id, draw_size, uv0, uv1, ImColor( 255, 255, 255, 255 ), ImColor( 255, 255, 255, 128 ) );
               }
          }
          else
          {
               ImGui::SliderInt2( "value", (int*)_pbase, _mini, _maxi );
          }
     }
     else if( _type == "af_vi3" )
     {
          ImGui::SliderInt3( "value", (int*)_pbase, _mini, _maxi );
     }
     else if( _type == "af_vi4" )
     {
          ImGui::SliderInt4( "value", (int*)_pbase, _mini, _maxi );
     }
     else if( _type == "af_vec2" )
     {
          ImGui::SliderFloat2( "value", (float*)_pbase, _minf, _maxf );
     }
     else if( _type == "af_vec3" )
     {
          if( rg == "clr" )
          {
               ImGui::ColorEdit3( "value", (float*)_pbase, ImGuiColorEditFlags_RGB );
          }
          else
          {
               ImGui::SliderFloat3( "value", (float*)_pbase, _minf, _maxf );
          }
     }
     else if( _type == "af_vec4" )
     {
          if( rg == "clr" )
          {
               ImGui::ColorEdit4( "value", (float*)_pbase, ImGuiColorEditFlags_RGB );
          }
          else
          {
               ImGui::SliderFloat4( "value", (float*)_pbase, _minf, _maxf );
          }
     }
     if (ImGui::Button("Cover the value to properties.."))
     {
          _sl_bs_prp->override_param_list();
     }
}
