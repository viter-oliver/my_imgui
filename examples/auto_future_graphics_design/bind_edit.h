#pragma once
#include "af_bind.h"
#define TXT_BUFF_SZ 1024 * 16
class bind_edit
{
	prop_ele_bind_unit* _pnew_bind_unit{nullptr};
	prop_ele_position _current_prop_ele;
	string _exp_calcu;
	char txt_buff[TXT_BUFF_SZ];
	bool _dragging{ false };
	bool _hit_bind_window{ false };
	int _pgidx, _fdidx;
	bool _be_unsavable{ true };
	bool _edit_new_obj{ true };
public:
	bind_edit()
	{
	}
	void bind_source_view();
	void set_dragging(bool be_dragging,base_ui_component* pobj=nullptr, uint16_t page_idx=0, uint16_t off_idx=0);
	void sel_prop_ele(base_ui_component* pobj, uint16_t page_idx, uint16_t off_idx);
};
