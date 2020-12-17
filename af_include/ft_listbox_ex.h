#pragma once
#include "ft_base.h"
namespace auto_future
{
     class AFG_EXPORT ft_listbox_ex :
          public ft_base
     {
          DEF_STRUCT_WITH_INIT( intl_pt, _pt,
          ( bool, _vertical, { true } )
          )
          af_vec2 _left_top , _right_bottom ;
     public:
          ft_listbox_ex();
          void draw_frames();
          void scroll( float delta );
     };
     REGISTER_CONTROL( ft_listbox_ex )
}