#pragma once
#include "ft_base.h"
namespace auto_future
{
#define MAX_POS_PAIR_CNT 20
	class AFG_EXPORT ft_polygon_image :
		public ft_base
	{
		DEF_STRUCT_WITH_INIT(pg_img_pt,_img_pt,
			(int, _pos_pair_cnt, { 2 }),
			(af_vec2, _pos[MAX_POS_PAIR_CNT]),
			(int, _texture_index_txt, {0})
			)
	public:
		ft_polygon_image();
		void draw();
	};
	REGISTER_CONTROL(ft_polygon_image)
}