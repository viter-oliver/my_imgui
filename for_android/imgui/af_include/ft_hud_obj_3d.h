#pragma once
#include "ft_base.h"
#include "af_shader.h"
#include "res_output.h"
#include "af_primitive_object.h"
namespace zl_future
{
     class ft_hud_obj_3d :
          public ft_base
     {
          DEF_STRUCT_WITH_INIT( pty_page, _pt_tb,
                                ( char, _attached_image[ FILE_NAME_LEN ] ),
                                ( float, _tanslation_x ),
                                ( float, _tanslation_y ),
                                ( float, _tanslation_z ) )
          static ps_shader _phud_sd;
          static ps_primrive_object _ps_prm; 
          ps_af_texture _pat_image;
          
     public:

          ft_hud_obj_3d();
          ~ft_hud_obj_3d();
          void link();
          void draw();
          void set_transx( float transx )
          {
               _pt_tb._tanslation_x = transx;
          }
          void set_transy( float transy )
          {
               _pt_tb._tanslation_y = transy;
          }
          void set_transz( float transz )
          {
               _pt_tb._tanslation_z = transz;
          }
     };
     REGISTER_CONTROL(ft_hud_obj_3d);
}