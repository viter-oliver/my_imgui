#pragma once
#include "ft_base.h"
#include "af_font_res_set.h"
namespace auto_future
{
     class ft_sentence :
          public ft_base
     {
          int _font_size = { 16 };
          af_vec4 _txt_clr;
          ps_font_unit& _pfont_unit;
          wstring _wssentence;
          
     public:
          int load_idx = { 0 };
          float line_spacing = {0.f};
          af_vec2 _endpos;
          af_vec2 _l_top_edge, _r_bottom_edge;
          ft_sentence( ps_font_unit& pfont, wstring wssentence );
          ~ft_sentence();
          void init_edge( float l, float r )
          {
               _l_top_edge.x = l;
               _r_bottom_edge.x = r;
               _l_top_edge.y = _r_bottom_edge.y = 0;
          }
          void set_txt_clr( float r, float g, float b, float a )
          {
               _txt_clr.x = r;
               _txt_clr.y = g;
               _txt_clr.z = b;
               _txt_clr.w = a;
          }
          void set_font_size( int fsz )
          {
               _font_size = fsz;
          }
          bool contains( float posx, float posy );
          void draw();
          bool is_head_of_a_paragraph();
          bool is_same_line();
     };
}
