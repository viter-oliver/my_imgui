#include "af_bind.h"
#include "python_interpreter.h"
bind_dic g_bind_dic;
bind_ref_dic g_bind_ref_dic;

void calcu_bind_node(prop_ele_position& pep)
{
	auto& iref = g_bind_ref_dic.find(pep);
	if (iref!=g_bind_ref_dic.end())
	{
		auto& ref_list = *iref->second;
		for (auto& ref_pos:ref_list)
		{
			auto& ibind_ut = g_bind_dic.find(ref_pos);
			if (ibind_ut!=g_bind_dic.end())
			{
				auto& pgidx = ref_pos._page_index;
				auto& fdidx = ref_pos._field_index;
				auto& pt_page = ref_pos._pobj->get_prop_ele(pgidx);
				auto& field = ref_pos._pobj->get_filed_ele(pgidx, fdidx);
				char* ppt_addr = (char*)pt_page._pro_address + field._offset;
				var_unit vrtn(field._type, ppt_addr);
				variable_list vlist;
				auto& param_list = ibind_ut->second->_param_list;
				for (auto& param:param_list)
				{
					auto& cpgidx = param._page_index;
					auto& cfdidx = param._field_index;
					auto& cpt_page = param._pobj->get_prop_ele(cpgidx);
					auto& cfel = param._pobj->get_filed_ele(cpgidx,cfdidx);
					char* pm_value = (char*)cpt_page._pro_address + cfel._offset;
					vlist.emplace_back(var_unit(cfel._type,pm_value));
				}
				auto& exp_calcu = ibind_ut->second->_expression;
				bool be_success = g_python_intp.call_python_fun(exp_calcu, python_fun_name, vrtn, vlist);
				calcu_bind_node(ref_pos);
			}
		}
	}
}
void calcu_prop_ele_pos_index(const prop_ele_position& pep, prop_ele_pos_index& pep_id)
{
	pep_id.emplace_back(pep._field_index);
	pep_id.emplace_back(pep._page_index);
	base_ui_component* ppt = nullptr;
	base_ui_component* pcur = pep._pobj;
	while (ppt=pcur->get_parent())
	{
		unsigned short idx = 0;
		size_t sz = ppt->get_child_count();
		for (;idx < sz;idx++)
		{
			auto pcd = ppt->get_child(idx);
			if (pcd==pcur)
			{
				pep_id.emplace_back(idx);
				break;
			}
		}
		pcur = ppt;
	}
}