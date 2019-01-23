#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_circle :
		public ft_base
	{


		DEF_STRUCT_WITH_INIT(intl_pt,_pt,
			(float, _radius, {1.f}),
			(float, _thickness_utn, { 1.f }),
			(int, _numb_segs, { 20 }),
			(bool, _solid, {false}),
			(af_vec4, _bkclr))

	public:
		ft_circle();
		~ft_circle(){}
		void draw();
		bool contains(float posx, float posy);
		bool relative_contain(af_vec2& point);
	};
	REGISTER_CONTROL(ft_circle)
}
