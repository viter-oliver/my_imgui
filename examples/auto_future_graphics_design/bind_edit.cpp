#include "bind_edit.h"
#include "user_control_imgui.h"
#include "res_output.h"
#include <GLFW/glfw3.h>
#include "common_functions.h"
#include "python_interpreter.h"
#include "ft_base.h"
//#include <algorithm>
//using namespace auto_future;
void bind_edit::set_dragging(bool be_dragging, base_ui_component* pobj, uint16_t page_idx, uint16_t off_idx)
{
	if (!be_dragging&&_hit_bind_window)
	{
		//string uname = pobj->get_name();
		//printf("we get %s a prop\n", uname.c_str());
		auto& pmlist = _pnew_bind_unit->_param_list;
	
		prop_ele_position new_prp_pos = { pobj, _pgidx, _fdidx };
		if (!(new_prp_pos==_current_prop_ele))
		{
			auto ifd = find(pmlist.begin(), pmlist.end(), new_prp_pos);
			if (ifd == pmlist.end())
			{
				pmlist.emplace_back();
				auto pm_idx = pmlist.size() - 1;
				auto& prop_ele_p = pmlist[pm_idx];
				prop_ele_p._pobj = pobj;
				prop_ele_p._page_index = _pgidx;
				prop_ele_p._field_index = _fdidx;
			}
		}
		
		_hit_bind_window = false;
	}
	if (be_dragging)
	{
		_pgidx = page_idx;
		_fdidx = off_idx;
	}
	_dragging = be_dragging;
}
void bind_edit::sel_prop_ele(base_ui_component* pobj, uint16_t page_idx, uint16_t off_idx)
{
	auto& cobj = _current_prop_ele._pobj;
	auto& cpgidx = _current_prop_ele._page_index;
	auto& cfdidx = _current_prop_ele._field_index;
	if (cobj == pobj&&cpgidx == page_idx&&cfdidx == off_idx)
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
		auto end_pos = exp.find('\n')+1;
		auto exp_content = exp.substr(end_pos);
		string exp_content_trim;
		trim_align_expression(exp_content, exp_content_trim);
		//txt_buff = exp;
		auto slen = exp_content_trim.length();
		memcpy(txt_buff, exp_content_trim.c_str(), slen);
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
	auto& cur_field = _current_prop_ele._pobj->get_filed_ele(cur_pgidx, cur_fieldidx);
	cur_name += "::";
	cur_name += cur_field._name;
	string bind_show = cur_name + "=";
	ImGui::Text(bind_show.c_str());
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	prop_ele_bind_unit* pele_bu=  _pnew_bind_unit;
	
	auto& prop_ele_unit = *pele_bu;
	int ix = 0;
	auto& param_list = prop_ele_unit._param_list;
	string param_pass;
	char* ppt_addr = cur_field._address;
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
		field_ele &fel = prop_ele_p._pobj->get_filed_ele(pgidx,fidx);
		char* pm_value = fel._address;
		vlist.emplace_back(var_unit(fel._type, pm_value));
		va += "::";
		va += fel._name;
		ImGui::Text(va.c_str());
		ImGui::SameLine();
		string del_btn = "X##";
		del_btn += cc;
		if (ImGui::Button(del_btn.c_str()))
		{
			if (!_edit_new_obj)
			{
				auto& iprop_ref = g_bind_ref_dic.find(*itp);
				if (iprop_ref!=g_bind_ref_dic.end())
				{
					auto& vppos = *iprop_ref->second;
					auto ifd = find(vppos.begin(), vppos.end(), _current_prop_ele);
					if (ifd!=vppos.end())
					{
						vppos.erase(ifd);
					}
					auto isize = vppos.size();
					if (isize==0)
					{
						g_bind_ref_dic.erase(iprop_ref);
					}
				}
			}
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
	ImGui::InputTextMultiline("bind source", txt_buff, TXT_BUFF_SZ, ImVec2(-1.0f, edit_ht), txt_flag);
	_hit_bind_window = _dragging&&ImGui::IsMouseHoveringWindow();
	if (ImGui::Button("test"))
	{
	    _exp_calcu = python_fun_head;
		_exp_calcu += param_pass;
		_exp_calcu += "):\n";
		string fun_content;
		string str_buff = txt_buff;
		align_expression(str_buff, fun_content);
		_exp_calcu += fun_content;
		bool be_success=g_python_intp.call_python_fun(_exp_calcu, python_fun_name, vrtn, vlist);
		if (!be_success)
		{
			_be_unsavable = true;
		}
		else
		{
			_be_unsavable = false;
		}
	}
	ImGui::SameLine(0,30);
	if (_be_unsavable)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	if (ImGui::Button("save"))
	{
		vprop_pos* pparam_list = &param_list;
		if (_edit_new_obj)
		{
			g_bind_dic[_current_prop_ele] = make_shared<prop_ele_bind_unit>();
			auto& cprop_ele_bu = g_bind_dic[_current_prop_ele];
			*cprop_ele_bu = *_pnew_bind_unit;
			pparam_list = &cprop_ele_bu->_param_list;
			_edit_new_obj = false;
			delete _pnew_bind_unit;
			_pnew_bind_unit = cprop_ele_bu.get();
		}
		
		_pnew_bind_unit->_expression = _exp_calcu;
		for (auto& prp_ele_pos:*pparam_list)
		{
			auto& iprop_ref = g_bind_ref_dic.find(prp_ele_pos);
			vprop_pos* pvprp_pos = nullptr;
			if (iprop_ref==g_bind_ref_dic.end())
			{
				g_bind_ref_dic[prp_ele_pos] = make_shared<vprop_pos>();
				pvprp_pos = g_bind_ref_dic[prp_ele_pos].get();
			}
			else
			{
				pvprp_pos = iprop_ref->second.get();
			}
			auto ifd = find(pvprp_pos->begin(), pvprp_pos->end(), _current_prop_ele);
			if (ifd==pvprp_pos->end())
			{
				pvprp_pos->emplace_back(_current_prop_ele);
			}
		}
	}
	if (!_edit_new_obj&&ImGui::Button("remove"))
	{
		vprop_pos& param_list=_pnew_bind_unit->_param_list;
		for (auto& prp_ele_pos:param_list)
		{
			auto& iprop_ref = g_bind_ref_dic.find(prp_ele_pos);
			if (iprop_ref != g_bind_ref_dic.end())
			{
				vprop_pos& vprp_pos = *iprop_ref->second;
				auto ifd = find(vprp_pos.begin(), vprp_pos.end(), _current_prop_ele);
				if (ifd!=vprp_pos.end())
				{
					vprp_pos.erase(ifd);
				}
				auto iszie = vprp_pos.size();
				if (iszie==0)
				{
					g_bind_ref_dic.erase(iprop_ref);
				}
			}
		}
		auto pnew_bind_unit= new prop_ele_bind_unit();
		pnew_bind_unit->_expression = _pnew_bind_unit->_expression;
		pnew_bind_unit->_param_list = _pnew_bind_unit->_param_list;
		_edit_new_obj = true;
		g_bind_dic.erase(_current_prop_ele);
	}
	if (_be_unsavable)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
}