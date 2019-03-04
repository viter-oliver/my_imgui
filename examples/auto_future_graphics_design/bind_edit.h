#pragma once
#include "ft_base.h"
#define TXT_BUFF_SZ 1024 * 16
class bind_edit
{
	prop_ele_bind_unit* _pnew_bind_unit{nullptr};
	prop_ele_position _current_prop_ele;
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
	void sel_prop_ele(base_ui_component* pobj, uint16_t page_idx, uint16_t off_idx)
	{
		auto& cobj = _current_prop_ele._pobj;
		auto& cpgidx = _current_prop_ele._page_index;
		auto& cfdidx = _current_prop_ele._field_index;
		if (cobj==pobj&&cpgidx==page_idx&&cfdidx==off_idx)
		{
			return;
		}
		cobj = pobj;
		cpgidx = page_idx;
		cfdidx = off_idx;
		if (_edit_new_obj)
		{
			if (_pnew_bind_unit)
			{
				delete _pnew_bind_unit;
			}
		}
		_edit_new_obj = true;
		_be_unsavable = true;
		//_pnew_bind_unit._param_list.clear();
		//_pnew_bind_unit._expression.clear();
		auto ibind = g_bind_dic.find(_current_prop_ele);
		if (ibind != g_bind_dic.end())
		{
			_pnew_bind_unit = ibind->second.get();
			auto&exp = _pnew_bind_unit->_expression;
			//txt_buff = exp;
			auto slen = exp.length();
			memcpy(txt_buff, exp.c_str(), slen);
			txt_buff[slen] = 0;
			//strcpy((char*)txt_buff.c_str(), exp.c_str());
			_edit_new_obj = false;
			_be_unsavable = false;
		}
		else{
			_pnew_bind_unit = new prop_ele_bind_unit();
			txt_buff[0] = '\0';
			//memset(txt_buff, 0, TXT_BUFF_SZ);
		}
	}
};
