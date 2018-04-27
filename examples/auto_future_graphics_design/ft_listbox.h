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
	void collect_property_range(vproperty_list& vplist)
	{
		ft_base::collect_property_range(vplist);
		vplist.emplace_back(&_lt_pt, sizeof(intl_pt));
	}
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
#endif

	bool handle_mouse();

};
REGISTER_CONTROL(ft_listbox)
