#include "control_common_def.h"
#include "res_output.h"
#define INT_VALUE 45
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

namespace auto_future
{
	static map<string, value_range> s_rg_tips;
	const int init_base_value_ranges()
	{
		s_rg_tips["shd"] = value_range(-100.f, 100.f);
		s_rg_tips["stn"] = value_range(-10.f, 10.f);
		s_rg_tips["srd"] = value_range(-360.f, 360.f);
		s_rg_tips["uhd"] = value_range(0.f, 100.f);
		s_rg_tips["utn"] = value_range(0.f, 10.f);
		s_rg_tips["urd"] = value_range(0.f, 360.f);
		s_rg_tips["nml"] = value_range(0.f, 1.f);
		return 1;
	}
	static const int s_triger = init_base_value_ranges();
	void ShowHelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
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
					auto mtype = memb->_type;
					auto mname = memb->_name;
					auto mtpsz = memb->_tpsz;
					auto moffset = memb->_offset;
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
					string rg = mname.substr(mname.length() - 3, 3);
					auto& irg = s_rg_tips.find(rg);
					value_range _vrange(0.f, screenw);
					if (irg!=s_rg_tips.end())
					{
						_vrange = irg->second;
					}
					else
					{
						auto& irg_reg=_mcustom_member_value_ranges_container.find(st_member_key(prop_ele->_pro_address, idx));
						if (irg_reg != _mcustom_member_value_ranges_container.end())
						{
							_vrange = irg_reg->second;
						}
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
								if (mtype == "char")
								{
									if (mname=="_name"){
										if (ImGui::InputText("object name", _in_p._name, name_len))
										{
											auto pparent = get_parent();
											if (pparent)
											{
												string nname = pparent->try_create_a_child_name(_in_p._name, this);
												set_name(nname);
											}
										}
									}
									else{
										ImGui::InputText(mname.c_str(), (char*)memb_address, array_cnt);
									}
								}
								else
								{
									for (int ix = 0; ix < array_cnt;++ix)
									{
										char str_index[50] = {0};
										sprintf(str_index,"[%d]", ix);
										string mname_width_index = mname + str_index;
										void* memb_index_address = (char*)memb_address + ix*mtpsz;
										if (mtype == "int"){
											ImGui::SliderInt(mname_width_index.c_str(), (int*)memb_index_address, _vrange._min._i, _vrange._max._i);
										}
										else if (mtype == "float"||mtype == "double"){
											ImGui::SliderFloat(mname_width_index.c_str(), (float*)memb_index_address, _vrange._min._f, _vrange._max._f);
										}
										else if (mtype == "ImVec2"){
											ImGui::SliderFloat2(mname_width_index.c_str(), (float*)memb_address, _vrange._min._f, _vrange._max._f);
										}
										else if (mtype == "ImVec3") {
											if (rg=="clr")
											{
												ImGui::ColorEdit3(mname_width_index.c_str(), (float*)memb_address, ImGuiColorEditFlags_RGB);
											}
											else
											{
												ImGui::SliderFloat3(mname_width_index.c_str(), (float*)memb_address, 0, screenw);
											}
										}
										else if (mtype == "ImVec4") {
											if (rg=="clr")
											{
												ImGui::ColorEdit4(mname_width_index.c_str(), (float*)memb_address);
											}
											else
											{
												ImGui::SliderFloat4(mname_width_index.c_str(), (float*)memb_address, _vrange._min._f, _vrange._max._f);
											}
										}
										else if (mtype == "bool"){
											ImGui::Checkbox(mname_width_index.c_str(), (bool*)memb_address);
										}
										else{
											printf("unknown member type!:%s\n", mtype.c_str());
										}
									}
								}
							}
							else{
								if (mtype == "int"){
									if (rg=="txt")// atexture
									{
										auto& res_coors = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
										int isize = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates.size();
										ImGui::Combo(mname.c_str(), (int*)memb_address, &get_texture_item, &g_vres_texture_list[g_cur_texture_id_index], isize);
										ImGui::SameLine(); ShowHelpMarker("select a image from image resource!\n");
										int txt_idx = *(int*)memb_address;
										float reswidth = res_coors[txt_idx].owidth();
										float resheight = res_coors[txt_idx].oheight();
										ImGui::Text("original size:%f,%f", reswidth, resheight);
										ImGui::Spacing();
										if (reswidth > 0)
										{
											float draw_height = imge_edit_view_width*resheight / reswidth;
											ImVec2 draw_size(imge_edit_view_width, draw_height);
											int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id;
											float wtexture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
											float wtexture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;

											ImVec2 uv0(res_coors[txt_idx]._x0 / wtexture_width, res_coors[txt_idx]._y0 / wtexture_height);
											ImVec2 uv1(res_coors[txt_idx]._x1 / wtexture_width, res_coors[txt_idx]._y1 / wtexture_height);
											ImGui::Image((ImTextureID)texture_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
										}
									}
									else{
										ImGui::SliderInt(mname.c_str(), (int*)memb_address, _vrange._min._i, _vrange._max._i);
									}
								}
								else if (mtype == "float"||mtype == "double"){
									ImGui::SliderFloat(mname.c_str(), (float*)memb_address, _vrange._min._f, _vrange._max._f);
								}
								else if (mtype=="ImVec2"){
									ImGui::SliderFloat2(mname.c_str(), (float*)memb_address, _vrange._min._f, _vrange._max._f);
								}
								else if (mtype=="ImVec3") {
									if (rg=="clr")
									{
										ImGui::ColorEdit3(mname.c_str(), (float*)memb_address, ImGuiColorEditFlags_RGB);
									}
									else
									{
										ImGui::SliderFloat3(mname.c_str(), (float*)memb_address, _vrange._min._f, _vrange._max._f);
									}
								}
								else if (mtype=="ImVec4") {
									if (rg=="clr")
									{
										ImGui::ColorEdit4(mname.c_str(), (float*)memb_address);
									}
									else{
										ImGui::SliderFloat4(mname.c_str(), (float*)memb_address, _vrange._min._f, _vrange._max._f);
									}

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
				ImGui::Separator();
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