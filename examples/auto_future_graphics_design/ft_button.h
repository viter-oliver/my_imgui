#pragma once
#include "ft_base.h"
class ft_button :
	public ft_base
{
	struct intl_pt
	{
		int _texture_indices[2];
		char _label[FILENAME_MAX];
	};
	intl_pt _pt;
public:
	ft_button();
	//ft_button(int tid) :base_ui_component(tid){}
	void draw();
	int collect_property_range(vproperty_list& vplist)
	{
		int plen=ft_base::collect_property_range(vplist);
		int len = sizeof(intl_pt);
		vplist.emplace_back(&_pt, len);
		len += plen;
		return len;

	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
#endif

	bool handle_mouse();
};
REGISTER_CONTROL(ft_button)
