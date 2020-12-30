#include "control_common_def.h"
#include "res_output.h"
#include "command_element_delta.h"
#include "af_factory.h"
#include "bind_edit.h"
#include "state_manager_edit.h"
#include "aliase_edit.h"
#include "base_prp_type_edit.h"
#include "common_functions.h"
#include <regex>

extern bind_edit g_bind_edit;
extern state_manager_edit g_state_manager_edit;
extern aliase_edit g_aliase_edit;
extern base_prp_type_edit g_common_value_edit;
extern bool show_bind_edit;
extern bool show_aliase_edit;
#define INT_VALUE 45
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

namespace auto_future
{
	mp_tp_propty_handle _mcustom_type_property_handles_container;
	void reg_property_handle(string tpname, property_handle ph)
	{
		_mcustom_type_property_handles_container[tpname] = ph;
	}
	void init_common_type_property_handles()
	{
		reg_property_handle("camera", [](void* membadr){
			camera* pcamera = reinterpret_cast<camera*>(membadr);
			ImGui::Text("Camera");
			ImGui::SliderFloat("Position x", &pcamera->_position.x, -base_ui_component::screenw, base_ui_component::screenw);
			ImGui::SliderFloat("Position y", &pcamera->_position.y, -base_ui_component::screenh, base_ui_component::screenh);
			ImGui::SliderFloat("Position z", &pcamera->_position.z, -base_ui_component::screenh, base_ui_component::screenh);

			ImGui::SliderFloat("Direction x", &pcamera->_direction.x, -10, 10);
			ImGui::SliderFloat("Direction y", &pcamera->_direction.y, -10, 10);
			ImGui::SliderFloat("Direction z", &pcamera->_direction.z, -10, 10);

			ImGui::SliderFloat("Up x", (float*)&pcamera->_up.x, -10, 10);
			ImGui::SliderFloat("Up y", (float*)&pcamera->_up.y, -10, 10);
			ImGui::SliderFloat("Up z", (float*)&pcamera->_up.z, -10, 10);

		});
		reg_property_handle("directional_light", [](void* membadr){
			directional_light* pdr_lt = reinterpret_cast<directional_light*>(membadr);
			ImGui::Text("Directional light");
			ImGui::ColorEdit3("Light color", (float*)&pdr_lt->_color, ImGuiColorEditFlags_RGB);
			ImGui::SliderFloat("Light direction x", &pdr_lt->_direction.x, -10, 10);
			ImGui::SliderFloat("Light direction y", &pdr_lt->_direction.y, -10, 10);
			ImGui::SliderFloat("Light direction z", &pdr_lt->_direction.z, -10, 10);

		});
		reg_property_handle("point_light", [](void* membadr){
			point_light* ppt_lt = reinterpret_cast<point_light*>(membadr);
			ImGui::Text("Point light");
			ImGui::ColorEdit3("Point light color", (float*)&ppt_lt->_color, ImGuiColorEditFlags_RGB);
			ImGui::SliderFloat("Point light position x", &ppt_lt->_position.x, -base_ui_component::screenw, base_ui_component::screenw);
			ImGui::SliderFloat("Point light position y", &ppt_lt->_position.y, -base_ui_component::screenh, base_ui_component::screenh);
			ImGui::SliderFloat("Point light position z", &ppt_lt->_position.z, -base_ui_component::screenh, base_ui_component::screenh);

		});
		reg_property_handle("transformation", [](void* membadr){
			transformation* ptrans = reinterpret_cast<transformation*>(membadr);
			ImGui::Text("Transformation");
			ImGui::Checkbox("SameScale:", &ptrans->_same_scale);
			if (ptrans->_same_scale)
			{
				if (ImGui::SliderFloat("Scale", &ptrans->_scale.x, -10.f, 10.f,"%.4f",0.01f))
				{
					ptrans->_scale.y = ptrans->_scale.x;
					ptrans->_scale.z = ptrans->_scale.x;
				}
				//ImGui::SliderFloat("Scale", &ptrans->_scale.y, -10.f, 10.f);
				//ImGui::SliderFloat("Scale", &ptrans->_scale.z, -10.f, 10.f);
			}
			else
			{
				ImGui::SliderFloat("Scale x", &ptrans->_scale.x, -10.f, 10.f,"%.4f", 0.01f);
				ImGui::SliderFloat("Scale y", &ptrans->_scale.y, -10.f, 10.f, "%.4f", 0.01f);
				ImGui::SliderFloat("Scale z", &ptrans->_scale.z, -10.f, 10.f, "%.4f", 0.01f);
			}
			ImGui::SliderFloat("Rotation x", &ptrans->_rotation.x, -180.f, 180.f);
			ImGui::SliderFloat("Rotation y", &ptrans->_rotation.y, -180.f, 180.f);
			ImGui::SliderFloat("Rotation z", &ptrans->_rotation.z, -180.f, 180.f);
			ImGui::SliderFloat("Translation x", &ptrans->_translation.x, -base_ui_component::screenw, base_ui_component::screenw, "%.3f", 0.1f);
			ImGui::SliderFloat("Translation y", &ptrans->_translation.y, -base_ui_component::screenw, base_ui_component::screenw, "%.3f", 0.1f);
			ImGui::SliderFloat("Translation z", &ptrans->_translation.z, -base_ui_component::screenw, base_ui_component::screenw, "%.3f", 0.1f);

		});
		reg_property_handle("projection", [](void* membadr){
			projection* pproj = reinterpret_cast<projection*>(membadr);
			ImGui::Text("Projection");
			ImGui::SliderFloat("Fovy", (float*)&pproj->_fovy, 0.f, 180.f);
			//ImGui::SliderFloat("Aspect", (float*)&pproj->_aspect, 0.f, 10);
			ImGui::SliderFloat("Near", (float*)&pproj->_near, 0.f, 180.f);
			ImGui::SliderFloat("Far", (float*)&pproj->_far, 0.f, 180.f);

		});
	}
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
static	string bind_btn_cp = "->##";
static	string aliase_btn_cp = "  ##";

	void base_ui_component::draw_peropertys()
	{
		int pgidx = 0;
		string type_name = typeid(*this).name();
		string type_show = type_name.substr(sizeof("class autofuture::"));
		ImGui::Text("Type name:%s", type_show.c_str());
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
				for (auto& memb:prop_page)
				{
					auto mtype = memb->_type;
					auto mname = memb->_name;
					auto mtpsz = memb->_tpsz;
					char* memb_address = memb->_address;
					int array_cnt = memb->_count;
					string::size_type apos = mname.find('[');
					if (apos!=string::npos)//is array
					{
						mname = mname.substr(0, apos);
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
                         string float_format = "%.3f";
                         if( regex_search(mname, regex("_hac") ))
                         {
                              float_format = "%.6f";
                         }
					cmd_value_block before_op_memb_value;
					before_op_memb_value.reserve(mtpsz);
					before_op_memb_value.resize(mtpsz);
					memcpy(&before_op_memb_value[0], memb_address, mtpsz);

					auto& imemb_tp_handl = _mcustom_type_property_handles_container.find(mtype);
					bool be_base_type = mtype == "int" || mtype == "float" || mtype == "double" || mtype == "bool"\
						||mtype=="af_vi2"||mtype=="af_vi3"||mtype=="af_vi4"\
						||mtype=="af_vec2"||mtype=="af_vec3"||mtype=="af_vec4";
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
                                        f_draw_index_prop = [&]( string& str_show, void*maddress )
                                        {
                                             return  ImGui::SliderInt( str_show.c_str(), (int*)maddress, _vrange._min._i, _vrange._max._i );
                                        };
							}
							else if (mtype == "float" || mtype == "double"){
								//be_base_type = true;
								f_draw_index_prop = [&](string& str_show, void*maddress){
                                             return  ImGui::SliderFloat( str_show.c_str(), (float*)maddress, _vrange._min._f, _vrange._max._f, float_format.c_str());
								};
							}
                                   else if (mtype=="af_vi2")
                                   {
                                        if( rg == "txt" )// atexture
                                        {
                                             f_draw_index_prop = [&]( string& str_show, void*maddress )
                                             {
                                                  af_vi2* ptxt_idx = (af_vi2*)maddress;
                                                  int igsize = g_vres_texture_list.size();
                                                  string str_gp = "Group id of" + str_show;
                                                  ImGui::Combo( str_gp.c_str(), &ptxt_idx->x, get_texture_group_name, &g_vres_texture_list, igsize );
                                                  int img_gp_id = ptxt_idx->x;
                                                  auto& res_coors = g_vres_texture_list[ img_gp_id ].vtexture_coordinates;
                                                  int isize = g_vres_texture_list[ img_gp_id ].vtexture_coordinates.size();
                                                  int txt_idx = ptxt_idx->y;
                                                  bool be_changed = ImGui::Combo( str_show.c_str(), &ptxt_idx->y, &get_texture_item, &img_gp_id, isize );

                                                  ImGui::SameLine(); ShowHelpMarker( "select a image from image resource!\n" );
                                                  float reswidth = res_coors[ txt_idx ].owidth();
                                                  float resheight = res_coors[ txt_idx ].oheight();
                                                  ImGui::Text( "original size:%f,%f", reswidth, resheight );
                                                  ImGui::Spacing();
                                                  if( reswidth > 0 )
                                                  {
                                                       float draw_height = imge_edit_view_width*resheight / reswidth;
                                                       ImVec2 draw_size( imge_edit_view_width, draw_height );
                                                       int texture_id = g_vres_texture_list[ img_gp_id ].texture_id();
                                                       float wtexture_width = g_vres_texture_list[ img_gp_id ].texture_width;
                                                       float wtexture_height = g_vres_texture_list[ img_gp_id ].texture_height;

                                                       ImVec2 uv0( res_coors[ txt_idx ]._x0 / wtexture_width, res_coors[ txt_idx ]._y0 / wtexture_height );
                                                       ImVec2 uv1( res_coors[ txt_idx ]._x1 / wtexture_width, res_coors[ txt_idx ]._y1 / wtexture_height );
                                                       ImGui::Image( (ImTextureID)texture_id, draw_size, uv0, uv1, ImColor( 255, 255, 255, 255 ), ImColor( 255, 255, 255, 128 ) );
                                                  }
                                                  return be_changed;
                                             };
                                        }
                                        else 
                                        {
                                             f_draw_index_prop = [&]( string& str_show, void*maddress )
                                             {
                                                  return ImGui::SliderInt2( str_show.c_str(), (int*)maddress, _vrange._min._i, _vrange._max._i );
                                             };
                                        }
                                   }
                                   else if (mtype=="af_vi3")
                                   {
                                        f_draw_index_prop = [&]( string& str_show, void*maddress )
                                        {
                                             return ImGui::SliderInt3( str_show.c_str(), (int*)maddress, _vrange._min._i, _vrange._max._i );
                                        };
                                   }
                                   else if( mtype == "af_vi4" )
                                   {
                                        f_draw_index_prop = [&]( string& str_show, void*maddress )
                                        {
                                             return ImGui::SliderInt4( str_show.c_str(), (int*)maddress, _vrange._min._i, _vrange._max._i );
                                        };
                                   }
							else if (mtype == "af_vec2"){
								f_draw_index_prop = [&](string& str_show, void*maddress){
                                             return ImGui::SliderFloat2( str_show.c_str(), (float*)maddress, _vrange._min._f, _vrange._max._f, float_format.c_str() );
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
                                                  return ImGui::SliderFloat3( str_show.c_str(), (float*)maddress, _vrange._min._f, _vrange._max._f, float_format.c_str() );
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
                                                  return ImGui::SliderFloat4( str_show.c_str(), (float*)maddress, _vrange._min._f, _vrange._max._f, float_format.c_str() );
									}
								};
							}
							else if (mtype == "bool"){
								//be_base_type = true;
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
									//be_base_type = false;
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
						static void* pmem_address = 0;
						static bool be_operating = false;
						static int page_idx=-1, fd_idx=-1;
						cmd_value_block after_op_memb_value;
						after_op_memb_value.reserve(mtpsz);
						after_op_memb_value.resize(mtpsz);
						memcpy(&after_op_memb_value[0], memb_address, mtpsz);

						ImGuiContext& g = *GImGui;
						if (g.operating_be_started)
						{
							printf("start changing value\n");
							pmem_address = memb_address;
							page_idx = pgidx;
							fd_idx = idx;
							bk_memb_value = before_op_memb_value;
							g.operating_be_started = false;
							be_operating = true;
						}
						if (be_operating)
						{
							if (g.IO.MouseReleased[0])
							{
								g_ui_edit_command_mg.create_command(edit_commd<base_ui_component>(this, pmem_address, &bk_memb_value[0], bk_memb_value.size()));
								be_operating = false;
								prop_ele_position cur_prp_ele_pos = { this, page_idx, fd_idx };
								calcu_bind_node(cur_prp_ele_pos);
								page_idx = fd_idx = -1;
							}
						}
						else if (before_op_memb_value != after_op_memb_value)
						{
							g_ui_edit_command_mg.create_command(edit_commd<base_ui_component>(this, memb_address, &before_op_memb_value[0], before_op_memb_value.size()));
							prop_ele_position cur_prp_ele_pos = { this, pgidx, idx };
							calcu_bind_node(cur_prp_ele_pos);
							page_idx = fd_idx = -1;
						}
						
						g.IO.InputContentChanged = false;
					}
					//bind_btn_cp += "#";
					char idstr[50] = { 0 };
					sprintf(idstr, "%d_%d", pgidx, idx);
					string btn_cap = aliase_btn_cp + idstr;
					ImGui::SameLine();
					/*if (ImGui::Button(btn_cap.c_str())&&ImGui::IsMouseDoubleClicked(0))
					{
					printf("%s is double clicked\n", idstr);
					}*/
					bool be_sel = false;
					if (ImGui::Selectable(btn_cap.c_str(), &be_sel, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(10, 15)) && ImGui::IsMouseDoubleClicked(0))
					{
						printf("%s is double(sel) clicked\n", idstr);
						show_aliase_edit = true;
						g_aliase_edit.sel_aliase(this, pgidx, idx);
					}
					if (be_base_type)
					{
						btn_cap = bind_btn_cp + idstr;
						ImGui::SameLine();
						if (ImGui::Button(btn_cap.c_str()))
						{
							g_bind_edit.sel_prop_ele(this, pgidx, idx);
							show_bind_edit = true;
						}
						if (ImGui::IsItemActive())
						{
							// Draw a line between the button and the mouse cursor
							ImDrawList* draw_list = ImGui::GetWindowDrawList();
							draw_list->PushClipRectFullScreen();
							ImGuiIO& io = ImGui::GetIO();
							draw_list->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
							draw_list->PopClipRect();
							g_bind_edit.set_dragging(true, this, pgidx, idx);
							g_state_manager_edit.set_dragging(true, this, pgidx, idx);
                                   g_common_value_edit.set_dragging( true, this, pgidx, idx );
							//ImGui::Button("Drag Me");
						}
					}
					if (ImGui::IsMouseReleased(0))
					{
						g_bind_edit.set_dragging(false,this);
						g_state_manager_edit.set_dragging(false, this);
                              g_common_value_edit.set_dragging( false, this );
					}
					idx++;
				}
				ImGui::Separator();
				ImGui::Spacing();
			}
			pgidx++;
		}
	}
	
	void base_ui_component::init_property_from_json(Value& jvalue){

		for (auto& prop_ele : _vprop_eles)
		{
			auto& prop_page = prop_ele->_pro_page;
			for (auto& memb : prop_page)
			{
				auto mtype = memb->_type;
				auto mname = memb->_name;
				auto mtpsz = memb->_tpsz;
				char* memb_address = memb->_address;
				int array_cnt = memb->_count;
				string::size_type apos = mname.find('[');
				if (apos != string::npos)//is array
				{
					mname = mname.substr(0, apos);
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
				}
                    function<void( void*, Value& )> f_assingn_json_to_memb;
                    if( mtype == "int" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(int*)membaddr = vele.asInt();
                         };
                    }
                    else if( mtype == "float" || mtype == "double" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(float*)membaddr = vele.asDouble();
                         };
                    }
                    else if( mtype == "af_vi2" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(int*)membaddr = vele[ "x" ].asInt();
                              *( (int*)membaddr + 1 ) = vele[ "y" ].asInt();
                         };
                    }
                    else if( mtype == "af_vi3" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(int*)membaddr = vele[ "x" ].asInt();
                              *( (int*)membaddr + 1 ) = vele[ "y" ].asInt();
                              *( (int*)membaddr + 2 ) = vele[ "z" ].asInt();
                         };
                    }
                    else if( mtype == "af_vi4" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(int*)membaddr = vele[ "x" ].asInt();
                              *( (int*)membaddr + 1 ) = vele[ "y" ].asInt();
                              *( (int*)membaddr + 2 ) = vele[ "z" ].asInt();
                              *( (int*)membaddr + 3 ) = vele[ "w" ].asInt();
                         };
                    }
                    else if( mtype == "af_vec2" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(float*)membaddr = vele[ "x" ].asDouble();
                              *( (float*)membaddr + 1 ) = vele[ "y" ].asDouble();
                         };
                    }
                    else if( mtype == "af_vec3" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(float*)membaddr = vele[ "x" ].asDouble();
                              *( (float*)membaddr + 1 ) = vele[ "y" ].asDouble();
                              *( (float*)membaddr + 2 ) = vele[ "z" ].asDouble();
                         };
                    }
                    else if( mtype == "af_vec4" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(float*)membaddr = vele[ "x" ].asDouble();
                              *( (float*)membaddr + 1 ) = vele[ "y" ].asDouble();
                              *( (float*)membaddr + 2 ) = vele[ "z" ].asDouble();
                              *( (float*)membaddr + 3 ) = vele[ "w" ].asDouble();
                         };
                    }
                    else if( mtype == "bool" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(bool*)membaddr = vele.asBool();
                         };
                    }
                    else
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              string out_bin;
                              convert_string_to_binary( vele.asString(), out_bin );
                              memcpy( membaddr, &out_bin[ 0 ], out_bin.size() );
                         };
                    }
				if (array_cnt > 1)
                    {
                         Value& jtemp = jvalue[ mname ];
                         if (jtemp.isNull())
                         {
                              continue;
                         }
					if (mtype == "char")
					{
                              Value&vbytes = jtemp;//must be string
						strcpy((char*)memb_address, vbytes.asCString());
					}
					else
					{
						Value& marray=jtemp;

						for (int ix = 0; ix < array_cnt; ++ix)
						{
							void* memb_index_address = (char*)memb_address + ix*mtpsz;
							f_assingn_json_to_memb(memb_index_address, marray[ix]);
						}
						
					}
				}
				else
                    {
                         Value& jtemp = jvalue[ mname ];
                         if (jtemp.isNull())
                         {
                              continue;
                         }
                         f_assingn_json_to_memb( memb_address, jtemp );
					
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
     void base_ui_component::init_property_from_json( Value& jvalue, dic_id& font_dic, dic_id& txt_dic )
     {
          for( auto& prop_ele : _vprop_eles )
          {
               auto& prop_page = prop_ele->_pro_page;
               for( auto& memb : prop_page )
               {
                    auto mtype = memb->_type;
                    auto mname = memb->_name;
                    auto mtpsz = memb->_tpsz;
                    string rg = mname.substr( mname.length() - 3, 3 );
                    char* memb_address = memb->_address;
                    int array_cnt = memb->_count;
                    string::size_type apos = mname.find( '[' );
                    if( apos != string::npos )//is array
                    {
                         mname = mname.substr( 0, apos );
                    }
                    else
                    {
                         auto eppos = mname.find( '=' );
                         if( eppos != string::npos )
                         {
                              mname = mname.substr( 0, eppos );
                         }
                         else
                         {
                              auto brpos = mname.find( '{' );
                              if( brpos != string::npos )
                              {
                                   mname = mname.substr( 0, brpos );
                              }
                         }
                    }
                    function<void( void*, Value& )> f_assingn_json_to_memb;
                    if( mtype == "int" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              if (mname=="_font_id")
                              {
                                   *(int*)membaddr = font_dic[vele.asInt()];
                              }
                              else
                              {
                                   *(int*)membaddr = vele.asInt();
                              }
                         };
                    }
                    else if( mtype == "float" || mtype == "double" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(float*)membaddr = vele.asDouble();
                         };
                    }
                    else if( mtype == "af_vi2" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              if( rg =="txt")
                              {
                                   *(int*)membaddr = txt_dic[vele[ "x" ].asInt()];
                              }
                              else
                              {
                                   *(int*)membaddr = vele[ "x" ].asInt();
                              }
                              *( (int*)membaddr + 1 ) = vele[ "y" ].asInt();
                         };
                    }
                    else if( mtype == "af_vi3" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(int*)membaddr = vele[ "x" ].asInt();
                              *( (int*)membaddr + 1 ) = vele[ "y" ].asInt();
                              *( (int*)membaddr + 2 ) = vele[ "z" ].asInt();
                         };
                    }
                    else if( mtype == "af_vi4" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(int*)membaddr = vele[ "x" ].asInt();
                              *( (int*)membaddr + 1 ) = vele[ "y" ].asInt();
                              *( (int*)membaddr + 2 ) = vele[ "z" ].asInt();
                              *( (int*)membaddr + 3 ) = vele[ "w" ].asInt();
                         };
                    }
                    else if( mtype == "af_vec2" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(float*)membaddr = vele[ "x" ].asDouble();
                              *( (float*)membaddr + 1 ) = vele[ "y" ].asDouble();
                         };
                    }
                    else if( mtype == "af_vec3" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(float*)membaddr = vele[ "x" ].asDouble();
                              *( (float*)membaddr + 1 ) = vele[ "y" ].asDouble();
                              *( (float*)membaddr + 2 ) = vele[ "z" ].asDouble();
                         };
                    }
                    else if( mtype == "af_vec4" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(float*)membaddr = vele[ "x" ].asDouble();
                              *( (float*)membaddr + 1 ) = vele[ "y" ].asDouble();
                              *( (float*)membaddr + 2 ) = vele[ "z" ].asDouble();
                              *( (float*)membaddr + 3 ) = vele[ "w" ].asDouble();
                         };
                    }
                    else if( mtype == "bool" )
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              *(bool*)membaddr = vele.asBool();
                         };
                    }
                    else
                    {
                         f_assingn_json_to_memb = [&]( void* membaddr, Value& vele )
                         {
                              string out_bin;
                              convert_string_to_binary( vele.asString(), out_bin );
                              memcpy( membaddr, &out_bin[ 0 ], out_bin.size() );
                         };
                    }
                    if( array_cnt > 1 )
                    {
                         Value& jtemp = jvalue[ mname ];
                         if( jtemp.isNull() )
                         {
                              continue;
                         }
                         if( mtype == "char" )
                         {
                              Value&vbytes = jtemp;//must be string
                              strcpy( (char*)memb_address, vbytes.asCString() );
                         }
                         else
                         {
                              Value& marray = jtemp;

                              for( int ix = 0; ix < array_cnt; ++ix )
                              {
                                   void* memb_index_address = (char*)memb_address + ix*mtpsz;
                                   f_assingn_json_to_memb( memb_index_address, marray[ ix ] );
                              }

                         }
                    }
                    else
                    {
                         Value& jtemp = jvalue[ mname ];
                         if( jtemp.isNull() )
                         {
                              continue;
                         }
                         f_assingn_json_to_memb( memb_address, jtemp );

                    }
               }
          }
          link();
          Value childs = jvalue[ "childs" ];
          if( !childs.isArray() )
          {
               return;
          }
          size_t chcnt = childs.size();
          for( size_t ix = 0; ix < chcnt; ix++ )
          {
               Value& child = childs[ ix ];
               auto& cname = child[ "type" ].asString();
               base_ui_component* pcontrol_instance = factory::get().produce( cname );
               add_child( pcontrol_instance );
               pcontrol_instance->init_property_from_json( child );
          }
     }
	void base_ui_component::save_property_to_json(Value& junit){

		string cname = typeid(*this).name();
		cname = cname.substr(sizeof("class autofuture::"));
		junit["type"] = cname;
		for (auto& prop_ele : _vprop_eles)
		{
			auto& prop_page = prop_ele->_pro_page;
			for (auto& memb : prop_page)
			{
				auto mtype = memb->_type;
				auto mname = memb->_name;
				auto mtpsz = memb->_tpsz;
				void* memb_address = memb->_address;
				int array_cnt = memb->_count;
				string::size_type apos = mname.find('[');
				if (apos != string::npos)//is array
				{
					mname = mname.substr(0, apos);
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
                              else if (mtype=="af_vi2")
                              {
                                   f_save_to_json = [&marray]( void* membaddr )
                                   {
                                        Value jv2( objectValue );
                                        jv2[ "x" ] = *(int*)membaddr;
                                        jv2[ "y" ] = *( (int*)membaddr + 1 );
                                        marray.append( jv2 );
                                   };
                              }
                              else if( mtype == "af_vi3" )
                              {
                                   f_save_to_json = [&marray]( void* membaddr )
                                   {
                                        Value jv( objectValue );
                                        jv[ "x" ] = *(int*)membaddr;
                                        jv[ "y" ] = *( (int*)membaddr + 1 );
                                        jv[ "z" ] = *( (int*)membaddr + 2 );
                                        marray.append( jv);
                                   };
                              }
                              else if( mtype == "af_vi4" )
                              {
                                   f_save_to_json = [&marray]( void* membaddr )
                                   {
                                        Value jv( objectValue );
                                        jv[ "x" ] = *(int*)membaddr;
                                        jv[ "y" ] = *( (int*)membaddr + 1 );
                                        jv[ "z" ] = *( (int*)membaddr + 2 );
                                        jv[ "w" ] = *( (int*)membaddr + 3 );
                                        marray.append( jv );
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

								string vsbytes;
								convert_binary_to_string((char*)membaddr, mtpsz, vsbytes);
								vsbytes[mtpsz] = '\0';
								Value vbytes = vsbytes;
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
                         else if( mtype == "af_vi2" )
                         {
                              Value jv( objectValue );
                              jv[ "x" ] = *(int*)memb_address;
                              jv[ "y" ] = *( (int*)memb_address + 1 );
                              junit[ mname ] = jv;
                         }
                         else if( mtype == "af_vi3" )
                         {
                              Value jv( objectValue );
                              jv[ "x" ] = *(int*)memb_address;
                              jv[ "y" ] = *( (int*)memb_address + 1 );
                              jv[ "z" ] = *( (int*)memb_address + 2 );
                              junit[ mname ] = jv;
                         }
                         else if( mtype == "af_vi4" )
                         {
                              Value jv( objectValue );
                              jv[ "x" ] = *(int*)memb_address;
                              jv[ "y" ] = *( (int*)memb_address + 1 );
                              jv[ "z" ] = *( (int*)memb_address + 2 );
                              jv[ "w" ] = *( (int*)memb_address + 3 );
                              junit[ mname ] = jv;
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
						string vsbytes;
						convert_binary_to_string((char*)memb_address, mtpsz, vsbytes);
						vsbytes[mtpsz*2] = '\0';
						junit[mname] = vsbytes;
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