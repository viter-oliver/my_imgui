#pragma once
#include "ft_base.h"
#include "af_shader.h"
#include "af_primitive_object.h"
#include "res_output.h"

namespace auto_future
{
     class AFG_EXPORT ft_1_texture_mesh :
          public ft_base
     {
          DEF_STRUCT_WITH_INIT( intl_pt, _pt,
			  (char, _attached_obj[FILE_NAME_LEN]),
			   (char, _attached_txt[FILE_NAME_LEN]))
		  static ps_shader _ptxt_node_sd;
		  ps_primrive_object _ps_prm = { nullptr };
		  ps_af_texture _ps_txt = { nullptr };
     public:
		 ft_1_texture_mesh();

		  void set_prm_obj(std::string prm_name){
			  strcpy(_pt._attached_obj, prm_name.c_str());
		  }
		  void set_txt_obj(std::string txt_name){
			  strcpy(_pt._attached_txt, txt_name.c_str());
		  }
		  void link();
		  void draw();
     };
     REGISTER_CONTROL( ft_1_texture_mesh )
}
