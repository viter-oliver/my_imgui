#pragma once
#include "ft_base.h"
class ft_listbox :
	public ft_base
{
	struct intl_pt
	{
		unsigned char _vertical;
		int _width, _height;
		intl_pt() :_vertical(true), _width(0), _height(0){}
	};
	intl_pt _lt_pt;
public:
	ft_listbox();
	int collect_property_range(vproperty_list& vplist)
	{
		int plen = ft_base::collect_property_range(vplist);
		int len = sizeof(intl_pt);
		vplist.emplace_back(&_lt_pt, len);
		len += plen;
		return len;
	}
	void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
#endif

	bool handle_mouse();

};
REGISTER_CONTROL(ft_listbox)
