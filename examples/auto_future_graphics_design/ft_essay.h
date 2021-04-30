#pragma once
#include "ft_base.h"
#include "ft_paragraph.h"
namespace auto_future
{
     class ft_essay :
          public ft_base
     {
     public:
          float hmargin = { 2.f }, vmargin = { 2.f };
          ft_essay();
          ~ft_essay();
          void load_content( wstring& str_content );
     };
}
