#pragma once
#include "af_bind.h"
class bind_edit
{
	prop_ele_bind_unit* _pcur_bind_unit{nullptr};
     prop_ele_bind_unit _new_bind_unit;
	prop_ele_position _current_prop_ele;
	string _exp_calcu;
	char txt_buff[TXT_BUFF_SZ];
	bool _dragging{ false };
	bool _hit_bind_window{ false };
	int _pgidx, _fdidx;
	bool _be_unsavable{ true };
public:
	bind_edit()
	{
	}
	void bind_source_view();
	void set_dragging(bool be_dragging,base_ui_component* pobj=nullptr, uint16_t page_idx=0, uint16_t off_idx=0);
	void sel_prop_ele(base_ui_component* pobj, uint16_t page_idx, uint16_t off_idx);
     void clear_sel()
     {
          _current_prop_ele._pobj = nullptr;
          _new_bind_unit._param_list.clear();
          _be_unsavable = true;
          _pcur_bind_unit = &_new_bind_unit;
     }
     bool is_edit_new_obj()
     {
          return _pcur_bind_unit == &_new_bind_unit;
     }
};
