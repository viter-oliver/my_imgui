#pragma once
#include "ft_base.h"
#define TXT_BUFF_SZ 1024 * 16
class bind_edit
{
	prop_ele_bind_unit _new_bind_unit;
	prop_ele_position _current_prop_ele;
	char txt_buff[TXT_BUFF_SZ];
	bool _dragging{ false };
	bool _hit_bind_window{ false };
	int _pgidx, _fdidx;
public:

	void bind_source_view();
	void set_dragging(bool be_dragging,base_ui_component* pobj=nullptr, uint16_t page_idx=0, uint16_t off_idx=0);
	void sel_prop_ele(base_ui_component* pobj, uint16_t page_idx, uint16_t off_idx)
	{
		_current_prop_ele._pobj = pobj;
		_current_prop_ele._page_index = page_idx;
		_current_prop_ele._field_index = off_idx;
		auto ibind = g_bind_dic.find(_current_prop_ele);
		if (ibind != g_bind_dic.end())
		{
			auto&exp = ibind->second->_expression;
			memcpy(txt_buff, &exp[0], exp.size());
			txt_buff[exp.size()] = '\0';
		}
	}
};
