#pragma once
#include "control_common_def.h"

struct prop_ele_position
{
	base_ui_component* _pobj;
	uint16_t _page_index;
	uint16_t _field_index;

	bool operator <(const prop_ele_position& tpp) const
	{
		if (_pobj != tpp._pobj)
		{
			return _pobj < tpp._pobj;
		}
		else
		{
			if (_page_index != tpp._page_index)
			{
				return _page_index < tpp._page_index;
			}
			else
			{
				return _field_index < tpp._field_index;
			}
		}
	}
	bool operator ==(const prop_ele_position& tpp) const
	{
		bool be_equal = _pobj == tpp._pobj&&_page_index == tpp._page_index&&_field_index == tpp._field_index;
		return be_equal;
	}
};
using vprop_pos = vector<prop_ele_position>;
struct prop_ele_bind_unit
{
	vprop_pos _param_list;
	//vector<prop_ele_position> _ref_list;
	string _expression;
	/*prop_ele_bind_unit()
	{
	_expression.resize(1024 * 16);
	}*/
};

using bind_dic = map<prop_ele_position, shared_ptr<prop_ele_bind_unit>>;
using bind_ref_dic = map<prop_ele_position, shared_ptr<vprop_pos>>;
using prop_ele_pos_index = vector<unsigned short>;

extern bind_dic g_bind_dic;
extern bind_ref_dic g_bind_ref_dic;
extern void calcu_bind_node(prop_ele_position& pep);
extern void calcu_prop_ele_pos_index(const prop_ele_position& pep, prop_ele_pos_index& pep_id); 
using ps_prp_ele_pos = shared_ptr<prop_ele_position>;
using aliase_map = map<string, ps_prp_ele_pos>;
extern aliase_map g_aliase_dic;
extern bool set_property_aliase_value(string prp_aliase_name, void* pvalue);