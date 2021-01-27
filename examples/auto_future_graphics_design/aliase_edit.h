#pragma once
#include "af_bind.h"
class aliase_edit
{
	string _key_name;
	string _popup_cap;
	ps_prp_ele_pos _psel{ nullptr };
	prop_ele_position _edit_prp_ele_pos;
public:
	void aliase_dic_view();
	void aliase_item_propoerty();
	void popup_new_aliase();
	void sel_aliase(base_ui_component* pobj, int pg_idx, int fd_idx);
     void clear_sel()
     {
          _psel = nullptr;
          _edit_prp_ele_pos._pobj = nullptr;
     }
};

