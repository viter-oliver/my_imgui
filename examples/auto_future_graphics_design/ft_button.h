#pragma once
#include "ft_base.h"
class ft_button :
	public ft_base
{
	struct intl_pt
	{
		int _texture_indices[2];
		string _label;
	};
	intl_pt _pt;
public:
	ft_button();
	//ft_button(int tid) :base_ui_component(tid){}
	void draw();
	void collect_property_range(vproperty_list& vplist)
	{
		ft_base::collect_property_range(vplist);
		vplist.emplace_back(&_pt, sizeof(intl_pt));
	}
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
#endif

	bool handle_mouse();
};
REGISTER_CONTROL(ft_button)
