#include "state_manager_edit.h"
#include "playlist_group_edit.h"
#include "res_internal.h"
#include "user_control_imgui.h"
#include "common_functions.h"
#include "ft_base.h"
#include "res_internal.h"
#include <sstream>
extern playlist_group_edit g_playlist_group_edit;
//#include <algorithm>
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
     ImGui::BeginChild( "state manager list area" );
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
				_stm_key_name = keyname;
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
     ImGui::EndChild();
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
	ImGui::Columns(3);
	ImGui::SetColumnWidth(0, 200);
	ImGui::SetColumnWidth(1, 300);
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
				auto blk_cnt = pvale.size();
				if (ppcnt!=blk_cnt)
				{
					pvale.resize(ppcnt);
				}
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
				iidx++;
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
		auto ntrans = mtrans.size();
		auto mxn_trans = state_cnt*(state_cnt - 1);
		if (ntrans < mxn_trans&&ImGui::Button("Add new trans"))
		{
			ImGui::OpenPopup("trans_edit");
			_trans_tar = en_trans_new;
			_trans_edit = { 0, 250, EaseLinear };
			_cur_trans_edit_key = { 0, 0 };
		}
		if (ImGui::Button("any<->any"))
		{
			ImGui::OpenPopup("trans_edit");
			_trans_tar = en_trans_any;
			_trans_edit = _psel->_any_to_any;
		}
		auto& playlist_list = _psel->_playlist_list;
		auto& cur_playlist_id = _psel->_cur_playlist_id;
          auto pl_sz = playlist_list.size();
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
				//g_state_trans_player.play_state_trans(_psel, itrans->first._from,itrans->first._to);
                    play_tran( _stm_key_name, itrans->first._from, itrans->first._to );
			}
               if (pl_sz>0)
               {
                    assert( cur_playlist_id < pl_sz );
                    auto& itrans_key = itrans->first;
                    auto& cur_pl = playlist_list[ cur_playlist_id ];
                    bool finded = false;
                    for (auto& itran:cur_pl)
                    {
                         if (itran==itrans_key)
                         {
                              finded = true;
                              break;
                         }
                    }
                    if (!finded)
                    {
		               string str_add = "+>>>##" + str_sel;
			               ImGui::SameLine();
                         if (ImGui::Button(str_add.c_str()))
                         {
                              cur_pl.emplace_back( itrans_key );
                         }
                    }
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

		ImGui::NextColumn();
		if (ImGui::Button("Add new play list"))
		{
			playlist_list.emplace_back();
		}
		idx = 0;
          for( auto ipl = playlist_list.begin(); ipl != playlist_list.end();idx++ )
		{
               auto&playlist = *ipl;
               auto plsz = playlist.size();
			stm_sel.str(string());
			stm_sel.clear();
			stm_sel << "playlist" << idx;
			str_sel = stm_sel.str();
			if (ImGui::Selectable(str_sel.c_str(), cur_playlist_id==idx,0,ImVec2(100,0)))
			{
				cur_playlist_id = idx;
			}
               if (plsz>0)
               {
				ImGui::SameLine();
                    str_sel = ">>##" + stm_sel.str();
                    if( ImGui::Button( str_sel.c_str() ))
                    {
                         play_tran_playlist( _stm_key_name, idx );
                    }
				if (ImGui::IsItemActive())
				{
					// Draw a line between the button and the mouse cursor
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					draw_list->PushClipRectFullScreen();
					ImGuiIO& io = ImGui::GetIO();
					draw_list->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
					draw_list->PopClipRect();
					g_playlist_group_edit.set_dragging(true, _stm_key_name, idx);
				}
				if (ImGui::IsMouseReleased(0))
				{
					g_playlist_group_edit.set_dragging(false, _stm_key_name, idx);
				}
                    str_sel = "##" + stm_sel.str();
                    ImGui::BeginChild( str_sel.c_str(), ImVec2( 0, 40 ), true );
                    ImGui::Columns( plsz );
                    int ixx = 0;
                    for( auto ipl = playlist.begin(); ipl != playlist.end(); )
				{

					ImGui::Text( "%d<->%d", ipl->_from, ipl->_to );
					stm_sel.str(string());
					stm_sel.clear();
					stm_sel << "trans" << idx<<"+"<<ixx;
					str_sel = "<-##"+stm_sel.str();
					if (ixx>0)
					{
                              ImGui::SameLine();
                              if(ImGui::Button(str_sel.c_str()))
							swap( playlist[ ixx ], playlist[ ixx - 1 ] );
					}
					str_sel = "->##" + stm_sel.str();
					if (ixx<plsz-1)
					{
                              ImGui::SameLine();
                              if(ImGui::Button(str_sel.c_str()))
						     swap( playlist[ ixx ], playlist[ ixx + 1 ] );
					}
					str_sel = "X##X" + stm_sel.str();
                         ImGui::SameLine();
					if (ImGui::Button(str_sel.c_str()))
					{
						ipl = playlist.erase( ipl );
						plsz = playlist.size();
					}
					else
					{
						ixx++;
						ipl++;
					}
					if( ixx<plsz )
					{
						ImGui::NextColumn();
					}
				}
                    ImGui::EndChild();
               }
               stm_sel.str( string() );
               stm_sel.clear();
               stm_sel << "X##" << idx;
               str_sel = stm_sel.str();
               if( ImGui::Button( str_sel.c_str() ) )
               {
                    ipl = playlist_list.erase( ipl );
                    cur_playlist_id = 0;
               }
               else
               {
                    ipl++;
               }
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
