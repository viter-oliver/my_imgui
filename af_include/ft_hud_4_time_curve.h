#pragma once
#include "ft_base.h"
#include "af_shader.h"
#include "res_output.h"
#include "af_primitive_object.h"

namespace auto_future
{
     class ft_hud_4_time_curve :
          public ft_base
     {
          DEF_STRUCT_WITH_INIT( pty_page, _pt_tb,
                                ( char, _attached_image[ FILE_NAME_LEN ] ),
                                ( float, _c[4] ),
                                ( bool, _left_border, {true} ),
                                ( float, _width, {100.f} ) )
          static ps_shader _phud_sd;
          static ps_primrive_object _ps_prm;
          struct assist
          {
               assist();
          };
          static assist ass;
          ps_af_texture _pat_image;
     public:
          ft_hud_4_time_curve();
          ~ft_hud_4_time_curve();
          void link();
          void draw();
     };
}


