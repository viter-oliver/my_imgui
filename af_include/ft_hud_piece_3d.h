#pragma once
#include "ft_base.h"
#include "af_shader.h"
#include "res_output.h"
#include "af_primitive_object.h"
namespace auto_future
{
	class AFG_EXPORT ft_hud_piece_3d :
          public ft_base
     {
          DEF_STRUCT_WITH_INIT( pty_page, _pt_tb,
								(float, _near, { -1.f }),
								(float, _far, { -1.f }),
                                ( char, _attached_image[ FILE_NAME_LEN ] ))
          static ps_shader _phud_sd;
		  static ps_primrive_object _ps_prm;
          ps_af_texture _pat_image;
          
     public:

          ft_hud_piece_3d();
          ~ft_hud_piece_3d();
          void link();
          void draw();
         
     };
     REGISTER_CONTROL( ft_hud_piece_3d );
}