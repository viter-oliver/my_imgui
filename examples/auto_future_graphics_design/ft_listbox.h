#pragma once
#include "ft_base.h"
class ft_listbox :
	public ft_base
{
	
	struct intl_pt
	{
		bool _vertical;
		ImVec2 _size;
		ImVec2 _range;
		intl_pt()
			:_vertical(true), _size(100.f, 100.f), _range(1.f,100.f)
		{}
	};
	intl_pt _lt_pt;
	float _scroll_value = { 0.f };
	float scroll_max();
	void set_scroll_value(float scvalue);
public:
	ft_listbox();
	void set_logic_scroll_value(float scvalue);
	int collect_property_range(vproperty_list& vplist)
	{
		int plen = ft_base::collect_property_range(vplist);
		int len = sizeof(intl_pt);
		vplist.emplace_back(&_lt_pt, len);
		len += plen;
		return len;
	}
	void draw();
	void add_child(base_ui_component* pchild);
	void remove_child(base_ui_component* pchild);
	ImVec2& get_logic_range()
	{
		return _lt_pt._range;
	}

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
	bool init_json_unit(Value& junit);
#endif

	bool handle_mouse();

};
REGISTER_CONTROL(ft_listbox)
