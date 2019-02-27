#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_block :
		public ft_base
	{


		DEF_STRUCT_WITH_INIT(intl_pt,_pt,
			(float, _round_utn, {0.f}),
			(bool, _with_border, {false}),
			(float, _border_width_uhd, {1.f}),
			(af_vec4, _bkclr),
			(af_vec4, _bdclr))

	public:
		ft_block();
		~ft_block(){}

		void set_background_color(float r, float g, float b, float a)
		{
			_pt._bkclr.x = r;
			_pt._bkclr.y = g;
			_pt._bkclr.z = b;
			_pt._bkclr.w = a;
		}
		void set_border_color(float r, float g, float b, float a)
		{
			_pt._bdclr.x = r;
			_pt._bdclr.y = g;
			_pt._bdclr.z = b;
			_pt._bdclr.w = a;
		}
		void draw();
	};
	REGISTER_CONTROL(ft_block)
}
