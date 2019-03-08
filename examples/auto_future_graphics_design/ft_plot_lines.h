#pragma once
#include "ft_base.h"
namespace auto_future
{ 
    #define MAX_VALUE_COUNT 50
	class AFG_EXPORT ft_plot_lines :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(intl_pt,_pt,
			(int, _v_count, {10}),
			(float, _min, {-1.f}),
			(float, _max, {1.f}))
		float _values[MAX_VALUE_COUNT] ;
		int _algebra{ 0 };
	public:
		ft_plot_lines();
		~ft_plot_lines(){}
		
		void draw();
	};
	REGISTER_CONTROL(ft_plot_lines)
}
