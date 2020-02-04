#include "feedback_edit.h"
#include "user_control_imgui.h"
#include "res_internal.h"

feedback_edit::feedback_edit()
{
}


feedback_edit::~feedback_edit()
{
}
 
void feedback_edit::draw_feedback_list()
{
	if (ImGui::Button("New feedback..."))
	{
		ImGui::OpenPopup("NewFeedback");
	}
	if (ImGui::BeginPopupModal("NewFeedback"))
	{
		static char mtl_name[FILE_NAME_LEN] = "";
		static char prm_name[FILE_NAME_LEN] = "";
		ImGui::InputText("key of material of new feedback", mtl_name, FILE_NAME_LEN);
		ImGui::InputText("key of primitive of new feedback", prm_name, FILE_NAME_LEN);
		if (ImGui::Button("Add"))
		{
			do 
			{
				const auto& imtl = g_material_list.find(mtl_name);
				if (imtl == g_material_list.end())
				{
					printf("invalid material name:%s\n", mtl_name);
					break;
				}
				const auto& iprm = g_primitive_list.find(prm_name);
				if (iprm == g_primitive_list.end())
				{
					printf("invalid primitive name:%s\n", prm_name);
					break;
				}
				feedback_key fkey = { mtl_name, prm_name };
				g_feedback_list[fkey] = make_shared<af_feedback>(imtl->second, iprm->second);
			} while (0);
		
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
	string icon_str = icn_nm_feedback;
	if (IconTreeNode(icon_str, "feedback list", node_flag))
	{
		for (auto& ifb : g_feedback_list)
		{
			auto& mtlkey = ifb.first._mtl_key;
			auto& prmkey = ifb.first._prm_key;
			auto key_show = prmkey+"->";
			key_show += mtlkey;
			auto& ps_fb = ifb.second;
			node_flag = ImGuiTreeNodeFlags_Leaf;
			if (ps_fb->_sel)
			{
				node_flag |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, key_show.c_str(), node_flag);
			if (ImGui::IsItemClicked() && _psfb_sel != ps_fb)
			{
				if (_psfb_sel)
				{
					_psfb_sel->_sel = false;
				}
				_psfb_sel = ps_fb;
				_psfb_sel->_sel = true;
			}

			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void feedback_edit::draw_feedback_item_property()
{
	if (_psfb_sel)
	{
		auto& vtex_len = _psfb_sel->_pprm->_vertex_buf_len;
		static bool feedback_executed = false;
		ImGui::Text("Vertex len:%d", vtex_len);
		ImGui::Checkbox("feedback", &feedback_executed);
		if (feedback_executed)
		{
			_psfb_sel->draw();
		}
		if (ImGui::Button("Show feedback"))
		{
			_psfb_sel->get_output_vertex(_output_buff);
			ImGui::OpenPopup("ShowFeedback");
		}
		if (ImGui::BeginPopupModal("ShowFeedback"))
		{
			auto stride = _psfb_sel->_pprm->get_stride();
			auto osz = _output_buff.size();
			for (int ix = 0; ix < osz;ix++)
			{
				ImGui::Text("%f", _output_buff[ix]);
				auto imd = ix%stride;
				if (imd!=(stride-1))
				{
					ImGui::SameLine();
				}
			}
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}
