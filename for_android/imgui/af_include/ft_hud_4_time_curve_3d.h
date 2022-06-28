#pragma once
#include "ft_base.h"
#include "af_shader.h"
#include "res_output.h"
#include "af_primitive_object.h"

namespace zl_future
{
     class ft_hud_4_time_curve_3d :
          public ft_base
     {
          DEF_STRUCT_WITH_INIT( pty_page, _pt_tb,
                                ( char, _attached_image[ FILE_NAME_LEN ] ),
                                ( float, _coeff_hac[4] ),
                                ( bool, _left_border, {true} ),
                                ( float, _width, { 100.f } ),
                                ( float, _tanslation_x, { 0.f } ),
                                ( float, _tanslation_y, { 0.f } ),
                                ( float, _tanslation_z, { 0.f } ))
          static ps_shader _phud_sd;
          static ps_primrive_object _ps_prm;
          ps_af_texture _pat_image;
     public:
          ft_hud_4_time_curve_3d();
          ~ft_hud_4_time_curve_3d();
          void link();
          void draw();
     };
     REGISTER_CONTROL(ft_hud_4_time_curve_3d);
}


