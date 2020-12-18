#include "ft_listbox_ex.h"
namespace auto_future
{

     ft_listbox_ex::ft_listbox_ex()
     {
          _left_top = _right_bottom= { 0.f, 0.f };
          _in_p._sizew = 100.f;
          _in_p._sizeh = 200.f;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
         reg_property_handle( &_pt, 0, [this]( void* )
          {
               ImGui::Checkbox( "vertical", &_pt._vertical );
               if( _pt._vertical )
               {
                    _left_top.x = 0;
               }
               else
               {
                    _left_top.y = 0;
               }
          } );
#endif
     }

     void ft_listbox_ex::draw_frames()
     {
          ImVec2 apos = absolute_coordinate_of_base_pos();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
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
                         _left_top.y += ms_delta.y;
                    }
                    else
                    {
                         _left_top.x += ms_delta.x;
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
          af_vec2 spos = _left_top;
          if( _pt._vertical )
          {
               for( auto item : _vchilds )
               {
                    if( item->is_visible() )
                    {
                         item->set_base_pos( spos.x,spos.y);
                         float w, h;
                         item->get_size( w, h );
                         spos.y += h;
                         item->draw_frames();
                    }
               }
          }
          else
          {
               for( auto item : _vchilds )
               {
                    if( item->is_visible() )
                    {
                         item->set_base_pos( spos.x, spos.y );
                         float w, h;
                         item->get_size( w, h );
                         spos.x += w;
                         item->draw_frames();
                    }
               }
          }
          _right_bottom = spos;
          ImGui::PopClipRect();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
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
     void ft_listbox_ex::scroll( float delta )
     {
          if( _pt._vertical )
          {
               _left_top.y += delta;
          }
          else
          {
               _left_top.x += delta;
          }

     }
}