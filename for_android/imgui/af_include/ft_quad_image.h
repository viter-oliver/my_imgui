#pragma once
#include "ft_base.h"
namespace zl_future
{

	class AFG_EXPORT ft_quad_image :
		public ft_base
	{
	
		DEF_STRUCT_WITH_INIT(intl_pt,_img_pt,
			(af_vec2, _imge_pos[4]),
			(af_vi2, _texture_index_txt))
	public:	
		ft_quad_image();// : ft_base(){}
		
		void draw();
	};

	REGISTER_CONTROL(ft_quad_image)
}
