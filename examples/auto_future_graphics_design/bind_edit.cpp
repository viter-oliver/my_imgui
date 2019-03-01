#include "bind_edit.h"
#include "user_control_imgui.h"
#include "res_output.h"
#include <GLFW/glfw3.h>
#include "common_functions.h"
#include "python_interpreter.h"
void bind_edit::set_dragging(bool be_dragging, base_ui_component* pobj, uint16_t page_idx, uint16_t off_idx)
{
	if (!be_dragging&&_hit_bind_window)
	{
		//string uname = pobj->get_name();
		//printf("we get %s a prop\n", uname.c_str());
		auto& pmlist = _new_bind_unit._param_list;
		pmlist.emplace_back();
		auto pm_idx = pmlist.size() - 1;
		auto& prop_ele_p = pmlist[pm_idx];
		prop_ele_p._pobj = pobj;
		prop_ele_p._page_index = _pgidx;
		prop_ele_p._field_index = _fdidx;
		_hit_bind_window = false;
	}
	if (be_dragging)
	{
		_pgidx = page_idx;
		_fdidx = off_idx;
	}
	_dragging = be_dragging;
}
void get_uic_path(base_ui_component* pobj, string& path_rtn)
{
	assert(pobj);
	vector<string> name_list;
	base_ui_component* ppt = nullptr;
	base_ui_component* pcur = pobj;
	name_list.push_back(pcur->get_name());
	while (ppt = pcur->get_parent())
	{
		name_list.push_back(ppt->get_name());
		pcur = ppt;
	}
	auto name_list_sz = name_list.size();
	for (int ix = name_list_sz - 1; ix >= 0;ix--)
	{
		path_rtn += '/';
		path_rtn += name_list[ix];
	}
}
void bind_edit::bind_source_view()
{
	if (!_current_prop_ele._pobj)
	{
		return;
	}
	string cur_name;
	get_uic_path(_current_prop_ele._pobj, cur_name);
	auto& cur_pgidx = _current_prop_ele._page_index;
	auto& cur_fieldidx = _current_prop_ele._field_index;
	auto& cur_pt_page = _current_prop_ele._pobj->get_prop_ele(cur_pgidx);
	auto& cur_field = _current_prop_ele._pobj->get_filed_ele(cur_pgidx, cur_fieldidx);
	cur_name += "::";
	cur_name += cur_field._name;
	string bind_show = cur_name + "=";
	ImGui::Text(bind_show.c_str());
	auto& prop_ele_unit = _new_bind_unit;
	auto ibind = g_bind_dic.find(_current_prop_ele);
	if (ibind != g_bind_dic.end())
	{
		prop_ele_unit = *ibind->second;
	}
	auto& expression = prop_ele_unit._expression;
	int ix = 0;
	auto& param_list = prop_ele_unit._param_list;
	string param_pass;
	char* ppt_addr = (char*)cur_pt_page._pro_address + cur_field._offset;
	var_unit vrtn(cur_field._type, ppt_addr);
	variable_list vlist;
	for (auto itp = param_list.begin(); itp != param_list.end();)//(auto&prop_ele_p:prop_ele_unit._param_list)
	{
		string va = "v";
		char cc[50] = { 0 };
		itoa(ix, cc, 10);
		va += cc;
		if (ix>0)
		{
			param_pass += ',';
		}
		param_pass += va;

		va += "=";
		auto&prop_ele_p = *itp;
		get_uic_path(prop_ele_p._pobj, va);
		auto& pgidx = prop_ele_p._page_index;
		auto& fidx = prop_ele_p._field_index;
		auto& pt_pg = prop_ele_p._pobj->get_prop_ele(pgidx);
		field_ele &fel = prop_ele_p._pobj->get_filed_ele(pgidx,fidx);
		char* pm_value = (char*)pt_pg._pro_address + fel._offset;
		vlist.emplace_back(var_unit(fel._type, pm_value));
		va += "::";
		va += fel._name;
		ImGui::Text(va.c_str());
		ImGui::SameLine();
		string del_btn = "X##";
		del_btn += cc;
		if (ImGui::Button(del_btn.c_str()))
		{
			itp = param_list.erase(itp);
		}
		else
		{
			itp++;
		}
		ix++;
	}
	auto edit_ht = ImGui::GetTextLineHeight() * 16;
	auto txt_flag = ImGuiInputTextFlags_AllowTabInput;
	if (ImGui::InputTextMultiline("bind source", txt_buff, TXT_BUFF_SZ, ImVec2(-1.0f, edit_ht), txt_flag))
	{
		expression = txt_buff;
	}
	_hit_bind_window = _dragging&&ImGui::IsMouseHoveringWindow();
	
	if (ImGui::Button("test"))
	{
		string exp = python_fun_head;
		exp += param_pass;
		exp += "):\n";
		string fun_content;
		align_expression(expression, fun_content);
		exp += fun_content;
		bool be_success=g_python_intp.call_python_fun(exp, python_fun_name, vrtn, vlist);
	}

}