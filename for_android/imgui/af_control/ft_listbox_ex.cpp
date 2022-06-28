#include "ft_listbox_ex.h"
#include "af_bind.h"
namespace zl_future
{

     ft_listbox_ex::ft_listbox_ex()
     {
          _in_p._sizew = 100.f;
          _in_p._sizeh = 200.f;
#if !defined(DISABLE_DEMO)
         reg_property_handle( &_pt, 0, [this]( void* )
          {
               if(ImGui::Checkbox( "vertical", &_pt._vertical ))
               {
                    if( _pt._vertical )
                    {
                         _pt._inner_top = 0;
                    }
                    else
                    {
                         _pt._inner_left = 0;
                    }
               }
               
          } );
#endif
     }

     void ft_listbox_ex::draw_frames()
     {
          ImVec2 apos = absolute_coordinate_of_base_pos();
#if !defined(DISABLE_DEMO)
          if( !is_selected())
          {
               auto offset = ImGui::GetCursorScreenPos();
               auto cur_pos = apos + offset;
               ImGui::SetCursorScreenPos( cur_pos );
               ImGui::InvisibleButton( _in_p._name, ImVec2( _in_p._sizew, _in_p._sizeh ) );
               if( ImGui::IsItemActive() && ImGui::IsMouseDragging() )
               {
                    auto ms_delta = ImGui::GetIO().MouseDelta;
                    if( _pt._vertical )
                    {
                         _pt._inner_top += ms_delta.y;
                    }
                    else
                    {
                         _pt._inner_left += ms_delta.x;

                    }
               }
          }

#endif
          ImVec2 winpos = ImGui::GetWindowPos();
          auto sizew = _in_p._sizew;
          auto sizeh = _in_p._sizeh;
          ImVec2 pos0 = { apos.x + winpos.x, apos.y + winpos.y };
          ImVec2 pos1 = { pos0.x, pos0.y + sizeh };
          ImVec2 pos2 = { pos0.x + sizew, pos0.y + sizeh };
          ImVec2 pos3 = { pos0.x + sizew, pos0.y };
          ImGui::PushClipRect( pos0, pos2, true );
          if( _pt._vertical )
          {
               float spos_y = _pt._inner_top;
               for( auto item : _vchilds )
               {
                    auto bpos = item->base_pos();
                    float w, h;
                    item->get_size( w, h );
                    if( bpos.y != spos_y )
                    {
                         item->set_base_posy( spos_y );
                         prop_ele_position pep { item, 0, 1 };
                         calcu_bind_node( pep );
                    }
                    spos_y += h;
                    spos_y += _pt._interval;
                    item->draw_frames();
               }
               _pt._inner_bottom = spos_y;
          }
          else
          {
               float spos_x = _pt._inner_left;
               for( auto item : _vchilds )
               {
                    auto bpos = item->base_pos();
                    float w, h;
                    item->get_size( w, h );
                    if( bpos.x != spos_x )
                    {
                         item->set_base_posy( spos_x );
                         prop_ele_position pep { item, 0, 0 };
                         calcu_bind_node( pep );
                    }
                    spos_x += w;
                    spos_x += _pt._interval;
                    item->draw_frames();
               }
               _pt._inner_right = spos_x;
          }
          ImGui::PopClipRect();
#if !defined(DISABLE_DEMO)
          if( is_selected() )//draw envelope
          {
               ImU32 col = ImGui::GetColorU32( ImGuiCol_HeaderActive );
               ImVec2 editunit( edit_unit_len, edit_unit_len );
               ImVec2 pos1a = pos0 - editunit;
               ImVec2 pos1b = pos0 + editunit;
               ImGui::RenderFrame( pos1a, pos1b, col );
               ImVec2 pos2a = pos1 - editunit;
               ImVec2 pos2b = pos1 + editunit;
               ImGui::RenderFrame( pos2a, pos2b, col );
               ImVec2 pos3a = pos2 - editunit;
               ImVec2 pos3b = pos2 + editunit;
               ImGui::RenderFrame( pos3a, pos3b, col );
               ImVec2 pos4a = pos3 - editunit;
               ImVec2 pos4b = pos3 + editunit;
               ImGui::RenderFrame( pos4a, pos4b, col );
          }
#endif
     }
}