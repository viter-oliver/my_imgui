#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ft_base.h"
#include "af_shader.h"
namespace zl_future
{
     class AFG_EXPORT ft_trans :
          public ft_base
     {
          DEF_STRUCT_WITH_INIT( intl_pt, _pt,
               ( int, _trans_order, { 0 } ),
               ( float, _trans_translation_x, { 0.f } ),
               ( float, _trans_translation_y, { 0.f } ),
               ( float, _trans_translation_z, { 0.f } ),
               ( float, _trans_scale_x, { 1.f } ),
               ( float, _trans_scale_y, { 1.f } ),
               ( float, _trans_scale_z, { 1.f } ),
               ( int, _rotate_order, { 0 } ),
               ( float, _trans_rotation_x, { 0.f } ),
               ( float, _trans_rotation_y, { 0.f } ),
               ( float, _trans_rotation_z, { 0.f } ) )
     public:
          ft_trans();
          void transform( glm::mat4& model );
     };
     REGISTER_CONTROL( ft_trans )
}
