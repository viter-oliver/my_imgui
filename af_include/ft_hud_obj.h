#pragma once
#include "ft_base.h"
#include "af_shader.h"
#include "res_output.h"
#include "af_primitive_object.h"
namespace auto_future
{
     class ft_hud_obj :
          public ft_base
     {
          DEF_STRUCT_WITH_INIT( pty_page, _pt_tb,
                                ( char, _attached_image[ FILE_NAME_LEN ] ),
                                ( float, _tanslation_x ),
                                ( float, _tanslation_y ),
                                ( float, _tanslation_z ) )
          ps_shader _phud_sd;
          ps_af_texture _pat_image;
          ps_primrive_object _ps_prm;
     public:
          ft_hud_obj();
          ~ft_hud_obj();
          void link();
          void draw();
     };
     REGISTER_CONTROL( ft_hud_obj );
}