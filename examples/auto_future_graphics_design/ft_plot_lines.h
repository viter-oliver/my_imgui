#pragma once
#include "ft_base.h"
namespace auto_future
{ 
    #define MAX_VALUE_COUNT 50
	class AFG_EXPORT ft_plot_lines :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(intl_pt,_pt,
			(float, _sizew, {400.f}),
			(float, _sizeh, {200.f}),
			(int, _v_count, {10}))
		float _values[MAX_VALUE_COUNT] ;

	public:
		ft_plot_lines();
		~ft_plot_lines(){}
		
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};
	REGISTER_CONTROL(ft_plot_lines)
}
