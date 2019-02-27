#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_block :
		public ft_base
	{
		struct intl_pt
		{
			ImVec2 _size;
			ImVec4 _block_color;
			intl_pt(){}
		};
		intl_pt _pt;
	public:
		ft_block(){}
		~ft_block(){}
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int len = sizeof(intl_pt);
			vplist.emplace_back(&_pt, len);
			len += plen;
			return len;
		}
		ImVec2 get_size()
		{
			return _pt._size;
		}
		void set_size(float w, float h)
		{
			_pt._size.x = w;
			_pt._size.y = h;
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
