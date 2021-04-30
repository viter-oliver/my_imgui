#pragma once
#include "ft_base.h"
namespace auto_future
{
     class AFG_EXPORT ft_listbox_ex :
          public ft_base
     {
          DEF_STRUCT_WITH_INIT( intl_pt, _pt,
               ( bool, _vertical, { true } ),
               ( float, _interval, { 0.f } ),
               ( float, _inner_left, { 0.f }  ),
               ( float, _inner_top , { 0.f } ),
               ( float, _inner_right, { 0.f }  ),
               ( float, _inner_bottom, { 0.f }  )
          )
     public:
          ft_listbox_ex();
          void draw_frames();
     };
     REGISTER_CONTROL( ft_listbox_ex )
}