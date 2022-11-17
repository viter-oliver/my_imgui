#pragma once
#include "ft_base.h"
#include "af_shader.h"
namespace auto_future
{
     class AFG_EXPORT ft_raw_trans :
          public ft_base
     {
          DEF_STRUCT_WITH_INIT( intl_pt, _pt,
               (float, _trans_mat4x4[16]) )
     public:
          ft_raw_trans();
          void transform( glm::mat4& model );
          void set_trans_mat(float* pfvalue){
			memcpy(_pt._trans_mat4x4, pfvalue, 16 * sizeof(float));
		  }
     };
     REGISTER_CONTROL( ft_raw_trans )
}
