#pragma once
#include "ft_base.h"
#include "af_font_res_set.h"
#include <chrono>
using namespace chrono;
namespace auto_future
{
     class ft_essay :
          public ft_base
     {
          ps_font_unit _pfont_unit;
          steady_clock::time_point  _play_start;
     public:
          float hmargin = { 20.f }, vmargin = { 30.f }, line_spacing = {10.f};
          int consume_seconds = 0;
          string game_state;
          ft_essay();
          ~ft_essay();
          void set_font( ps_font_unit& pfont )
          {
               _pfont_unit = pfont;
          }
          void load_content( wstring& str_content );
          void draw();
          void shuffle();
          bool orignal_order();
     };
}
