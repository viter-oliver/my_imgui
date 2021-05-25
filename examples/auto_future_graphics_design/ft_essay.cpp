#include "ft_essay.h"
#include "ft_sentence.h"
#include <random>
#include <algorithm>
namespace auto_future
{
     ft_essay::ft_essay()
     {}


     ft_essay::~ft_essay()
     {}
     static default_random_engine generator;
     static uniform_real_distribution<float> distribution( 0, 1.f );

     void ft_essay::load_content( wstring& str_content )
     {
          if( !_pfont_unit )
          {
               return;
          }
          for( auto it_child = _vchilds.begin(); it_child != _vchilds.end(); )
          {
               auto pchild = *it_child;
               delete pchild;
               it_child = _vchilds.erase( it_child );
          }
          wstring wsparagraph;
          int chd_cnt = 0;
          srand( unsigned( time( 0 ) ) );
          for (auto& ch:str_content)
          {
               if( ch == 0x3002|| ch ==0x230)
               {
                    wsparagraph += ch;
                    auto pnew_sentence = new ft_sentence( _pfont_unit, wsparagraph );
                    float r = distribution( generator );
                    float g = distribution( generator );
                    float b = distribution( generator );

                    pnew_sentence->set_txt_clr( r, g,b,1.f );
                    pnew_sentence->load_idx = chd_cnt++;
                    add_child( pnew_sentence );
                    wsparagraph.clear();
               }
               else if( ch == 0xd&& wsparagraph.size()>0)
               {
                    auto pnew_sentence = new ft_sentence( _pfont_unit, wsparagraph );
                    pnew_sentence->load_idx = chd_cnt++;
                    add_child( pnew_sentence );
                    wsparagraph = ch;
               }
               else
               {
                    wsparagraph += ch;
               }
          }
     }

     void ft_essay::draw()
     {
          if (!_pfont_unit)
          {
               return;
          }
          auto wsz = ImGui::GetWindowSize();
          auto winpos = ImGui::GetWindowPos();
          auto l_edge = hmargin+winpos.x;
          auto r_edge = wsz.x - hmargin+winpos.x;
          af_vec2 draw_pos = {hmargin,vmargin};
          af_vec2 prev_end_pos;
          for (auto ich:_vchilds)
          {
               ft_sentence* pstc = static_cast<ft_sentence*>( ich );
               pstc->init_edge( l_edge, r_edge );
               pstc->line_spacing = line_spacing;
               if (pstc->is_head_of_a_paragraph())
               {
                    draw_pos.x = hmargin;
                    draw_pos.y = prev_end_pos.y - winpos.y + _in_p._posy;
               }
               pstc->set_base_pos( draw_pos.x, draw_pos.y );
               pstc->draw();
               draw_pos.x = pstc->_endpos.x-winpos.x+_in_p._posx;
               prev_end_pos = pstc->_endpos;
               if (!pstc->is_same_line())
               {
                    draw_pos.y = pstc->_r_bottom_edge.y - winpos.y + _in_p._posy;;
               }
          }
          ImGuiContext& g = *GImGui;
          ImGuiWindow* cur_window = ImGui::GetCurrentWindow();
          ImGuiWindow* front_window = g.Windows.back();
          ImRect wrect( cur_window->Pos, cur_window->Pos + cur_window->Size );
          auto swap_ui = [&]( base_ui_component* pui1, base_ui_component*pui2 )
          {
               int isiz = _vchilds.size();
               int i1, i2;
               i1 = i2 = -1;
               for( int it = 0; it < isiz; it++ )
               {
                    if( _vchilds[ it ] == pui1 )
                    {
                         i1 = it;
                    }
                    if( _vchilds[ it ] == pui2 )
                    {
                         i2 = it;
                    }
                    if( i1 != -1 && i2 != -1 )
                    {
                         break;
                    }
               }
               _vchilds[ i2 ] = pui1;
               _vchilds[ i1 ] = pui2;
          };
           if( cur_window == front_window && wrect.Contains( ImGui::GetIO().MousePos ) )
          {
               static base_ui_component* psel_ui = nullptr;
               ImGuiIO& io = ImGui::GetIO();

               if( ImGui::IsMouseClicked( 0 ) )
               {
                    psel_ui = get_hit_ui_object(io.MousePos.x, io.MousePos.y );
               }
               if( psel_ui&&ImGui::IsMouseReleased( 0 ) )
               {
                    base_ui_component* phit = get_hit_ui_object( io.MousePos.x, io.MousePos.y );
                    if (phit)
                    {
                         swap_ui( psel_ui, phit );
                         psel_ui = nullptr;
                         if( orignal_order())
                         {
                              auto currentTime = steady_clock::now();
                              int delta = chrono::duration_cast<chrono::duration<int, std::milli>>( currentTime - _play_start ).count();
                              //printf( "msg_host consume%d milli seconds\n", delta );
                              consume_seconds = delta / 1000;
                              if (delta<10000)
                              {
                                   game_state = "You did a unbelievable job!";
                              }
                              else if(delta<20000 )
                              {
                                   game_state = "You did a excellent job!";
                              }
                              else if (delta<600000)
                              {
                                   game_state = "You did a good job!";
                              }
                              else
                              {
                                   game_state = "Fail!";
                              }
                         }
                    }
               }
               
               if( psel_ui&&ImGui::IsMouseDragging( 0 ) )
               {
                    ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    draw_list->PushClipRectFullScreen();
                    draw_list->AddLine( io.MouseClickedPos[ 0 ], io.MousePos, ImGui::GetColorU32( ImGuiCol_Button ), 4.0f );
                    draw_list->PopClipRect();
               }
               
          }
     }

     void ft_essay::shuffle()
     {
          game_state = "";
          _play_start = steady_clock::now();
          consume_seconds = 0;
          srand( unsigned( time( 0 ) ) );
          random_shuffle( _vchilds.begin(), _vchilds.end() );
     }

     bool ft_essay::orignal_order()
     {
          int ix = 0;
          for (auto& itc:_vchilds)
          {
               ft_sentence* pst = static_cast<ft_sentence*>( itc );
               if( pst->load_idx !=ix)
               {
                    return false;
               }
               ix++;
          }
          return true;
     }

}