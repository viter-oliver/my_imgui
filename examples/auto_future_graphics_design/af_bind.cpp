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
				auto& field = ref_pos._pobj->get_filed_ele(pgidx, fdidx);
				char* ppt_addr = field._address;
				var_unit vrtn(field._type, ppt_addr);
				variable_list vlist;
				auto& param_list = ibind_ut->second->_param_list;
				for (auto& param:param_list)
				{
					auto& cpgidx = param._page_index;
					auto& cfdidx = param._field_index;
					auto& cfel = param._pobj->get_filed_ele(cpgidx,cfdidx);
					char* pm_value = cfel._address;
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
aliase_map g_aliase_dic;
bool set_property_aliase_value(string prp_aliase_name, void* pvalue)
{
	auto& ialiase = g_aliase_dic.find(prp_aliase_name);
	if (ialiase==g_aliase_dic.end())
	{
		printf("unknown alias name:%s\n", prp_aliase_name.c_str());
		return false;
	}
	auto& prop_pos =*ialiase->second;
	return prop_pos._pobj->set_prop_fd_value(prop_pos._page_index, prop_pos._field_index, pvalue);
}