#pragma once
#include "ft_base.h"
namespace auto_future
{ 
    #define MAX_VALUE_COUNT 50
	class AFG_EXPORT ft_plot_lines :
		public ft_base
	{
		struct intl_pt
		{
			ImVec2 _size = { 400, 200 };
			int _v_count = { 10 };
			intl_pt()
			{
			}
		};
		float _values[MAX_VALUE_COUNT] ;
		intl_pt _pt;

	public:
		ft_plot_lines()
			:ft_base(){
				_values[0] = 0.6;
				_values[1] = 0.1;
				_values[2] = 1.0;
				_values[3] = 0.5;
				_values[4] = 0.2;
				_values[5] = 0.9;
				_values[6] = 0.2;
				_values[7] = 0.7;
				_values[8] = 0.4;
				_values[9] = 0.6;

		}
		~ft_plot_lines(){}
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int len = sizeof(intl_pt);
			vplist.emplace_back(&_pt, len);
			len += plen;
			return len;
		}
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page(int property_part = -1);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};
	REGISTER_CONTROL(ft_plot_lines)
}
