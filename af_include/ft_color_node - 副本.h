#pragma once
#include "ft_base.h"
#include "af_shader.h"
#include "af_primitive_object.h"
namespace auto_future
{
     class AFG_EXPORT ft_color_node :
          public ft_base
     {
          DEF_STRUCT_WITH_INIT( intl_pt, _pt,
			   (char, _attached_obj[FILE_NAME_LEN]),
               ( int, _trans_order, { 0 } ),
               ( float, _trans_translation_x, { 0.f } ),
               ( float, _trans_translation_y, { 0.f } ),
               ( float, _trans_translation_z, { 0.f } ),
               ( float, _trans_scale_x, { 1.f } ),
               ( float, _trans_scale_y, { 1.f } ),
               ( float, _trans_scale_z, { 1.f } ),
               ( float, _rotate_angle_hac, { 0 } ),
			   (af_vec3, _rotate_axis),
			   (af_vec3, _ambient_clr),
			   (af_vec3, _diffuse_clr),
			   (af_vec3, _specular_clr))
		  static ps_shader _pcolor_node_sd;
		  ps_primrive_object _ps_prm = { nullptr };
     public:
		 ft_color_node();
#define DEF_TRANSLATION(x)\
		  ft_color_node& set_translate_##x(float v##x){\
			  _pt._trans_translation_##x= v##x;\
			  return *this;\
		  }
		 DEF_TRANSLATION(x)
			 DEF_TRANSLATION(y)
			 DEF_TRANSLATION(z)
#define DEF_SCALE(x)\
		  ft_color_node& set_scale_##x(float v##x){\
			  _pt._trans_scale_##x= v##x;\
			  return *this;\
		  }
		  DEF_SCALE(x)
		  DEF_SCALE(y)
		  DEF_SCALE(z)
		  ft_color_node&  set_rotate(af_vec3 axis, float angle){
			  _pt._rotate_axis = axis;
			  _pt._rotate_angle_hac = angle;
			  return *this; 
		 }
#define DEF_COLOR_SET(item)\
	      ft_color_node&  set_##item(af_vec3 clr){\
			  _pt._##item##_clr=clr;\
			  return *this;\
		  }
		  DEF_COLOR_SET(ambient)
		  DEF_COLOR_SET(diffuse)
		  DEF_COLOR_SET(specular)
		  void set_prm_obj(std::string prm_name){
			  strcpy(_pt._attached_obj, prm_name.c_str());
		  }
          void transform( glm::mat4& model );
		  void link();
		  void draw();
     };
     REGISTER_CONTROL( ft_color_node )
}
