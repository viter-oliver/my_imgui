#include "state_manager_edit.h"
#include "res_internal.h"
#include "user_control_imgui.h"
#include "common_functions.h"
#include "ft_base.h"
#include "res_internal.h"
#include <sstream>
#include <algorithm>
void state_manager_edit::set_dragging(bool be_dragging, base_ui_component* pobj, uint16_t page_idx, uint16_t off_idx)
{
	if (!be_dragging&&_hit_sm_window)
	{
		auto& pplist=_psel->_prop_list;
		prop_ele_position new_prp_pos = { pobj, _pgidx, _fdidx };
		auto ifd = find(pplist.begin(), pplist.end(), new_prp_pos);
		if (ifd == pplist.end())
		{
			pplist.emplace_back();
			auto pm_idx = pplist.size() - 1;
			auto& prop_ele_p = pplist[pm_idx];
			prop_ele_p._pobj = pobj;
			prop_ele_p._page_index = _pgidx;
			prop_ele_p._field_index = _fdidx;
		}

		_hit_sm_window = false;
	}
	if (be_dragging)
	{
		_pgidx = page_idx;
		_fdidx = off_idx;
	}
	_be_draging = be_dragging;
}

//#include <algorithm>

//class trans_edit
//{
//public:
//	trans_edit()
//	{
//		ImGui::BeginPopupModal("trans edit");
//	}
//	~trans_edit()
//	{
//		ImGui::EndPopup();
//	}
//
//};
//

void state_manager_edit::view_state_managers()
{
	static char sm_name_str[FILE_NAME_LEN] = "";
	if (ImGui::Button("New state manager..."))
	{
		sm_name_str[0] = '\0';
		ImGui::OpenPopup("new state manager");
	}
	if (ImGui::BeginPopupModal("new state manager"))
	{
		ImGui::InputText("enter a name for the new state manger", sm_name_str, FILE_NAME_LEN);
		if (ImGui::Button("Add"))
		{
			string str_sm_nm = sm_name_str;
			string str_sm_key = find_a_key_from_mp(g_mstate_manager, str_sm_nm);
			auto ps_state_mg = make_shared<af_state_manager>();
			ps_state_mg->_any_to_any = { 0, 250, EaseLinear };
			g_mstate_manager[sm_name_str] = ps_state_mg;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	string icon_str = icn_nm_state_manager;
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	if (IconTreeNode(icon_str, "State manager list", node_flags_root))
	{
		for (auto& ist_mg:g_mstate_manager)
		{
			auto& keyname = ist_mg.first;
			auto& pst_mg = ist_mg.second;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (pst_mg->_sel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, keyname.c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				if (_psel)
				{
					_psel->_sel = false;
				}
				pst_mg->_sel = true;
				_psel = pst_mg;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	if (_psel&&ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("delete",NULL,false))
		{
			auto ifd = find_if(g_mstate_manager.begin(), g_mstate_manager.end(), [&](pair<string, ps_state_manager> tar){return _psel == tar.second; });
			if (ifd!=g_mstate_manager.end())
			{
				g_mstate_manager.erase(ifd);
				_psel = nullptr;
			}
		}
		ImGui::EndPopup();
	}
}

void state_manager_edit::view_state_manager_item_property()
{
	if (!_psel)
	{
		return;
	}
	_hit_sm_window = _be_draging&&ImGui::IsMouseHoveringWindow();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	auto& pplist = _psel->_prop_list;
	int idx = 0;
	for (auto itp = pplist.begin(); itp != pplist.end();idx++)
	{
		auto& prop_ele_p = *itp;
		string prop_ele_show;
		get_uic_path(prop_ele_p._pobj, prop_ele_show);
		auto& pgidx = prop_ele_p._page_index;
		auto& fidx = prop_ele_p._field_index;
		field_ele &fel = prop_ele_p._pobj->get_filed_ele(pgidx, fidx);
		prop_ele_show += "::";
		prop_ele_show += fel._name;
		ImGui::Text(prop_ele_show.c_str());
		ImGui::SameLine();
		stringstream stream_delbtn;
		stream_delbtn << "X##"<<idx;
		string str_delbtn = stream_delbtn.str();
		if (ImGui::Button(str_delbtn.c_str()))
		{
			itp = pplist.erase(itp);
		}
		else
		{
			itp++;
		}
	}
	auto& pp_value_list = _psel->_prop_value_list;
	auto& cur_state = _psel->_state_idx;
	auto& mtrans = _psel->_mtrans;
	auto state_cnt = pp_value_list.size();
	ImGui::BeginChild("stata_manager",ImVec2(0,0),true);
	ImGui::Columns(2);
	stringstream stm_sel;
	string str_sel;
	idx = 0;
	for (auto ipv = pp_value_list.begin(); ipv != pp_value_list.end();idx++ )
	{
		stm_sel.str(string());
		stm_sel.clear();
		stm_sel << "State" << idx;
		str_sel = stm_sel.str();
		if (ImGui::Selectable(str_sel.c_str(),cur_state==idx,0,ImVec2(100,0)))
		{
			cur_state = idx;
			auto& cur_pp_value = *ipv;
			auto cur_pp_bk_cnt = cur_pp_value.size();
			auto ppcnt = pplist.size();
			if (ppcnt==cur_pp_bk_cnt)
			{
				for (int iix = 0; iix < ppcnt;iix++)
				{
					auto& p_pos = pplist[iix];
					auto& pp_block = cur_pp_value[iix];
					auto& pgidx = p_pos._page_index;
					auto& fidx = p_pos._field_index;
					auto& fel = p_pos._pobj->get_filed_ele(pgidx, fidx);
					if (pp_block.size()==fel._tpsz)
					{
						char* ppt_addr =  fel._address;
						memcpy(ppt_addr, &pp_block[0], fel._tpsz);
					}
				}
			}

		}
		stm_sel.str(string());
		stm_sel.clear();
		//stm_sel << "\xe2\x86\x91";
		stm_sel << "^|^##" << idx;
		str_sel = stm_sel.str();
		ImGui::SameLine(120);
		if (ImGui::Button(str_sel.c_str()))
		{
			auto ppcnt = pplist.size();//属性个数
			for (auto& pvale:pp_value_list)//每个状态都应该有属性个数的数据块
			{
				pvale.resize(ppcnt);
			}
			auto& cur_pp_value = *ipv;
			int iidx = 0;
			for (auto&p_pos:pplist)
			{
				auto& pgidx = p_pos._page_index;
				auto& fidx = p_pos._field_index;
				field_ele &fel = p_pos._pobj->get_filed_ele(pgidx, fidx);
				auto& pp_block = cur_pp_value[iidx];
				char* ppt_addr = fel._address;
				int tp_sz = fel._tpsz;
				pp_block.resize(tp_sz);
				memcpy(&pp_block[0], ppt_addr, tp_sz);
			}
		}

		stm_sel.str(string());
		stm_sel.clear();
		stm_sel << "X##X"<<idx;
		str_sel = stm_sel.str();
		ImGui::SameLine();
		if (ImGui::Button(str_sel.c_str()))
		{
			ipv = pp_value_list.erase(ipv);
		}
		else
		{
			ipv++;
		}
	}
	if (pplist.size()>0&& ImGui::Button("Add new state..."))
	{
		pp_value_list.emplace_back();
	}
	ImGui::NextColumn();
	if (state_cnt>=2)
	{
		if (ImGui::Button("any<->any"))
		{
			ImGui::OpenPopup("trans_edit");
			_trans_tar = en_trans_any;
			_trans_edit = _psel->_any_to_any;
		}
		for (auto itrans = mtrans.begin(); itrans != mtrans.end();)
		{
			stm_sel.str(string());
			stm_sel.clear();
			stm_sel << "state" << itrans->first._from << "<->state" << itrans->first._to;
			str_sel = stm_sel.str();
			if (ImGui::Button(str_sel.c_str()))
			{
				ImGui::OpenPopup("trans_edit");
				_trans_tar = en_trans_existed;
				_trans_edit = *itrans->second;
				_cur_trans_edit_key = itrans->first;
			}
			ImGui::SameLine();
			string str_play = ">>##" + str_sel;
			if (ImGui::Button(str_play.c_str()))//play the trans
			{
				_cur_trans_play_key = itrans->first;
				_be_playing = true;
				_cur_trans_play_start = steady_clock::now();
			}

			str_sel = "X##" + str_sel;
			ImGui::SameLine();
			if (ImGui::Button(str_sel.c_str()))
			{
				itrans = mtrans.erase(itrans);
			}
			else
			{
				itrans++;
			}
		}
		auto ntrans = mtrans.size();
		auto mxn_trans = state_cnt*(state_cnt - 1);
		if (ntrans<mxn_trans&&ImGui::Button("Add new trans"))
		{
			ImGui::OpenPopup("trans_edit");
			_trans_tar = en_trans_new;
			_trans_edit = {0,250,EaseLinear};
			_cur_trans_edit_key = { 0, 0 };
		}
	}
	if (ImGui::BeginPopupModal("trans_edit"))
	{
		ImGui::Text("------------------------------------------------------------------------------------");
		switch (_trans_tar)
		{
		case en_trans_any:
			ImGui::Text("any<->any");
			break;
		case en_trans_existed:
			ImGui::Text("from:state%d", _cur_trans_edit_key._from);
			ImGui::Text("to:state%d", _cur_trans_edit_key._to);
			break;
		case en_trans_new:
			{
				string str_comb_sel;
				for (idx = 0; idx < state_cnt; idx++)
				{
					stm_sel.str(string());
					stm_sel.clear();
					stm_sel << "state" << idx << '\0';
					str_sel = stm_sel.str();
					str_comb_sel += str_sel;
				}
				ImGui::Combo("from", &_cur_trans_edit_key._from, str_comb_sel.c_str());
				ImGui::Combo("to", &_cur_trans_edit_key._to, str_comb_sel.c_str());
			}
			break;
		}

	
		ImGui::SliderInt("start time", &_trans_edit._start_time, 0, 5000);
		ImGui::SliderInt("duration", &_trans_edit._duration, 0, 5000);
		//ImGui::Columns(6,false);
		for (idx = EaseLinear; idx < EaseFuncsCount; idx++)
		{
			auto itxt = g_mtxt_intl.find(easing_func_name[idx]);
			auto& txt_unit = itxt->second;
			ImVec2 uv0 = ImVec2(txt_unit._x0 / g_txt_width_intl, txt_unit._y0 / g_txt_height_intl);
			ImVec2 uv1 = ImVec2((txt_unit._x1) / g_txt_width_intl, (txt_unit._y1) / g_txt_height_intl);
			ImVec2 img_sz(88, 81);
			ImVec4 bk_col;
			bk_col = idx == _trans_edit._easing_func ? ImVec4(0,0,0,0) :ImVec4(0.3, 0.3, 0.3, 1);
			ImGui::PushID(idx);	
			if (ImGui::ImageButton((ImTextureID)g_txt_id_intl, img_sz, uv0, uv1, 0,1, bk_col))
			{
				_trans_edit._easing_func = idx;
			}
			ImGui::PopID();
			ImGui::Text(easing_func_name[idx]);
			if (idx==EaseLinear)
			{
				ImGui::Columns(6, false);
			}
			if (idx>EaseLinear)
			{
				ImGui::NextColumn();
			}
		}
		ImGui::Columns(1);
		bool be_valid = true;
		if (_trans_tar==en_trans_new)
		{
			be_valid = _cur_trans_edit_key.valid();
			if (be_valid)
			{
				auto itrans = mtrans.find(_cur_trans_edit_key);
				if (itrans!=mtrans.end())
				{
					be_valid = false;
				}
			}
		}
		if (!be_valid)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button("Ok"))
		{
			switch (_trans_tar)
			{
			case en_trans_any:
				_psel->_any_to_any = _trans_edit;
				break;
			case en_trans_existed:
				{
					auto itrans = mtrans.find(_cur_trans_edit_key);
					*itrans->second = _trans_edit;
				}
				break;
			case en_trans_new:
 				{ 
					auto ps_trans = make_shared<state_transition>();
					*ps_trans = _trans_edit;
					mtrans[_cur_trans_edit_key] = ps_trans;
				}
			    break;
			}
			ImGui::CloseCurrentPopup();
		}
		if (!be_valid)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		ImGui::SameLine(100.f);
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		
		ImGui::EndPopup();
	}
	ImGui::EndChild();
	
}

void state_manager_edit::trans_play()
{
	if (_psel)
	{
		auto cur_clk = steady_clock::now();
		auto dur_mills = duration_cast<milliseconds>(cur_clk - _cur_trans_play_start);
		auto& icur_trans = _psel->_mtrans.find(_cur_trans_play_key);
		auto& cur_trans = *icur_trans->second;
		auto delta_tm = dur_mills.count() - cur_trans._start_time;
		if (delta_tm>0)
		{
			double tm_pt_mill = (double)delta_tm / cur_trans._duration;
			auto& easing_fun = easingFun[cur_trans._easing_func];
			double value_scale = easing_fun(tm_pt_mill);
			if (delta_tm < cur_trans._duration)
			{
				_psel->_mstate = en_state_moving;
			}
			else
			{
				_be_playing = false;
				_psel->_mstate = en_state_pause;
				value_scale = 1.0;
				_psel->_state_idx = _cur_trans_play_key._to;
			}
			auto& prop_list = _psel->_prop_list;
			auto& pp_vl_lst_from = _psel->_prop_value_list[_cur_trans_play_key._from];
			auto& pp_vl_lst_to = _psel->_prop_value_list[_cur_trans_play_key._to];
			int idx = 0;
			for (auto& prop:prop_list)
			{
				auto& pgidx = prop._page_index;
				auto& fdidx = prop._field_index;
				auto& pobj = prop._pobj;
				auto& fel = pobj->get_filed_ele(pgidx, fdidx);
				char* ppt_addr =fel._address;
				auto& ppt_blk_from = pp_vl_lst_from[idx];
				auto& ppt_blk_to = pp_vl_lst_to[idx];
				if (fel._type=="int")
				{
					int* pifrom = (int*)&ppt_blk_from[0];
					int* pito = (int*)&ppt_blk_to[0];
					int* piprop = (int*)ppt_addr;
					int ivalue_span = *pito - *pifrom;
					int idelta_value = ivalue_span*value_scale;
					*piprop = *pifrom + idelta_value;
				}
				else
				if (fel._type=="float")
				{
					float* pffrom = (float*)&ppt_blk_from[0];
					float* pfto = (float*)&ppt_blk_to[0];
					float* pfprop = (float*)ppt_addr;
					float fvalue_span = *pfto - *pffrom;
					float fdelta_value = fvalue_span*value_scale;
					*pfprop = *pffrom + fdelta_value;
				}
				else
				if (fel._type=="double")
				{
					double* pdfrom = (double*)&ppt_blk_from[0];
					double* pdto = (double*)&ppt_blk_to[0];
					double* pdprop = (double*)ppt_addr;
					double dvalue_span = *pdto - *pdfrom;
					double ddelta_value = dvalue_span*value_scale;
					*pdprop = *pdfrom + ddelta_value;
				}
				else
				if (fel._type=="bool")
				{
				}

				idx++;
			}
			

		}
		
	}
}
