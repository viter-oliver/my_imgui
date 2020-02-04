#include "ui_assembler.h"
#include <fstream>
#include "texture_res_load.h"
#include "af_shader.h"
#include "af_material.h"
#include "af_feedback.h"
#include "af_font_res_set.h"
#include "SOIL.h"
#include "dir_output.h"
#include "af_model.h"
#include "af_bind.h"
#include "af_state_manager.h"
#include "common_functions.h"
#include "af_factory.h"
#include "imgui_impl_glfw_gl3.h"
//#include "af_model.h"
//#include "./fbx_save_info.h"
extern string g_cureent_directory, g_afb_output_path;
extern bool show_project_window, show_edit_window, show_property_window,\
show_resource_manager, show_fonts_manager, show_file_manager,show_state_manager_edit,\
show_aliase_edit, show_slider_path_picker;
void init_controls_res_constrained()
{
	auto vres_texture_constrain = [&](){return g_vres_texture_list.size() > 0; };
	factory::get().register_res_constrained("ft_image", vres_texture_constrain);
	factory::get().register_res_constrained("ft_button", vres_texture_constrain);
	factory::get().register_res_constrained("ft_polygon_image", vres_texture_constrain);
	factory::get().register_res_constrained("ft_slider", vres_texture_constrain);
	factory::get().register_res_constrained("ft_slider_thumb", vres_texture_constrain);
	factory::get().register_res_constrained("ft_quad_image", vres_texture_constrain);

	factory::get().register_res_constrained("ft_modeling_3d", [&](){return g_mmodel_list.size() > 0; });
	factory::get().register_res_constrained("ft_textblock", [&](){
		auto& ft_nm_list = g_pfont_face_manager->get_dic_fonts();
		return ft_nm_list.size() > 0; });
	factory::get().register_res_constrained("ft_image_play", [&](){return g_mtexture_list.size() > 0 && g_mfiles_list.size()>0; });

}
void clear_pre_proj_resource()
{
	auto& ft_nm_list = g_pfont_face_manager->get_dic_fonts();
	ft_nm_list.clear();
	g_output_bin_format = { en_uncompressed_txt, en_shader_code };
	g_vres_texture_list.clear();
	g_mtexture_list.clear();
	g_mfiles_list.clear();
	g_material_list.clear();
	g_af_shader_list.clear();
	g_primitive_list.clear();
	g_mmodel_list.clear();
	g_aliase_dic.clear();
	g_bind_dic.clear();
	g_bind_ref_dic.clear();
	g_mstate_manager.clear();
     g_base_prp_dic.clear();
}
extern HCURSOR g_hcursor_wait;
bool ui_assembler::load_ui_component_from_file(const char* file_path)
{
	//ImGuiMouseCursor old_cursor=setMouseCusor()
	HCURSOR hcur_cursor= GetCursor();
	SetCursor(g_hcursor_wait);
	ifstream fin;
	fin.open(file_path);
	if (fin.is_open())
	{
		Reader reader;
		Value jroot;
		if (reader.parse(fin, jroot, false))
		{
			clear_pre_proj_resource();
			Value screenw = jroot["screenw"];
			Value screenh = jroot["screenh"];
			if (!screenw.isNull())
			{
				base_ui_component::screenw = screenw.asDouble();
			}
			else
			{
			    base_ui_component::screenw = 2000.0f;
			}
			if (!screenh.isNull())
			{
				base_ui_component::screenh = screenh.asDouble();
			}
			else
			{
				base_ui_component::screenh = 1000.0f;
			}
			Value& window_show = jroot["window_show"];
			if (!window_show.isNull())
			{
				show_project_window = window_show["show_project_window"].asBool();
				show_edit_window=window_show["show_edit_window"].asBool();
				show_property_window = window_show["show_property_window"].asBool();
				show_resource_manager = window_show["show_resource_manager"].asBool();
				show_fonts_manager = window_show["show_fonts_manager"].asBool();
				show_file_manager=window_show["show_file_manager"].asBool();
				show_state_manager_edit=window_show["show_state_manager_edit"].asBool();
				show_aliase_edit = window_show["show_aliase_edit"].asBool();
				show_slider_path_picker = window_show["show_slider_path_picker"].asBool();

			}
               Value& afb_output_path = jroot[ "afb_output_path" ];
               if( afb_output_path.isNull() )
               {
                    g_afb_output_path = g_cureent_directory + afb_fold;
               }
               else
               {
                    g_afb_output_path = afb_output_path.asString();
               }
			Value& fonts = jroot["fonts"];
			if (!fonts.isNull())
			{
				string str_font_path = g_cureent_directory+font_fold;
				int isz = fonts.size();
				for (int ix = 0; ix < isz;ix++)
				{
					Value& jfont = fonts[ix];
					string font_name = jfont["name"].asString();
					string font_full_name= str_font_path + font_name;
					auto ft_u=g_pfont_face_manager->load_font(font_name, font_full_name);
					ft_u->_name = font_name;
					ft_u->_char_count_c = jfont["cols"].asInt();
					ft_u->_char_count_r = jfont["rows"].asInt();
				}

			}
			Value& output_bin_fmt = jroot["output_bin_fmt"];
			if (!output_bin_fmt.isNull())
			{
				g_output_bin_format._txt_fmt = static_cast<texture_format> (output_bin_fmt["txt_fmt"].asInt());
				g_output_bin_format._pgm_fmt = static_cast<program_format> (output_bin_fmt["pgm_fmt"].asInt());
			}
			texture_res_load tresload(g_vres_texture_list);
			tresload.load_res_from_json(jroot);
			Value& texture_list = jroot["texture_list"];
			auto isize = texture_list.size();
			UInt ix = 0;
			string str_img_path = g_cureent_directory+image_fold;
			string str_files_path = g_cureent_directory + files_fold;
			for (ix = 0; ix < isize;ix++)
			{
				Value& txt_unit = texture_list[ix];
				auto& kname = txt_unit["name"].asString();
				auto img_file_path = str_img_path + kname;

				GLubyte* imgdata = NULL;
				int width, height, channels;
				imgdata = SOIL_load_image(img_file_path.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);


				if (imgdata == NULL)
				{
					printf("Fail to load texture file:%s\n", img_file_path.c_str());
					break;
				}
				auto pimge = make_shared<af_texture>();
				pimge->_is_separated = txt_unit["separated"].asBool();
				pimge->_mip_map = txt_unit["mipmap"].asBool();
				pimge->_width = width;
				pimge->_height = height;

				GLuint textureId = 0;
				glGenTextures(1, &textureId);
				pimge->_atxt_id = textureId;
				glBindTexture(GL_TEXTURE_2D, textureId);
				if (channels == SOIL_LOAD_RGBA)
				{
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}
				// Step2 设定wrap参数
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				// Step3 设定filter参数
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				if (pimge->_mip_map)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 					GL_LINEAR_MIPMAP_LINEAR); // 为MipMap设定filter方法
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
					0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
						0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);

				}

				SOIL_free_image_data(imgdata);
				
				g_mtexture_list[kname] = pimge;
			}
			Value& file_list = jroot["file_list"];
			auto fsize = file_list.size();
			for (ix = 0; ix < fsize; ix++)
			{
				Value& filse_unit = file_list[ix];
				auto& kname = filse_unit.asString();
				auto file_path = str_files_path + kname;
				ifstream ifs;
				ifs.open(file_path,ios::binary);

				auto res_size = ifs.tellg();
				ifs.seekg(0, ios::end);
				res_size = ifs.tellg() - res_size;
				ifs.seekg(0, ios::beg);
				g_mfiles_list[kname] = make_shared<af_file>(res_size);
				ifs.read((char*)g_mfiles_list[kname]->_pbin, res_size);
				ifs.close();
			}
			Value& shader_list = jroot["shader_list"];
			isize = shader_list.size();
			string str_shader_path = g_cureent_directory + shaders_fold;
			for (ix = 0; ix < isize; ix++)
			{
				Value& shd_unit = shader_list[ix];
				//auto& vs_code = shd_unit["vs_code"].asString();
				//auto& fs_code = shd_unit["fs_code"].asString();
				string vs_file = str_shader_path + shd_unit["vs_name"].asString();
				string fs_file = str_shader_path + shd_unit["fs_name"].asString();
				ifstream ifs_shd;
				string vs_code, fs_code;
				ifs_shd.open(vs_file);
				if (!ifs_shd.is_open())
				{
					continue;
				}
				getline(ifs_shd, vs_code, (char)EOF);
				
				ifs_shd.close();
				ifs_shd.open(fs_file);
				if (!ifs_shd.is_open())
				{
					continue;
				}
				getline(ifs_shd, fs_code, (char)EOF);
				ifs_shd.close();
				shared_ptr<af_shader> pshd = make_shared<af_shader>(vs_code.c_str(), fs_code.c_str());
				pshd->_vs_name = shd_unit["vs_name"].asString();
				pshd->_fs_name = shd_unit["fs_name"].asString();
				pshd->set_name(shd_unit["name"].asString());
				g_af_shader_list[shd_unit["name"].asString()] = pshd;
			}
			Value& material_list = jroot["material_list"];
			isize = material_list.size();
			for (ix = 0; ix < isize; ix++)
			{
				Value& mtl_unit = material_list[ix];
				shared_ptr<material> pmtl = make_shared<material>();
				if(pmtl->init_from_json(mtl_unit))
					g_material_list[pmtl->get_name()]=pmtl;

			}
			Value& primitive_list = jroot["primitive_list"];
			isize = primitive_list.size();
			for (ix = 0; ix < isize; ix++)
			{
				Value& jpm = primitive_list[ix];
				auto& kname = jpm["name"].asString();

				Value& vformat = jpm["format"];
				GLuint ebo_len = vformat.size();
				vector<GLubyte> ele_format;
				for (int ii = 0; ii < ebo_len; ii++)
				{
					Value& fmu = vformat[ii];
					ele_format.emplace_back(fmu.asInt());
				}
				//GL_UNSIGNED_INT_8_8_8_8_REV
				GLuint vbo_len = jpm["vbo_len"].asUInt();
				ebo_len = jpm["ebo_len"].asUInt();
				ps_primrive_object ppobj= load_primitive_from_file(kname, ele_format, vbo_len, ebo_len);
				if (ppobj)
				{
					ppobj->_model_name = jpm["model"].asString();
					ppobj->_mesh_id = jpm["mesh_id"].asUInt();
					g_primitive_list[kname] = ppobj;
				}
			}
			 Value& models = jroot["models"];
			 if (models.isObject())
			 {
				 Value::Members md_mb = models.getMemberNames();
				 for (auto& md_nm:md_mb)
				 {
					 Value& jmeshlist = models[md_nm];
					 auto pmodel = make_shared<af_model>();
					 g_mmodel_list[md_nm] = pmodel;
					 af_model& mdl = *pmodel;
					 int iisz = jmeshlist.size();
					 for (int iix = 0; iix < iisz; ++iix)
					 {
						 Value& jmesh_unit = jmeshlist[iix];
						 mdl.emplace_back();
						 af_mesh& mesh_unit = mdl[iix];
						 mesh_unit._prm_id = jmesh_unit["prim_id"].asString();
						 auto iprim = g_primitive_list.find(mesh_unit._prm_id);
						 assert(iprim != g_primitive_list.end() && "primitive is missed?");
						 mesh_unit._ps_prm_id = iprim->second;
						 Value& jdiffuse_list = jmesh_unit["diffuse_list"];
						 int jdsize = jdiffuse_list.size();
						 for (int jd = 0; jd < jdsize; ++jd)
						 {
							 auto& diff_txt = jdiffuse_list[jd].asString();
							 mesh_unit._text_diffuse_list.emplace_back(diff_txt);
							 auto& idiff = g_mtexture_list.find(diff_txt);
							 if (idiff!=g_mtexture_list.end())
							 {
								 mesh_unit._ps_text_diffuse_list.emplace_back(idiff->second);
							 }
						 }
						 Value& jspecular_list = jmesh_unit["specular_list"];
						 jdsize = jspecular_list.size();
						 for (int jd = 0; jd < jdsize; ++jd)
						 {
							 auto& specular_txt = jspecular_list[jd].asString();
							 mesh_unit._text_specular_list.emplace_back(specular_txt);
							 auto& ispec = g_mtexture_list.find(specular_txt);
							 if (ispec!=g_mtexture_list.end())
							 {
								 mesh_unit._ps_text_specular_list.emplace_back(ispec->second);
							 }
						 }
						 Value& jheight_list = jmesh_unit["height_list"];
						 jdsize = jheight_list.size();
						 for (int jd = 0; jd < jdsize; ++jd)
						 {
							 auto& height_txt = jheight_list[jd].asString();
							 mesh_unit._text_height_list.emplace_back(height_txt);
							 auto& iheight = g_mtexture_list.find(height_txt);
							 if (iheight!=g_mtexture_list.end())
							 {
								 mesh_unit._ps_text_height_list.emplace_back(iheight->second);
							 }
						 }
						 Value& jambient_list = jmesh_unit["ambient_list"];
						 jdsize = jambient_list.size();
						 for (int jd = 0; jd < jdsize; ++jd)
						 {
							 auto& ambient_txt = jambient_list[jd].asString();
							 mesh_unit._text_ambient_list.emplace_back(ambient_txt);
							 auto& iamb = g_mtexture_list.find(ambient_txt);
							 if (iamb!=g_mtexture_list.end())
							 {
								 mesh_unit._ps_text_ambient_list.emplace_back(iamb->second);
							 }
						 }
						 Value& bdbox = jmesh_unit["bounding_box"];
						 if (!bdbox.isNull())
						 {
							 mesh_unit._box._xmin = bdbox["xmin"].asDouble();
							 mesh_unit._box._xmax = bdbox["xmax"].asDouble();
							 mesh_unit._box._ymin = bdbox["ymin"].asDouble();
							 mesh_unit._box._ymax = bdbox["ymax"].asDouble();
							 mesh_unit._box._zmin = bdbox["zmin"].asDouble();
							 mesh_unit._box._zmax = bdbox["zmax"].asDouble();

						 }
					 }
				 }
			 }
			_root.init_property_from_json(jroot);//
			auto jarry_2_prp_pos = [this](Value& jarry, prop_ele_position&prp_epos){
				int jsize = jarry.size();
				assert(jsize > 1);
				int con_id_max = jsize - 1;
				base_ui_component* pcontrol = &_root;
				int ii = con_id_max;
				for (; ii>1; ii--)
				{
					Value& jcid = jarry[ii];
					auto cid = jcid.asInt();
					pcontrol = pcontrol->get_child(cid);
				}
				prp_epos._pobj = pcontrol;
				prp_epos._page_index = jarry[1].asInt();
				ii = 0;
				prp_epos._field_index = jarry[ii].asInt();
			};
			Value& alias = jroot["aliase"];
			Value::Members amemb(alias.getMemberNames());
			for (auto imemb = amemb.begin(); imemb != amemb.end();++imemb)
			{
				Value& jpepid = alias[*imemb];
				auto ps_pep_pos = make_shared<prop_ele_position>();
				jarry_2_prp_pos(jpepid, *ps_pep_pos);
				g_aliase_dic[*imemb] = ps_pep_pos;
			}
			Value& binds = jroot["binds"];
			Value& jdic = binds["dic"];
			isize = jdic.size();
			for (ix = 0; ix < isize;ix++)
			{
				Value& jprp_ele = jdic[ix];
				Value& jkey = jprp_ele["key"];
				prop_ele_position prp_ele_pos;
				jarry_2_prp_pos(jkey, prp_ele_pos);
				Value& jbind_unit = jprp_ele["bind_unit"];
				auto ps_bind_unit = make_shared<prop_ele_bind_unit>();
				ps_bind_unit->_expression = jbind_unit["expression"].asString();
				auto& vparam = ps_bind_unit->_param_list;
				Value& jvparam = jbind_unit["param_list"];
				int vsize = jvparam.size();
				for (int iv = 0; iv < vsize;iv++)
				{
					Value& jprp_ele_pos = jvparam[iv];
					vparam.emplace_back();
					auto& sub_prp_ele_pos = vparam[iv];
					jarry_2_prp_pos(jprp_ele_pos, sub_prp_ele_pos);
				}
				g_bind_dic[prp_ele_pos] = ps_bind_unit;
			}
			Value& jref_dic = binds["ref_dic"];
			isize = jref_dic.size();
			for (ix = 0; ix < isize; ix++)
			{
				Value& jref_unit = jref_dic[ix];
				Value& jref_key = jref_unit["key"];
				prop_ele_position prp_ele_pos;
				jarry_2_prp_pos(jref_key, prp_ele_pos);
				auto ps_ref_list = make_shared<vprop_pos>();
				Value& jref_list = jref_unit["ref_list"];
				int vsize = jref_list.size();
				for (int iv = 0; iv < vsize;iv++)
				{
					Value& jref = jref_list[iv];
					ps_ref_list->emplace_back();
					auto& sub_prp_ele_pos = (*ps_ref_list)[iv];
					jarry_2_prp_pos(jref, sub_prp_ele_pos);
				}
				g_bind_ref_dic[prp_ele_pos] = ps_ref_list;
			}
			Value& jvstate_manager = jroot["state_manager_list"];
			Value::Members memb(jvstate_manager.getMemberNames());
			for (auto item = memb.begin(); item != memb.end();++item)
			{
				auto& mname = *item;
				Value& jstm = jvstate_manager[mname];
				auto ps_stm = make_shared<af_state_manager>();
				auto& stm = *ps_stm;
				auto& any_2_any = stm._any_to_any;
				Value& jany = jstm["any_to_any"];
				any_2_any._start_time = jany["start_time"].asInt();
				any_2_any._easing_func = jany["easing_fun"].asInt();
				any_2_any._duration = jany["duration"].asInt();
				stm._mstate = (moving_state)jstm["mstate"].asInt();
				auto& mtrans = stm._mtrans;
				Value& jmtrans = jstm["mtrans"];
				int jsz = jmtrans.size();
				for (int ii = 0; ii < jsz;ii++)
				{
					Value&jtrans = jmtrans[ii];
					trans_key ikey = { jtrans["key_from"].asInt(), jtrans["key_to"].asInt() };
					auto ps_trans = make_shared<state_transition>();
					ps_trans->_start_time = jtrans["start_time"].asInt();
					ps_trans->_duration = jtrans["duration"].asInt();
					ps_trans->_easing_func = jtrans["easing_fun"].asInt();
					mtrans[ikey] = ps_trans;
				}
				auto& prp_list = stm._prop_list;
				Value& jprop_list = jstm["prop_list"];
				jsz = jprop_list.size();
				for (int ii = 0; ii < jsz;ii++)
				{
					prp_list.emplace_back();
					auto& prp_pos = prp_list[ii];
					Value& jprp_pos = jprop_list[ii];
					jarry_2_prp_pos(jprp_pos, prp_pos);
				}
				auto& prp_value_list = stm._prop_value_list;
				Value& jprop_value_list = jstm["prop_value_list"];
				jsz = jprop_value_list.size();
				for (int ii = 0; ii < jsz;ii++)
				{
					prp_value_list.emplace_back();
					auto& vprp_block = prp_value_list[ii];
					Value& jvprp_blck = jprop_value_list[ii];
					int jjsz = jvprp_blck.size();
					for (int ix = 0; ix < jjsz;ix++)
					{
						Value& jbk_value = jvprp_blck[ix];
						vprp_block.emplace_back();
						auto& prp_blcok = vprp_block[ix];
						string str_value = jbk_value.asString();
						convert_string_to_binary(str_value, prp_blcok);
					}
				}
                    auto& playlist_list = stm._playlist_list;
                    Value& jplaylist_list = jstm[ "playlist_list" ];
                    jsz = jplaylist_list.size();
                    for( int ii = 0; ii < jsz;ii++ )
                    {
                         playlist_list.emplace_back();
                         auto& playlist = playlist_list[ ii ];
                         Value& jplaylist = jplaylist_list[ ii ];
                         int jjsz = jplaylist.size();
                         for( int ix = 0; ix < jjsz;ix++ )
                         {
                              playlist.emplace_back();
                              auto& tran_unit = playlist[ ix ];
                              Value& jtran = jplaylist[ ix ];
                              tran_unit._from = jtran[ "key_from" ].asInt();
                              tran_unit._to = jtran[ "key_to" ].asInt();
                         }
                    }
				g_mstate_manager[mname] = ps_stm;
			}
               Value& jcommonvalue_list = jroot[ "common_value_list" ];
               Value::Members jmemb( jcommonvalue_list.getMemberNames() );
               for( auto item = jmemb.begin(); item != jmemb.end(); ++item )
               {
                    auto& mname = *item;
                    Value& jcmv = jcommonvalue_list[ mname ];
                    Value& jvalue = jcmv[ "value" ];
                    Value& jprop_list = jcmv[ "prop_list" ];
                    string vtype = jcmv[ "type" ].asString();
                    auto pcmv = make_shared<base_prp_type>( vtype );
                    auto& mvalue = pcmv->_pbase;
                    auto& pmlist = pcmv->_param_list;
                    if (vtype=="int")
                    {
                         *(int*)mvalue = jvalue.asInt();
                    }
                    else if (vtype=="float"||vtype=="double")
                    {
                         *(float*)mvalue = jvalue.asDouble();
                    }
                    else if (vtype=="af_vec2")
                    {
                         *(float*)mvalue = jvalue[ "x" ].asDouble();
                         *( (float*)mvalue+1 ) = jvalue[ "y" ].asDouble();
                    }
                    else if (vtype=="af_vec3")
                    {
                         *(float*)mvalue = jvalue[ "x" ].asDouble();
                         *( (float*)mvalue+1 ) = jvalue[ "y" ].asDouble();
                         *( (float*)mvalue + 2 ) = jvalue[ "z" ].asDouble();
                    }
                    else if( vtype == "af_vec4" )
                    {
                         *(float*)mvalue = jvalue[ "x" ].asDouble();
                         *( (float*)mvalue + 1 ) = jvalue[ "y" ].asDouble();
                         *( (float*)mvalue + 2 ) = jvalue[ "z" ].asDouble();
                         *( (float*)mvalue + 3 ) = jvalue[ "w" ].asDouble();
                    }
                    int jsz = jprop_list.size();
                    for( int ii = 0; ii < jsz;ii++ )
                    {
                         pmlist.emplace_back();
                         auto& prp_pos = pmlist[ ii ];
                         Value& jprp_pos = jprop_list[ ii ];
                         jarry_2_prp_pos( jprp_pos, prp_pos );
                    }
                    g_base_prp_dic[ mname ] = pcmv;
               }
			   Value& feedback = jroot["feedback"];
			   auto ifbsz = feedback.size();
			   for (int ix = 0; ix < ifbsz;ix++)
			   {
				   Value fbunit = feedback[ix];
				   auto& mtl_key = fbunit["mtl_key"].asString();
				   auto& prm_key = fbunit["prm_key"].asString();
				   const auto& imtl = g_material_list.find(mtl_key);
				   if (imtl==g_material_list.end())
				   {
					   printf("invalid material name:%s for the current feedback\n", mtl_key.c_str());
					   continue;
				   }
				   const auto& iprm = g_primitive_list.find(prm_key);
				   if (iprm==g_primitive_list.end())
				   {
					   printf("invalid primitive name:%s for the current feedback\n", prm_key.c_str());
					   continue;
				   }
				   feedback_key fkey = { mtl_key, prm_key };
				   g_feedback_list[fkey] = make_shared<af_feedback>(imtl->second, iprm->second);

			   }
		}
		fin.close();
	}
	else
	{
		printf("invalid file_path:%s\n", file_path);
		//return false;
	}
	SetCursor(hcur_cursor);

	return true;
}

bool ui_assembler::output_ui_component_to_file(const char* file_path)
{
	HCURSOR hcur_cursor = GetCursor();
	SetCursor(g_hcursor_wait);
	ofstream fout;
	fout.open(file_path);
	Value jroot(objectValue);
	jroot["screenw"] = base_ui_component::screenw;
	jroot["screenh"] = base_ui_component::screenh;
	Value window_show(objectValue);
	window_show["show_project_window"] = show_project_window;
	window_show["show_edit_window"] = show_edit_window;
	window_show["show_property_window"] = show_property_window;
	window_show["show_resource_manager"] = show_resource_manager;
	window_show["show_fonts_manager"] = show_fonts_manager;
	window_show["show_file_manager"] = show_file_manager;
	window_show["show_state_manager_edit"] = show_state_manager_edit;
	window_show["show_aliase_edit"] = show_aliase_edit;
	window_show["show_slider_path_picker"] = show_slider_path_picker;

	jroot["window_show"] = window_show;
     jroot[ "afb_output_path" ] = g_afb_output_path;
	Value fonts(arrayValue);
	//vfont_face_name& ft_nm_list = g_pfont_face_manager->get_font_name_list();
	auto& dic_fonts= g_pfont_face_manager->get_dic_fonts();
	for (auto& font_item : dic_fonts)
	{
		Value jfont(objectValue);
		jfont["name"] = font_item->_name;
		jfont["cols"] = font_item->_char_count_c;
		jfont["rows"]=font_item->_char_count_r;
		fonts.append(jfont);
	}
	jroot["fonts"] = fonts;
	Value output_bin_fmt(objectValue);
	output_bin_fmt["txt_fmt"] = g_output_bin_format._txt_fmt;
	output_bin_fmt["pgm_fmt"] = g_output_bin_format._pgm_fmt;
	jroot["output_bin_fmt"] = output_bin_fmt;
	Value jtexture(arrayValue);
	for (auto& reslist:g_vres_texture_list)
	{
		Value jtext_res_unit(objectValue);
		jtext_res_unit["texture_pack_file"] = reslist.texture_pack_file;
		jtext_res_unit["texture_data_file"] = reslist.texture_data_file;
		jtext_res_unit["separated"] = reslist._is_separated;
		jtexture.append(jtext_res_unit);
	}
	jroot["texture_res_list"] = jtexture;
	Value texture_list(arrayValue);
	for (auto& txt : g_mtexture_list)
	{
		auto& kname = txt.first;
		Value txt_unit(objectValue);
		txt_unit["name"] = kname;
		txt_unit["separated"] = txt.second->_is_separated;
		txt_unit["mipmap"] = txt.second->_mip_map;
		//auto& af_txt = txt.second;
		texture_list.append(txt_unit);
	}
	jroot["texture_list"] = texture_list;
	Value file_list(arrayValue);
	string str_files_path = g_cureent_directory + files_fold;
	for (auto& fileu : g_mfiles_list)
	{
		auto& kname = fileu.first;
		Value jfile(kname);
		file_list.append(jfile);
		string file_full_path = str_files_path + kname;
		//if (!fileExist(file_full_path.c_str()))
		{
			auto& fobj = *fileu.second;
			ofstream fout;// (file_full_path);
			fout.open(file_full_path, ios::binary);
			fout.write((char*)fobj._pbin, fobj._fsize);
			fout.close();
		}
	}
	jroot["file_list"] = file_list;
	Value jshader(arrayValue);
	string str_shader_path = g_cureent_directory + shaders_fold;
	for (auto& shd_ut : g_af_shader_list)
	{
		Value jshd_unit(objectValue);
		auto& sd_name = shd_ut.first;
		auto& p_sd = shd_ut.second;
		jshd_unit["name"] = sd_name;
		jshd_unit["vs_name"] = p_sd->_vs_name;
		jshd_unit["fs_name"] = p_sd->_fs_name;
		string vs_file = str_shader_path + p_sd->_vs_name;
		string fs_file = str_shader_path + p_sd->_fs_name;
		string& vs_code=p_sd->get_vs_code();
		string& fs_code = p_sd->get_fs_code();
		ofstream ofs_shd;
		ofs_shd.open(vs_file, ios::out);
		ofs_shd.write(vs_code.c_str(), vs_code.size());
		ofs_shd.close();
		ofs_shd.open(fs_file, ios::out);
		ofs_shd.write(fs_code.c_str(), fs_code.size());
		ofs_shd.close();
		jshader.append(jshd_unit);
	}
	jroot["shader_list"] = jshader;
	Value jmaterial(arrayValue);
	for (auto& mtl_ut : g_material_list)
	{
		Value jmtl_ut(objectValue);
		auto& pmtl = mtl_ut.second;
		pmtl->output_2_json(jmtl_ut);
		jmaterial.append(jmtl_ut);
	}
	jroot["material_list"] = jmaterial;
	Value jprimitive_list(arrayValue);
	for (auto& pm:g_primitive_list)
	{
		Value jpm(objectValue);
		jpm["name"] = pm.first; 
		
		auto& pmu = pm.second;	
		Value jformat(arrayValue);
		for (auto& fmu:pmu->_ele_format)
		{
			jformat.append(fmu);
		}
		jpm["format"] = jformat;
		jpm["vbo_len"] = pmu->_vertex_buf_len;
		jpm["ebo_len"] = pmu->_ele_buf_len;
		jpm["model"] = pmu->_model_name;
		jpm["mesh_id"] = pmu->_mesh_id;
		jprimitive_list.append(jpm);
	}
	jroot["primitive_list"] = jprimitive_list;

	jroot["texture_id_index"] = g_cur_texture_id_index;
	Value jmodels(objectValue);

	for (auto& model_unit : g_mmodel_list)
	{
		Value jmesh_list(arrayValue);
		auto& mesh_list = *model_unit.second;
		for (auto& mesh_unit : mesh_list)
		{
			Value jmesh_unit(objectValue);
			jmesh_unit["prim_id"] = mesh_unit._prm_id;
			Value diffuse_list(arrayValue);
			for (auto& diffuse : mesh_unit._text_diffuse_list)
			{
				diffuse_list.append(diffuse);
			}
			jmesh_unit["diffuse_list"] = diffuse_list;
			Value spcular_list(arrayValue);
			for (auto& specular : mesh_unit._text_specular_list)
			{
				spcular_list.append(specular);
			}
			jmesh_unit["specular_list"] = spcular_list;
			Value height_list(arrayValue);
			for (auto& height : mesh_unit._text_height_list)
			{
				height_list.append(height);
			}
			jmesh_unit["height_list"] = height_list;
			Value ambient_list(arrayValue);
			for (auto& ambient : mesh_unit._text_ambient_list)
			{
				ambient_list.append(ambient);
			}
			jmesh_unit["ambient_list"] = ambient_list;
			Value jbdbox(objectValue);
			jbdbox["xmin"] = mesh_unit._box._xmin;
			jbdbox["xmax"] = mesh_unit._box._xmax;
			jbdbox["ymin"] = mesh_unit._box._ymin;
			jbdbox["ymax"] = mesh_unit._box._ymax;
			jbdbox["zmin"] = mesh_unit._box._zmin;
			jbdbox["zmax"] = mesh_unit._box._zmax;
			jmesh_unit["bounding_box"] = jbdbox;
			jmesh_list.append(jmesh_unit);
		}
		jmodels[model_unit.first] = jmesh_list;
	}
	jroot["models"] = jmodels;
	_root.save_property_to_json(jroot);
	Value aliase(objectValue);
	for (auto& ialias:g_aliase_dic)
	{
		auto& ikey = ialias.first;
		auto& ipos = *ialias.second;
		prop_ele_pos_index pep_id;
		calcu_prop_ele_pos_index(ipos, pep_id);
		Value jpos(arrayValue);
		for (auto pos_id:pep_id)
		{
			jpos.append(pos_id);
		}
		aliase[ikey] = jpos;
	}
	jroot["aliase"] = aliase;
	Value binds(objectValue);
	Value dic(arrayValue);
	for (auto& idic:g_bind_dic)
	{
		Value dic_unit(objectValue);
		Value dic_key(arrayValue);
		prop_ele_pos_index pep_id;
		auto& ikey = idic.first;
		calcu_prop_ele_pos_index(ikey, pep_id);
		for (auto keyid:pep_id)
		{
			dic_key.append(keyid);
		}
		dic_unit["key"] = dic_key;
		auto& bind_unit = *idic.second;
		Value jbind_unit(objectValue);
		Value param_list(arrayValue);
		for (auto& pele_pos : bind_unit._param_list)
		{
			prop_ele_pos_index sub_pep_id;
			calcu_prop_ele_pos_index(pele_pos, sub_pep_id);
			Value param(arrayValue);
			for (auto keyid:sub_pep_id)
			{
				param.append(keyid);
			}
			param_list.append(param);
		}
		jbind_unit["param_list"] = param_list;
		jbind_unit["expression"] = bind_unit._expression;
		dic_unit["bind_unit"] = jbind_unit;
		dic.append(dic_unit);
	}
	binds["dic"] = dic;
	Value ref_dic(arrayValue);
	for (auto& idic:g_bind_ref_dic)
	{
		Value dic_unit(objectValue);
		Value dic_key(arrayValue);
		prop_ele_pos_index pep_id;
		auto& ikey = idic.first;
		calcu_prop_ele_pos_index(ikey, pep_id);
		for (auto keyid:pep_id)
		{
			dic_key.append(keyid);
		}
		dic_unit["key"] = dic_key;
		auto&ref_bind_unit = *idic.second;
		Value ref_list(arrayValue);
		for (auto& iref : ref_bind_unit)
		{
			prop_ele_pos_index sub_pep_id;
			calcu_prop_ele_pos_index(iref, sub_pep_id);
			Value param(arrayValue);
			for (auto keyid:sub_pep_id)
			{
				param.append(keyid);
			}
			ref_list.append(param);
		}
		dic_unit["ref_list"] = ref_list;
		ref_dic.append(dic_unit);
	}
	binds["ref_dic"] = ref_dic;
	jroot["binds"] = binds;
	Value state_manager(objectValue);
	for (auto& ism:g_mstate_manager)
	{
		Value st_m_unit(objectValue);
		auto& stm_unit = *ism.second;
		auto& prop_list = stm_unit._prop_list;
		Value jprop_list(arrayValue);
		for (auto& pp_unit:prop_list)
		{
			prop_ele_pos_index prp_id;
			calcu_prop_ele_pos_index(pp_unit, prp_id);
			Value jprp_id(arrayValue);
			for (auto keyid:prp_id)
			{
				jprp_id.append(keyid);
			}
			jprop_list.append(jprp_id);
		}
		st_m_unit["prop_list"] = jprop_list;
		auto& prop_value_list = stm_unit._prop_value_list;
		Value jprop_value_list(arrayValue);
		for (auto& vprp_value:prop_value_list)
		{
			Value jprp_value(arrayValue);
			for (auto& prp_value:vprp_value )
			{
				string prp_value_value;
				convert_binary_to_string(&prp_value[0],prp_value.size(),prp_value_value);
				jprp_value.append(prp_value_value);
			}
			jprop_value_list.append(jprp_value);
		}
		st_m_unit["prop_value_list"] = jprop_value_list;
		auto& any_to_any = stm_unit._any_to_any;
		Value jany(objectValue);
		jany["start_time"] = any_to_any._start_time;
		jany["duration"] = any_to_any._duration;
		jany["easing_fun"] = any_to_any._easing_func;
		st_m_unit["any_to_any"] = jany;
		auto& mtrans = stm_unit._mtrans;
		Value jmstrans(arrayValue);
		for (auto& itran:mtrans)
		{
			Value jtran(objectValue);
			jtran["key_from"] = itran.first._from;
			jtran["key_to"] = itran.first._to;
			auto& tran = *itran.second;
			jtran["start_time"] = tran._start_time;
			jtran["duration"] = tran._duration;
			jtran["easing_fun"] = tran._easing_func;
			jmstrans.append(jtran);
		}
		st_m_unit["mtrans"] = jmstrans;
		auto& state_idx = stm_unit._state_idx;
		st_m_unit["state_idx"] = state_idx;
		auto& mstate = stm_unit._mstate;
		st_m_unit["mstate"] = mstate;
          auto& playlist_list = stm_unit._playlist_list;
          Value jplaylist_list( arrayValue );
          for (auto& iplaylist:playlist_list)
          {
               Value jplaylist( arrayValue );
               for (auto& itran:iplaylist)
               {
                    Value jtran( objectValue );
                    jtran[ "key_from" ] = itran._from;
                    jtran[ "key_to" ] = itran._to;
                    jplaylist.append( jtran );
               }
               jplaylist_list.append( jplaylist );
          }
          st_m_unit[ "playlist_list" ] = jplaylist_list;
		state_manager[ism.first] = st_m_unit;
	}
	jroot["state_manager_list"] = state_manager;
     Value common_value_list(objectValue);
     for (auto& ivc:g_base_prp_dic)
     {
          Value uvalue( objectValue );
          auto& knm = ivc.first;
          auto& cmvalue = *ivc.second;
          auto& mvalue = cmvalue._pbase;
          auto& pmlist = cmvalue._param_list;
          auto& vtype = cmvalue._type;
          uvalue[ "type" ] = vtype;
          Value jprop_list( arrayValue );
          for( auto& pp_unit : pmlist )
          {
               prop_ele_pos_index prp_id;
               calcu_prop_ele_pos_index( pp_unit, prp_id );
               Value jprp_id( arrayValue );
               for( auto keyid : prp_id )
               {
                    jprp_id.append( keyid );
               }
               jprop_list.append( jprp_id );
          }
          uvalue[ "prop_list" ] = jprop_list;
          if (vtype=="int")
          {
               int* pvi = (int*)mvalue;
               uvalue[ "value" ] = *pvi;
          }
          else if( vtype == "float" || vtype=="double")
          {
               uvalue[ "value" ] = *(float*)mvalue;
          }
          else if( vtype == "bool" )
          {
               uvalue[ "value" ] = *(bool*)mvalue;
          }
          else if (vtype=="af_vec2")
          {
               Value jv2( objectValue );
               jv2["x"] = *(float*)mvalue;
               jv2[ "y" ] = *((float*)mvalue+1);
               uvalue[ "value" ] = jv2;
          }
          else if( vtype == "af_vec3" )
          {
               Value jv3( objectValue );
               jv3["x"]=*(float*)mvalue;
               jv3["y"]=*((float*)mvalue+1);
               jv3["z"]=*((float*)mvalue+2);
               uvalue[ "value" ] = jv3;
          }
          else if (vtype=="af_vec4")
          {
               Value jv4( objectValue );
               jv4[ "x" ] = *(float*)mvalue;
               jv4[ "y" ] = *( (float*)mvalue + 1 );
               jv4[ "z" ] = *( (float*)mvalue + 2 );
               jv4[ "w" ] = *( (float*)mvalue + 3 );
               uvalue[ "value" ] = jv4;
          }
          common_value_list[ knm ] = uvalue;
     }
     jroot[ "common_value_list" ] = common_value_list;
	 Value feedback(arrayValue);
	 for (auto& ifb:g_feedback_list)
	 {
		 auto& mtl_key = ifb.first._mtl_key;
		 auto& prm_key = ifb.first._prm_key;
		 Value fbkey(objectValue);
		 fbkey["mtl_key"] = mtl_key;
		 fbkey["prm_key"] = prm_key;
		 feedback.append(fbkey);
	 }
	 jroot["feedback"] = feedback;
	fout << jroot << endl;
	fout.close();
	//save_fbx_file();
	SetCursor(hcur_cursor);
	return true;
}
int real_id_after_update(string& file_name)
{
	res_texture_list& cur_txt_list = g_vres_texture_list[g_cur_texture_id_index];
	vres_txt_cd& cur_txt_cd = cur_txt_list.vtexture_coordinates;
	for (int ix = 0; ix < cur_txt_cd.size();++ix)
	{
		if (file_name==cur_txt_cd[ix]._file_name)
		{
			return ix;
		}
	}
	return 0;
}
void update_ui_component_texture_res_index(base_ui_component& tar, vres_txt_cd& old_txt_cd)
{
	vp_prop_ele& cur_vprop_eles = tar.get_prop_ele();
	for (auto& prop_ele:cur_vprop_eles)
	{
		auto& prop_page = prop_ele->_pro_page;
		for (auto& memb:prop_page)
		{
			auto mtype = memb->_type;
			auto mname = memb->_name;
			auto moffset = memb->_offset;
			string rg = mname.substr(mname.length() - 3, 3);
			if (mtype=="int"&&rg=="txt")
			{
				void* memb_address = (char*)prop_ele->_pro_address + moffset;
			    int* ptxtidx = (int*)memb_address;
				int ix_old = *ptxtidx;
				*ptxtidx = real_id_after_update(old_txt_cd[ix_old]._file_name);
			}
		}
	}
	int chldcnt = tar.child_count();
	for (size_t ix = 0; ix < chldcnt;++ix)
	{
		base_ui_component* pchild = tar.get_child(ix);
		update_ui_component_texture_res_index(*pchild, old_txt_cd);
	}

}
bool ui_assembler::update_texture_res()
{
	res_texture_list& cur_txt_list=g_vres_texture_list[g_cur_texture_id_index];
	vres_txt_cd cur_txt_cd = cur_txt_list.vtexture_coordinates;
	unsigned int txtid = cur_txt_list.texture_id();
	glDeleteTextures(1, &txtid);
	cur_txt_list.txt_id = 0;
	cur_txt_list.vtexture_coordinates.clear();
	load_texture_info(cur_txt_list, cur_txt_list.texture_pack_file, cur_txt_list.texture_data_file);
	int chldcnt = _root.child_count();
	for (size_t ix = 0; ix < chldcnt; ++ix)
	{
		base_ui_component* pchild = _root.get_child(ix);
		update_ui_component_texture_res_index(*pchild, cur_txt_cd);
	}
	return true;
}

ps_primrive_object ui_assembler::load_primitive_from_file(string &kname, vector<GLubyte> ele_format, GLuint vbo_len, GLuint ebo_len)
{
	GLfloat* pvbo = 0;
	GLuint* pebo = 0;
	auto ifile = g_mfiles_list.find(kname);
	if (ifile!=g_mfiles_list.end())
	{
		auto& pfobj = ifile->second;
		auto& fopj = *pfobj;
		char* phead = (char*)fopj._pbin;
		GLuint* phead_len = (GLuint*)phead;
		phead += 4;
		pvbo = (GLfloat*)phead;
		if (ebo_len)
		{
			pebo = (GLuint*)(phead + *phead_len);
		}

	}
	else
	{
		return nullptr;
	}
	
	ps_primrive_object ppbj = make_shared<primitive_object>();
	ppbj->set_ele_format(ele_format);
	ppbj->load_vertex_data(pvbo, vbo_len, pebo, ebo_len);
	ppbj->_ps_file = ifile->second;
	return ppbj;
	
}