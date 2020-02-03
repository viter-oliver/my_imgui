#include "primitve_edit.h"
#include <string>
#include "user_control_imgui.h"
#include "res_internal.h"
#ifdef IMGUI_WAYLAND
#include "../../deps/glad/glad.h"
#else
#include <GL/gl3w.h> 
#endif
#include <sstream>
#include "af_type.h"
#include "common_functions.h"
void primitve_edit::draw_primitive_list()
{
	if (ImGui::Button("New primitive..."))
	{
		ImGui::OpenPopup("NewPrimitive");
	}
	if (ImGui::BeginPopupModal("NewPrimitive"))
	{
		static char prm_name_str[FILE_NAME_LEN] = "";
		static vector<GLubyte> prm_frm = { 3 };
		static GLint vetex_cnt = 1;
		ImGui::InputText("enter a name for the new primitive object", prm_name_str, FILE_NAME_LEN);
		ImGui::InputInt("enter a number for the count of vertex of new primitive object", &vetex_cnt);
		ImGui::Text("Element format:");
		string str_minus("-##");
		auto& fmts = prm_frm;
		auto fsz = fmts.size();
		string str_plus("+##");
		GLuint stride = 0;
		for (int ix = 0; ix < fsz; ix++)
		{
			stride += fmts[ix];
			ImGui::Text("%d", fmts[ix]);
			if (fmts[ix] > 1)
			{
				ImGui::SameLine();
				str_minus += "1";
				if (ImGui::Button(str_minus.c_str()))
				{
					fmts[ix]--;
				}
			}
			if (fmts[ix] < 10)
			{
				ImGui::SameLine();
				str_plus += "1";
				if (ImGui::Button(str_plus.c_str()))
				{
					fmts[ix]++;
				}
			}
		}

		if (fsz < 10)
		{
			if (ImGui::Button("Add..."))
			{
				fmts.emplace_back();
				fmts.back() = 1;
			}
		}
		if (fsz > 1)
		{
			if (fsz < 10)
			{
				ImGui::SameLine();
			}
			if (ImGui::Button("Del..."))
			{
				fmts.resize(fsz - 1);
			}
		}
		if (ImGui::Button("New"))
		{
			string prm_name(prm_name_str);
			string prm_kname = find_a_key_from_mp(g_primitive_list, prm_name);
			prm_kname = find_a_key_from_mp(g_mfiles_list, prm_kname);
			auto pmtv = make_shared<primitive_object>();
			pmtv->set_ele_format(prm_frm);
			auto stride = pmtv->get_stride();
			auto vlen = stride* vetex_cnt;
			GLfloat* pvertex = new GLfloat[vlen];
			memset(pvertex, 0, vlen*sizeof GLfloat);
			pmtv->load_vertex_data(pvertex, vlen);
			auto buff_len = 4 + vlen*sizeof(GLfloat);
			ps_af_file ps_file = make_shared<af_file>(buff_len);
			char* phead = (char*)ps_file->_pbin;
			GLuint* phead_len = (GLuint*)phead;
			*phead_len = buff_len - 4;
			phead += 4;
			memcpy(phead, pvertex, *phead_len);
			g_mfiles_list[prm_kname] = ps_file;
			pmtv->_ps_file = ps_file;
			save_ojfile_to_file(prm_kname);
			delete[] pvertex;
			g_primitive_list[prm_kname] = pmtv;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = icn_primitive;
	if (IconTreeNode(icon_str,"primitive objects",node_flag))
	{
		for (auto& iprm:g_primitive_list)
		{
			auto& key_name = iprm.first;
			auto& prm = iprm.second;
			node_flag = ImGuiTreeNodeFlags_Leaf;
			if (prm->_sel)
			{
				node_flag |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, key_name.c_str(), node_flag);
			if (ImGui::IsItemClicked() && _pmobj!=prm)
			{
				if (_pmobj)
				{
					_pmobj->_sel = false;
				}
				_pmobj = prm;
				_key_name = key_name;
				_pmobj->_sel = true;
				auto& ps_file = _pmobj->_ps_file;
				char* phead = (char*)ps_file->_pbin;
				//GLuint* phead_buff_len = (GLuint*)phead;
				phead += 4;
				_pvertex = (float*)phead;
				//_vertex_buff.resize(_pmobj->_vertex_buf_len);
				//memcpy(&_vertex_buff[0], phead, *phead_buff_len);

			}
			
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	if (ImGui::BeginPopupContextWindow())
	{
		
			
		if (_pmobj&&ImGui::MenuItem("delete", NULL, false, _pmobj.use_count() == 2))
		{
			auto& item_del = g_primitive_list.find(_key_name);
			g_primitive_list.erase(item_del);
			_pmobj = nullptr;
			_key_name = "";
		}
		ImGui::EndPopup();
	}
	
}
void primitve_edit::clear_states()
{
     _pmobj = nullptr;
     _key_name = "";
}
void primitve_edit::draw_primitive_item_property()
{
	if (_pmobj)
	{
		ImGui::Text("Vertex buffer length:%d ,element buffer length:%d", _pmobj->_vertex_buf_len, _pmobj->_ele_buf_len);
		auto& fmts = _pmobj->_ele_format;
		static char str_numb[0xa] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
		string str_fmt;
		for (auto& fmt_u : fmts)
		{
			str_fmt += str_numb[fmt_u];
			str_fmt += ':';
		}
		str_fmt.back() = '\0';
		ImGui::Text("Element format:%s", str_fmt.c_str());
		if (ImGui::Button("Vertex Edit..."))
		{
			ImGui::OpenPopup("vertex_edit");
		}
		//if (_pmobj->_read_only)
		/**
		else
		{
			ImGui::Text("Element format:");
			string str_minus("-##");
			auto fsz = fmts.size();
			string str_plus("+##");
			GLuint stride = 0;
			for (int ix = 0; ix < fsz; ix++)
			{
				stride += fmts[ix];
				ImGui::Text("%d",fmts[ix]);
				if (fmts[ix]>1)
				{
					ImGui::SameLine();
					str_minus += "1";
					if (ImGui::Button(str_minus.c_str()))
					{
						fmts[ix]--;
					}
				}
				if (fmts[ix]<10)
				{
					ImGui::SameLine();
					str_plus += "1";
					if (ImGui::Button(str_plus.c_str()))
					{
						fmts[ix]++;
					}
				}
			}

			if (fsz < 10)
			{
				if (ImGui::Button("Add..."))
				{
					fmts.emplace_back();
					fmts.back() = 1;
				}
			}
			if (fsz>1)
			{
				if (fsz<10)
				{
					ImGui::SameLine();
				}
				if (ImGui::Button("Del..."))
				{
					fmts.resize(fsz - 1);
				}
			}
			static bool show_xy = true, show_xz = false, show_yz = false;

			if (fmts[0]>2)
			{
				if (ImGui::Checkbox("x_y", &show_xy) && show_xy)
				{
					show_xz = false;
					show_yz = false;
				}
				ImGui::SameLine();
				if (ImGui::Checkbox("x_z", &show_xz) && show_xz)
				{
					show_xy = false;
					show_yz = false;
				}
				ImGui::SameLine();
				if (ImGui::Checkbox("y_z", &show_yz) && show_yz)
				{
					show_xy = false;
					show_xz = false;
				}

			}
			ImGui::SliderFloat("Width of canvas", &canvas_w, 100., 900.);
			//ImGui::SameLine();
			ImGui::SliderFloat("Height of canvas", &canvas_h, 100., 900.);
			if (ImGui::Button("Save..."))
			{
				auto& ps_file = _pmobj->_ps_file;
				char* phead = (char*)ps_file->_pbin;
				GLuint* phead_buff_len = (GLuint*)phead;
				phead += 4;
				memcpy(phead, &_vertex_buff[0], *phead_buff_len);
				glBindBuffer(GL_ARRAY_BUFFER, _pmobj->_vbo);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*_pmobj->_vertex_buf_len, phead, GL_DYNAMIC_DRAW);
			}
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
			ImVec2 chd_wd_sz(canvas_w, canvas_h);

			ImGui::BeginChild("canvas", chd_wd_sz, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
			ImVec2 origin = ImGui::GetCursorScreenPos()+ImVec2(canvas_w/2,canvas_h/2);
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
			draw_list->AddLine(origin-ImVec2(0,canvas_h/2), origin + ImVec2(0, canvas_h/2), GRID_COLOR);
			draw_list->AddLine(origin-ImVec2(canvas_w/2,0), origin + ImVec2(canvas_w/2, 0), GRID_COLOR);
			//auto pt_cnt = _pmobj->_vertex_buf_len / stride;
			const float pt_radius = 4.;
			ImU32 pt_col = IM_COL32(200, 150, 150, 250);
			for (int ix = 0; ix < _pmobj->_vertex_buf_len; ix += stride)
			{
				float* pPosx;
				float* pPosy;
				if (show_xy)
				{
					pPosx= &_vertex_buff[ix];
					pPosy = &_vertex_buff[ix+1];
				}
				else
				if (show_xz)
				{
					pPosx = &_vertex_buff[ix];
					pPosy = &_vertex_buff[ix + 2];
				}
				else
				{
					pPosx = &_vertex_buff[ix+1];
					pPosy = &_vertex_buff[ix + 2];
				}
				ImVec2 potrkPt = origin+ImVec2(*pPosx, *pPosy)*ImVec2(canvas_w / 2, canvas_h / 2);
				draw_list->AddCircleFilled(potrkPt, pt_radius,pt_col);
				ImVec2 btn_pos = potrkPt - ImVec2(4., 4.);
				ImGui::SetCursorScreenPos(btn_pos);
				stringstream stm_tmp;
				stm_tmp << "in" << ix;
				string btn_cap=stm_tmp.str();
				ImGui::InvisibleButton(btn_cap.c_str(), ImVec2(8, 8));
				if (ImGui::IsItemActive()&&ImGui::IsMouseDragging(0))
				{
					auto delta = ImGui::GetIO().MouseDelta*ImVec2(2 / canvas_w, 2 / canvas_h);
					*pPosx = *pPosx + delta.x;
					*pPosy = *pPosy + delta.y;
				}
			}

			ImGui::EndChild();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		}*/
		if (ImGui::BeginPopupModal("vertex_edit"))
		{
			auto& vlen=_pmobj->_vertex_buf_len;
			GLubyte stride = _pmobj->get_stride();
			GLuint vcnt = vlen / stride;
			float* pvt = _pvertex;
               if( ImGui::Button( "Save" ) )
               {
                    glBindBuffer( GL_ARRAY_BUFFER, _pmobj->_vbo );
                    glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat )*_pmobj->_vertex_buf_len, _pvertex, GL_DYNAMIC_DRAW );
                    ImGui::CloseCurrentPopup();
               }
               ImGui::SameLine();
               if( ImGui::Button( ( "Cancel" ) ) )
               {
                    ImGui::CloseCurrentPopup();
               }
			stringstream stm_it;
			ImGui::Text("vertex buffer:");
			for (int ix = 0; ix < vcnt;ix++)
			{
				stm_it.str(string());
				stm_it.clear();
				stm_it << "Vertex" << ix;
				string citstr = stm_it.str();
				ImGui::InputFloatN(citstr.c_str(), pvt, stride,-1,0);
				pvt += stride;
			}
			/*
			ImGui::InputFloat("##0", pvt);
			pvt++;
			ImGui::SameLine();
			for (GLuint ix = 1; ix < vlen;ix++)
			{
				stm_it.str(string());
				stm_it.clear();
				stm_it << "##" << ix;
				string citstr = stm_it.str();
				ImGui::InputFloat(citstr.c_str(), pvt);
				pvt++;
				auto mvalue = ix%stride;
				if (mvalue)
					ImGui::SameLine();
			}*/

			ImGui::EndPopup();
		}
	}
}
