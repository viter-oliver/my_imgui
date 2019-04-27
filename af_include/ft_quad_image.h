#pragma once
#include "ft_base.h"
namespace auto_future
{
	enum anchor_type
	{
		en_anchor_top_left,
		en_anchor_top_right,
		en_anchor_bottom_right,
		en_anchor_bottom_left,
		en_anchor_center,
	};
	class AFG_EXPORT ft_quad_image :
		public ft_base
	{
	
		DEF_STRUCT_WITH_INIT(intl_pt,_img_pt,
			(af_vec2, _imge_pos[4]),
			(int, _texture_index_txt, { 0 }))
	public:	
		ft_quad_image();// : ft_base(){}
		
		void draw();
	};

	REGISTER_CONTROL(ft_quad_image)
}
