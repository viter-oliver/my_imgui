#include "control_common_def.h"
#include "res_output.h"
#include "command_element_delta.h"
#include "factory.h"
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
					value_range _vrange(-screenw, screenw);
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
					cmd_value_block before_op_memb_value;
					before_op_memb_value.reserve(mtpsz);
					before_op_memb_value.resize(mtpsz);
					memcpy(&before_op_memb_value[0], memb_address, mtpsz);

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
							function<bool(string&, void* )> f_draw_index_prop;
							if (mtype == "char"){
								if (array_cnt > 0)
								{
									if (mname == "_name"){
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
									ImGui::SliderInt(mname.c_str(), (int*)memb_address, 0, 255);
							}
							else if (mtype == "int"){
								if (rg == "txt")// atexture
								{
									f_draw_index_prop = [&](string& str_show, void*maddress){									
										auto& res_coors = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
										int isize = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates.size();
										int imem_value = *(int*)memb_address;
										bool be_changed=ImGui::Combo(str_show.c_str(), (int*)maddress, &get_texture_item, &g_vres_texture_list[g_cur_texture_id_index], isize);

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
											int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id();
											float wtexture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
											float wtexture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;

											ImVec2 uv0(res_coors[txt_idx]._x0 / wtexture_width, res_coors[txt_idx]._y0 / wtexture_height);
											ImVec2 uv1(res_coors[txt_idx]._x1 / wtexture_width, res_coors[txt_idx]._y1 / wtexture_height);
											ImGui::Image((ImTextureID)texture_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
										}
										return be_changed;
									};
								}
								else{
									f_draw_index_prop = [&](string& str_show, void*maddress){
										return  ImGui::SliderInt(str_show.c_str(), (int*)maddress, _vrange._min._i, _vrange._max._i);
									};
								}

							}
							else if (mtype == "float" || mtype == "double"){
								f_draw_index_prop = [&](string& str_show, void*maddress){
									return  ImGui::SliderFloat(str_show.c_str(), (float*)maddress, _vrange._min._f, _vrange._max._f);
								};
							}
							else if (mtype == "af_vec2"){
								f_draw_index_prop = [&](string& str_show, void*maddress){
										return ImGui::SliderFloat2(str_show.c_str(), (float*)maddress, _vrange._min._f, _vrange._max._f);
								};
				
							}
							else if (mtype == "af_vec3") {
								f_draw_index_prop = [&](string& str_show, void*maddress){
									if (rg == "clr")
									{
										return ImGui::ColorEdit3(str_show.c_str(), (float*)maddress, ImGuiColorEditFlags_RGB);
									}
									else
									{
										return ImGui::SliderFloat3(str_show.c_str(), (float*)maddress, _vrange._min._f, _vrange._max._f);
									}
								};
										
							}
							else if (mtype == "af_vec4") {
								f_draw_index_prop = [&](string& str_show, void*maddress){
									if (rg == "clr")
									{
										return ImGui::ColorEdit4(str_show.c_str(), (float*)maddress, ImGuiColorEditFlags_RGB);
									}
									else
									{
										return ImGui::SliderFloat4(str_show.c_str(), (float*)maddress, _vrange._min._f, _vrange._max._f);
									}
								};
							}
							else if (mtype == "bool"){
								f_draw_index_prop = [&](string& str_show, void*maddress){
									return ImGui::Checkbox(str_show.c_str(), (bool*)maddress);
								};
							}
							else{
								printf("unknown member type!:%s\n", mtype.c_str());
								continue;
							}
							if (f_draw_index_prop)
							{
								if (array_cnt>0){
									for (int ix = 0; ix < array_cnt; ++ix)
									{
										char str_index[50] = { 0 };
										sprintf(str_index, "[%d]", ix);
										string mname_width_index = mname + str_index;
										void* memb_index_address = (char*)memb_address + ix*mtpsz;
										f_draw_index_prop(mname_width_index, memb_index_address);
									}
								}
								else{
									f_draw_index_prop(mname, memb_address);
								}
							}
						}

						static cmd_value_block bk_memb_value;
						static void* pmem_address = 0;;
						static bool be_operating = false;
						cmd_value_block after_op_memb_value;
						after_op_memb_value.reserve(mtpsz);
						after_op_memb_value.resize(mtpsz);
						memcpy(&after_op_memb_value[0], memb_address, mtpsz);

						ImGuiContext& g = *GImGui;
						if (g.operating_be_started)
						{
							printf("start changing value\n");
							pmem_address = memb_address;

							bk_memb_value = before_op_memb_value;
							g.operating_be_started = false;
							be_operating = true;
						}
						if (be_operating)
						{
							if (g.IO.MouseReleased[0])
							{
								g_ui_edit_command_mg.create_command(edit_commd<base_ui_component>(this, pmem_address, &before_op_memb_value[0], before_op_memb_value.size()));
								be_operating = false;
							}
						}
						else if (before_op_memb_value != after_op_memb_value)
						{
							g_ui_edit_command_mg.create_command(edit_commd<base_ui_component>(this, memb_address, &before_op_memb_value[0], before_op_memb_value.size()));

						}
		
						g.IO.InputContentChanged = false;
					}
					idx++;
				}
				ImGui::Separator();
				ImGui::Spacing();
			}
		}
	}
	void base_ui_component::init_property_from_json(Value& jvalue){

		for (auto& prop_ele : _vprop_eles)
		{
			auto& prop_page = prop_ele->_pro_page;
			int prev_memb_offset_p_tpsz = 0;
			for (auto& memb : prop_page)
			{
				auto mtype = memb->_type;
				auto mname = memb->_name;
				auto mtpsz = memb->_tpsz;
				auto moffset = memb->_offset;
				void* memb_address = 0;
				int array_cnt = 0;
				string::size_type apos = mname.find('[');
				if (apos != string::npos)//is array
				{
					mname = mname.substr(0, apos);
					int wsz = moffset - prev_memb_offset_p_tpsz;
					array_cnt = wsz / mtpsz;
					memb_address = (char*)prop_ele->_pro_address + prev_memb_offset_p_tpsz;
					prev_memb_offset_p_tpsz = moffset;
				}
				else{
					auto eppos = mname.find('=');
					if (eppos != string::npos){
						mname = mname.substr(0, eppos);
					}
					else{
						auto brpos = mname.find('{');
						if (brpos != string::npos)
						{
							mname = mname.substr(0, brpos);
						}
					}
					memb_address = (char*)prop_ele->_pro_address + moffset;
					prev_memb_offset_p_tpsz = moffset + mtpsz;
				}
				if (array_cnt > 0){
					if (mtype == "char")
					{
						Value vbytes = jvalue[mname];//must be string
						strcpy((char*)memb_address, vbytes.asCString());
					}
					else
					{
						Value marray=jvalue[mname];
						function<void(void*,Value&)> f_assingn_json_to_memb;
						if (mtype == "int"){
							f_assingn_json_to_memb = [&](void* membaddr,Value& vele)
							{
								*(int*)membaddr=vele.asInt();
							};
						}
						else if (mtype == "float" || mtype == "double"){
							f_assingn_json_to_memb = [&](void* membaddr, Value& vele)
							{
								*(float*)membaddr=vele.asDouble();
							};
						}
						else if (mtype == "af_vec2"){
							f_assingn_json_to_memb = [&](void* membaddr, Value& vele)
							{
								 *(float*)membaddr=vele["x"].asDouble();
								 *((float*)membaddr + 1)=vele["y"].asDouble();
							};
						}
						else if (mtype == "af_vec3"){
							f_assingn_json_to_memb = [&marray](void* membaddr, Value& vele)
							{
								*(float*)membaddr=vele["x"].asDouble();
								*((float*)membaddr + 1)=vele["y"].asDouble();
								*((float*)membaddr + 2)=vele["z"].asDouble();
							};
						}
						else if (mtype == "af_vec4"){
							f_assingn_json_to_memb = [&marray](void* membaddr, Value& vele)
							{
								*(float*)membaddr=vele["x"].asDouble();
								*((float*)membaddr + 1)=vele["y"].asDouble();
								*((float*)membaddr + 2)=vele["z"].asDouble();
								*((float*)membaddr + 3) = vele["w"].asDouble();					
							};
						}
						else if (mtype == "bool"){
							f_assingn_json_to_memb = [&marray](void* membaddr, Value& vele)
							{
								*(bool*)membaddr=vele.asBool();
							};
						}
						else {
							f_assingn_json_to_memb = [&](void* membaddr, Value& vele)
							{
								strcpy((char*)membaddr, vele.asCString());
							};
						}
						for (int ix = 0; ix < array_cnt; ++ix)
						{
							void* memb_index_address = (char*)memb_address + ix*mtpsz;
							f_assingn_json_to_memb(memb_index_address, marray[ix]);
						}
						
					}
				}
				else{
					if (mtype == "int"){
						*(int*)memb_address = jvalue[mname].asInt();
					}
					else if (mtype == "float" || mtype == "double"){
						*(float*)memb_address=jvalue[mname].asDouble();
					}
					else if (mtype == "af_vec2"){
						Value jv2=jvalue[mname];
						 *(float*)memb_address=jv2["x"].asDouble();
						 *((float*)memb_address + 1)=jv2["y"].asDouble();
					}
					else if (mtype == "af_vec3") {
						Value jv3=jvalue[mname];
						 *(float*)memb_address=jv3["x"].asDouble();
						*((float*)memb_address + 1) = jv3["y"].asDouble();
						*((float*)memb_address + 2) = jv3["z"].asDouble();
					}
					else if (mtype == "af_vec4") {
						Value jv4=jvalue[mname];
						 *(float*)memb_address=jv4["x"].asDouble();
						*((float*)memb_address + 1) = jv4["y"].asDouble();
						*((float*)memb_address + 2) = jv4["z"].asDouble();
						*((float*)memb_address + 3) = jv4["w"].asDouble();

					}
					else if (mtype == "bool"){
						*(bool*)memb_address=jvalue[mname].asBool();
					}
					else{
						Value vbytes=jvalue[mname];//must be string
						strcpy((char*)memb_address, vbytes.asCString());
					}
				}
			}
		}
		link();
		Value childs=jvalue["childs"];
		if (!childs.isArray())
		{
			return;
		}
		size_t chcnt = childs.size();
		for (size_t ix = 0; ix < chcnt; ix++)
		{
			Value& child = childs[ix];
			auto& cname = child["type"].asString();
			base_ui_component* pcontrol_instance = factory::get().produce(cname);
			add_child(pcontrol_instance);
			pcontrol_instance->init_property_from_json(child);
			
		}
	}
	void base_ui_component::save_property_to_json(Value& junit){
		string cname = typeid(*this).name();
		cname = cname.substr(sizeof("class autofuture::"));
		junit["type"] = cname;
		for (auto& prop_ele : _vprop_eles)
		{
			auto& prop_page = prop_ele->_pro_page;
			int prev_memb_offset_p_tpsz = 0;
			for (auto& memb : prop_page)
			{
				auto mtype = memb->_type;
				auto mname = memb->_name;
				auto mtpsz = memb->_tpsz;
				auto moffset = memb->_offset;
				void* memb_address = 0;
				int array_cnt = 0;
				string::size_type apos = mname.find('[');
				if (apos != string::npos)//is array
				{
					mname = mname.substr(0, apos);
					int wsz = moffset - prev_memb_offset_p_tpsz;
					array_cnt = wsz / mtpsz;
					memb_address = (char*)prop_ele->_pro_address + prev_memb_offset_p_tpsz;
					prev_memb_offset_p_tpsz = moffset;
				}
				else{
					auto eppos = mname.find('=');
					if (eppos != string::npos){
						mname = mname.substr(0, eppos);
					}
					else{
						auto brpos = mname.find('{');
						if (brpos != string::npos)
						{
							mname = mname.substr(0, brpos);
						}
					}
					memb_address = (char*)prop_ele->_pro_address + moffset;
					prev_memb_offset_p_tpsz = moffset + mtpsz;
				}
				if (array_cnt > 0){
					if (mtype == "char")
					{
						char* str_memb= new char[array_cnt];
						memset(str_memb, 0, array_cnt);
						memcpy(str_memb, memb_address, array_cnt);
						junit[mname] = str_memb;
						delete str_memb;
					}
					else
					{
						Value marray(arrayValue);
						function<void(void*)> f_save_to_json;
						if (mtype=="int"){
							f_save_to_json = [&marray](void* membaddr)
							{
								int imemb = *(int*)membaddr;
								marray.append(imemb);
							};
						}
						else if (mtype=="float"|| mtype == "double"){
							f_save_to_json = [&marray](void* membaddr)
							{
								float fmemb = *(float*)membaddr;
								marray.append(fmemb);
							};
						}
						else if (mtype == "af_vec2"){
							f_save_to_json = [&marray](void* membaddr)
							{
								Value jv2(objectValue);
								jv2["x"] = *(float*)membaddr;
								jv2["y"] = *((float*)membaddr + 1);
								marray.append(jv2);
							};							
						}
						else if (mtype == "af_vec3"){
							f_save_to_json = [&marray](void* membaddr)
							{
								Value jv3(objectValue);
								jv3["x"] = *(float*)membaddr;
								jv3["y"] = *((float*)membaddr + 1);
								jv3["z"] = *((float*)membaddr + 2);
								marray.append(jv3);
							};
						}
						else if (mtype == "af_vec4"){
							f_save_to_json = [&marray](void* membaddr)
							{
								Value jv4(objectValue);
								jv4["x"] = *(float*)membaddr;
								jv4["y"] = *((float*)membaddr + 1);
								jv4["z"] = *((float*)membaddr + 2);
								jv4["z"] = *((float*)membaddr + 3);
								marray.append(jv4);
							};
						}
						else if (mtype == "bool"){
							f_save_to_json = [&marray](void* membaddr)
							{
								bool bmemb = *(bool*)membaddr;
								marray.append(bmemb);
							};
						}
						else {
							f_save_to_json = [&](void* membaddr)
							{

								char* pbytes = new char[mtpsz+1];
								int ix = 0;
								for (; ix < mtpsz; ++ix)
								{
									pbytes[ix] = *((char*)membaddr + ix);
								}
								pbytes[ix] = '\0';
								Value vbytes=pbytes;
								delete pbytes;
								marray.append(vbytes);
							};
						}
						for (int ix = 0; ix < array_cnt; ++ix)
						{
							void* memb_index_address = (char*)memb_address + ix*mtpsz;
							f_save_to_json(memb_index_address);
						}
						junit[mname] = marray;
					}
				}
				else{
					if (mtype == "int"){
						junit[mname] = *(int*)memb_address;
					}
					else if (mtype == "float" || mtype == "double"){
						junit[mname] = *(float*)memb_address;
					}
					else if (mtype == "af_vec2"){
						Value jv2(objectValue);
						jv2["x"] = *(float*)memb_address;
						jv2["y"]= *((float*)memb_address+1);
						junit[mname] = jv2;
					}
					else if (mtype == "af_vec3") {
						Value jv3(objectValue);
						jv3["x"] = *(float*)memb_address;
						jv3["y"] = *((float*)memb_address + 1);
						jv3["z"] = *((float*)memb_address + 2);
						junit[mname] = jv3;
					}
					else if (mtype == "af_vec4") {
						Value jv4(objectValue);
						jv4["x"] = *(float*)memb_address;
						jv4["y"] = *((float*)memb_address + 1);
						jv4["z"] = *((float*)memb_address + 2);
						jv4["w"] = *((float*)memb_address + 3);
						junit[mname] = jv4;

					}
					else if (mtype == "bool"){
						junit[mname] = *(bool*)memb_address;
					}
					else{
						char* pbytes = new char[mtpsz+1];
						int ix = 0;
						for (; ix < mtpsz; ++ix)
						{
							pbytes[ix] = *((char*)memb_address + ix);
						}
						pbytes[ix] = '\0';
						junit[mname] = pbytes;
						delete pbytes;
					}
				}
			}
		}
		Value jchilds(arrayValue);
		size_t chcnt = child_count();
		for (size_t ix = 0; ix < chcnt; ix++)
		{
			base_ui_component* pch_uc = get_child(ix);
			Value jchuc(objectValue);
			pch_uc->save_property_to_json(jchuc);
			jchilds.append(jchuc);
		}
		if (chcnt > 0)
		{
			junit["childs"] = jchilds;
		}
	}

}