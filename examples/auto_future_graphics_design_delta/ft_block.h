#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_block :
		public ft_base
	{


		DEF_STRUCT(intl_pt,_pt,
			(float,_sizew),
			(float,_sizeh),
			(float,_bkr),
			(float,_bkg),
			(float,_bkb),
			(float,_bka))
	public:
		ft_block(){}
		~ft_block(){}

		ImVec2 get_size()
		{
			return ImVec2(_pt._sizew,_pt._sizew);
		}
		void set_size(float w, float h)
		{
			_pt._sizew = w;
			_pt._sizeh= h;
		}
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page(int property_part = -1);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};
	REGISTER_CONTROL(ft_block)
}
