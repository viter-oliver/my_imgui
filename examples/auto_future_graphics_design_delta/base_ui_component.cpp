#include "control_common_def.h"
namespace auto_future
{

	void base_ui_component::draw_peropertys()
	{
		for (auto& prop_ele:_vprop_eles)
		{
			auto& address_handl = _mcustom_var_property_handles_container.find(prop_ele->_pro_address);
			if (address_handl!=_mcustom_var_property_handles_container.end())
			{
				address_handl->second(prop_ele->_pro_address);
			}
			else
			{
				auto& prop_page = prop_ele->_pro_page;
				int idx = 0;
				int prev_memb_offset_p_tpsz = 0;
				for (auto& memb:prop_page)
				{
					auto& mtype = memb->_type;
					auto& mname = memb->_name;
					auto& mtpsz = memb->_tpsz;
					auto& moffset = memb->_offset;
					void* memb_address = 0;
					int array_cnt = 0;
					string::size_type apos = mname.find('[');
					if (apos!=string::npos)//is array
					{
						mname = mname.substr(0, apos);
						int wsz = moffset - prev_memb_offset_p_tpsz;
						array_cnt = wsz / mtpsz;
						memb_address = (char*)prop_ele->_pro_address + prev_memb_offset_p_tpsz;
						prev_memb_offset_p_tpsz = moffset;
					}
					else{
						auto eppos = mname.find('=');
						if (eppos!=string::npos){
							mname = mname.substr(0, eppos);
						}
						else{
							auto brpos = mname.find('{');
							if (brpos!=string::npos)
							{
								mname = mname.substr(0, brpos);
							}
						}
						memb_address=  (char*)prop_ele->_pro_address + moffset;
						prev_memb_offset_p_tpsz = moffset+mtpsz;
					}
					
					auto& imemb_tp_handl = _mcustom_type_property_handles_container.find(mtype);
					if (imemb_tp_handl != _mcustom_type_property_handles_container.end())
					{
						imemb_tp_handl->second(memb_address);
					}
					else
					{
						auto& imemb_var_handle = _mcustom_member_property_handles_container.find(st_member_key(prop_ele->_pro_address,idx));
						if (imemb_var_handle != _mcustom_member_property_handles_container.end()){
							imemb_var_handle->second(memb_address);
						}
						else{
							if (array_cnt>0){
								for (int ix = 0; ix < array_cnt;++ix)
								{
									char str_index[50] = {0};
									sprintf(str_index,"[%d]", ix);
									string mname_width_index = mname + str_index;
									void* memb_index_address = (char*)memb_address + ix*mtpsz;
									if (mtype == "int"){
										ImGui::DragInt(mname_width_index.c_str(), (int*)memb_index_address, 1);
									}
									else if (mtype == "float"||mtype == "double"){
										ImGui::SliderFloat(mname_width_index.c_str(), (float*)memb_index_address, -screenw, screenw);
									}
									else if (mtype == "ImVec2"){
										ImGui::SliderFloat2(mname_width_index.c_str(), (float*)memb_address, -screenw, screenw);
									}
									else if (mtype == "ImVec3") {
										ImGui::SliderFloat3(mname_width_index.c_str(), (float*)memb_address, -screenw, screenw);
									}
									else if (mtype == "ImVec4") {
										ImGui::SliderFloat4(mname_width_index.c_str(), (float*)memb_address, -screenw, screenw);
									}
									else if (mtype == "bool"){
										ImGui::Checkbox(mname_width_index.c_str(), (bool*)memb_address);
									}
									else{
										printf("unknown member type!:%s\n", mtype.c_str());
									}
								}
							}
							else{
								if (mtype == "int"){
									ImGui::DragInt(mname.c_str(), (int*)memb_address, 1);
								}
								else if (mtype == "float"||mtype == "double"){
									ImGui::SliderFloat(mname.c_str(), (float*)memb_address, -screenw, screenw);
								}
								else if (mtype=="ImVec2"){
									ImGui::SliderFloat2(mname.c_str(), (float*)memb_address, -screenw, screenw);
								}
								else if (mtype=="ImVec3") {
									ImGui::SliderFloat3(mname.c_str(), (float*)memb_address, -screenw, screenw);
								}
								else if (mtype=="ImVec4") {
									ImGui::SliderFloat4(mname.c_str(), (float*)memb_address, -screenw, screenw);
								}
								else if (mtype == "bool"){
									ImGui::Checkbox(mname.c_str(), (bool*)memb_address);
								}else{
									printf("unknown member type!:%s\n", mtype.c_str());
								}						
							}

						}

					}
					idx++;
				}
				ImGui::Spacing();
			}
		}
	}
	int base_ui_component::collect_property_range(vproperty_list& vplist)
	{
		int len = 0;
		for (auto& prop_ele : _vprop_eles)
		{
			len += prop_ele->_pro_sz;
			vplist.emplace_back(prop_ele->_pro_address, prop_ele->_pro_sz);
		}
		return len;
	}
}