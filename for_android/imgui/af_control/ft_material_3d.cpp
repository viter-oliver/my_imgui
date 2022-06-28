#include "ft_material_3d.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ft_trans.h"
namespace zl_future
{
	ft_material_3d::ft_material_3d()
	{
		strcpy(_pt._uf_model_name, "model");
		strcpy(_pt._uf_view_name, "view");
		strcpy(_pt._uf_proj_name, "projection");
          _pt._cam = { { -400, 400, 0 }, { -400, 400, 0 }, { 0, 1, 0 } };
          _pt._pj = { 20, 10, 10000 };
#if !defined(DISABLE_DEMO)
		_pt._primitive_name[0] = '\0';
		_pt._material_name[0] = '\0';

		reg_property_handle(&_pt, 0, [this](void* memb_adress)
		{
			ImGui::Combo("Draw mode:", &_pt._draw_mode, draw_mode, en_gl_count);
		});
		reg_property_handle(&_pt, 1, [this](void* memb_adress)
		{
			if (_ps_prm)
			{
				ImGui::Text("Primitive object:%s", _pt._primitive_name);
				ImGui::SameLine();
				if (ImGui::Button("Delink##prm_obj"))
				{
					_ps_prm = nullptr;
					_matched = false;
				}
			}
			else
			{
				ImGui::InputText("Primitive object:", _pt._primitive_name, FILE_NAME_LEN);
				if (ImGui::Button("Link##prm"))
				{
					auto& iprm = g_primitive_list.find(_pt._primitive_name);
					if (iprm!=g_primitive_list.end())
					{
						_ps_prm = iprm->second;
						const auto& imdl = g_mmodel_list.find(_ps_prm->_model_name);
						if (imdl!=g_mmodel_list.end())
						{
							auto& mdl = *imdl->second;
							auto mesh_size = mdl.size();
							if (_ps_prm->_mesh_id<mesh_size)
							{
								auto& mesh_tar = mdl[_ps_prm->_mesh_id];
								auto& tbox = mesh_tar._box;
								af_vec3 pt_core = { (tbox._xmax - tbox._xmin)*0.5, \
									(tbox._ymax - tbox._ymin)*0.5, (tbox._zmax - tbox._zmin)*0.5 };
								_pt._cam._position = pt_core;
								_pt._cam._position.z = 5 * pt_core.z;
								_pt._cam._direction = _pt._cam._position - pt_core;
								_pt._cam._up = { 0.f, 1.f, 0.f };
							}
						}
					}
					else
					{
						string str_prm_nm(_pt._primitive_name);
						if (ref_a_intenal_primitive(str_prm_nm))
						{
							_ps_prm = g_primitive_list[str_prm_nm];
						}
					}
				}
			}
			if (_pt._with_feedback)
			{
				if (ImGui::Button("Show feedback"))
				{
					get_output_vertex(_output_buff);
					ImGui::OpenPopup("ShowFeedback");
				}
				if (ImGui::BeginPopupModal("ShowFeedback"))
				{
					auto stride = 3;// _ps_prm->get_stride();
					auto osz = _output_buff.size();
					for (int ix = 0; ix < osz; ix++)
					{
						ImGui::Text("%f", _output_buff[ix]);
						auto imd = ix%stride;
						if (imd != (stride - 1))
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
		});
		reg_property_handle(&_pt, 2, [this](void* memb_adress)
		{
			if (_ps_mtl)
			{
				ImGui::Text("Material name:%s", _pt._material_name);
				ImGui::SameLine();
				if (ImGui::Button("Delink##material"))
				{
					_ps_mtl = nullptr;
					_matched = false;
				}
			}
			else
			{
				ImGui::InputText("Material name:", _pt._material_name, FILE_NAME_LEN);
				if (ImGui::Button("Link##mtl"))
				{
					auto& imtl = g_material_list.find(_pt._material_name);
					if (imtl!=g_material_list.end())
					{
						_ps_mtl = imtl->second;
					}
				}
			}
			if (!_matched)
			{
				ImGui::Text("material is unmatched to primitive object");
			}
			
		});
          reg_property_handle( &_pt, 8, [this]( void* memb_adress )
          {
               ImGui::Combo( "trans order:", &_pt._trans_order, str_trans_order, en_trans_order_cnt );
          } );
           
          reg_property_handle( &_pt, 15, [this]( void* memb_adress )
          {
               ImGui::Combo( "rotate order:", &_pt._rotate_order, str_rotate_oder, en_rotate_order_cnt );
          } );

#endif
	}
	ft_material_3d::~ft_material_3d()
	{
		if (_gpu_outbuff!=0)
		{
			glDeleteBuffers(1, &_gpu_outbuff);
			_gpu_outbuff = 0;
		}
	}
	void ft_material_3d::link()
	{
		int imatch = 0;
		auto imt=g_material_list.find(_pt._material_name);
		if (imt!=g_material_list.end())
		{
			_ps_mtl = imt->second;
			imatch++;
		}
		else
		{
			LOGE("material %s is mismathced\n",_pt._material_name);
			for(auto& imtl:g_material_list)
			{
				LOGE("mtl:%s\n",imtl.first.c_str());
			}
		}
		auto iprm = g_primitive_list.find(_pt._primitive_name);
		if (iprm!=g_primitive_list.end())
		{
			_ps_prm = iprm->second;
			if (_pt._with_feedback)
			{
				glGenBuffers(1, &_gpu_outbuff);
				glBindBuffer(GL_ARRAY_BUFFER, _gpu_outbuff);
				auto buff_sz = _ps_prm->_vertex_buf_len*sizeof(GLfloat);
				glBufferData(GL_ARRAY_BUFFER, buff_sz, nullptr, GL_STATIC_DRAW);
			}
			imatch++;
		}
		else
		{
			LOGE("primative %s is mismathced\n",_pt._primitive_name);
			for(auto& ipm:g_primitive_list)
			{
				LOGE("mtl:%s\n",ipm.first.c_str());
			}
		}
		_matched = imatch==2;
	}
	
	bool ft_material_3d::get_output_vertex(vector<float>& overtex)
	{
		if (!_pt._with_feedback)
		{
			return false;
		}
		overtex.resize(_ps_prm->_vertex_buf_len);
		auto bfsz= _ps_prm->_vertex_buf_len*sizeof(GLfloat);
		#ifdef INCLUDE_ES3
		//float* pbuf=(float*)glMapBufferRangeEXT(GL_ARRAY_BUFFER,0,bfsz,GL_MAP_READ_BIT);
		//memcpy(&overtex[0],pbuf,bfsz);
	//	glUnmapBuffer(GL_ARRAY_BUFFER);
		#else
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, bfsz, &overtex[0]);
		#endif
		return true;
	}
	
	void ft_material_3d::draw()
	{
#if 1//!defined(DISABLE_DEMO)

		if (_ps_mtl&&_ps_mtl->is_valid()&&_ps_prm)
		{
			auto& ps_sd = _ps_mtl->get_shader();
			_matched = ps_sd->match_format(_ps_prm->_ele_format);
		}
		else
		{
			_matched = false;
		}
#endif
		if (_matched)
		{
			if (!_ps_mtl->is_valid()||!_ps_prm)
			{
				return;
			}
			static GLuint draw_model[en_gl_count] =
			{
				GL_POINTS,
				GL_LINES,
				GL_LINE_LOOP,
				GL_LINE_STRIP,
				GL_TRIANGLES,
				GL_TRIANGLE_STRIP,
				GL_TRIANGLE_FAN,
			};			
			glm::mat4 model;
			//auto& aftr = _pt._trans._translation;
			//auto& afsc = _pt._trans._scale;
			//auto& afrt = _pt._trans._rotation;
               for (auto& item:_vchilds)
               {
                    ft_trans* ptrans_item = (ft_trans*) item;
                    ptrans_item->transform( model );
               }
               glm::vec3 gtranslate( _pt._trans_translation_x, _pt._trans_translation_y, _pt._trans_translation_z );
               glm::vec3 gscale( _pt._trans_scale_x, _pt._trans_scale_y, _pt._trans_scale_z );
               function<void()> f_rotate[ en_rotate_order_cnt ]=
               {
                    [&]( )
                    {
                         model = glm::rotate( model, _pt._trans_rotation_x*glm::radians( 1.f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_y*glm::radians( 1.f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_z*glm::radians( 1.f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
                    },
                    [&]()
                    {
                         model = glm::rotate( model, _pt._trans_rotation_x*glm::radians( 1.f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_z*glm::radians( 1.f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_y*glm::radians( 1.f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
                    },
                    [&]()
                    {
                         model = glm::rotate( model, _pt._trans_rotation_y*glm::radians( 1.f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_x*glm::radians( 1.f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_z*glm::radians( 1.f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
                    },
                    [&]()
                    {
                         model = glm::rotate( model, _pt._trans_rotation_y*glm::radians( 1.f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_z*glm::radians( 1.f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_x*glm::radians( 1.f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
                    },
                    [&]()
                    {
                         model = glm::rotate( model, _pt._trans_rotation_z*glm::radians( 1.f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_x*glm::radians( 1.f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_y*glm::radians( 1.f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
                    },
                    [&]()
                    {
                         model = glm::rotate( model, _pt._trans_rotation_z*glm::radians( 1.f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_y*glm::radians( 1.f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
                         model = glm::rotate( model, _pt._trans_rotation_x*glm::radians( 1.f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
                    },               
               };
               function<void()> f_trans[ en_trans_order_cnt ] =
               {
                    [&]()
                    {
                         f_rotate[ _pt._rotate_order ]();
			          model = glm::scale(model, gscale);
                         model = glm::translate(model, gtranslate);
                    },
                    [&]()
                    {
                         model = glm::scale( model, gscale );
                         f_rotate[ _pt._rotate_order ]();
                         model = glm::translate( model, gtranslate );
                    }, 
                    [&]()
                    {
                         f_rotate[ _pt._rotate_order ]();
                         model = glm::translate( model, gtranslate );
                         model = glm::scale( model, gscale );
                    },
                    [&]()
                    {
                         model = glm::translate( model, gtranslate );
                         f_rotate[ _pt._rotate_order ]();
                         model = glm::scale( model, gscale );
                    },
                    [&]()
                    {
                         model = glm::scale( model, gscale );
                         model = glm::translate( model, gtranslate );
                         f_rotate[ _pt._rotate_order ]();
                    },
                    [&]()
                    {
                         model = glm::translate( model, gtranslate );
                         model = glm::scale( model, gscale );
                         f_rotate[ _pt._rotate_order ]();
                    },
               };
               f_trans[ _pt._trans_order ]();
			glm::mat4 view;
			const auto& cam_pos = _pt._cam._position;
			const auto& cam_dir = _pt._cam._direction;
			const auto& cam_up = _pt._cam._up;
			glm::vec3 gcam_pos(cam_pos.x, cam_pos.y, cam_pos.z);
			glm::vec3 gcam_dir(cam_dir.x, cam_dir.y, cam_dir.z);
			glm::vec3 gcam_up(cam_up.x, cam_up.y, cam_up.z);
			view = glm::lookAt(gcam_pos, gcam_dir, gcam_up);
			glm::mat4 proj;
			const auto& cproj = _pt._pj;
			const auto pparent = get_parent();
			float w, h;
			pparent->get_size(w, h);
			float aspect = w / h;
			proj = glm::perspective(glm::radians(cproj._fovy), aspect, cproj._near, cproj._far);
			_ps_mtl->set_value(string(_pt._uf_model_name), glm::value_ptr(model), 16);
			_ps_mtl->set_value(string(_pt._uf_view_name), glm::value_ptr(view), 16);
			_ps_mtl->set_value(string(_pt._uf_proj_name), glm::value_ptr(proj), 16);
 			_ps_mtl->set_value( string("alpha"), &_pt._alpha_nml, 1 );
			_ps_mtl->use();
			auto& primid = *_ps_prm;
			
			glBindVertexArray(primid._vao);
			primid.enableVertex();
			GLuint& dml = draw_model[_pt._draw_mode];
			if (_pt._with_feedback)
			{
				if (_gpu_outbuff == 0)
				{
					glGenBuffers(1, &_gpu_outbuff);
					glBindBuffer(GL_ARRAY_BUFFER, _gpu_outbuff);
					auto buff_sz = _ps_prm->_vertex_buf_len*sizeof(GLfloat);
					glBufferData(GL_ARRAY_BUFFER, buff_sz, nullptr, GL_STATIC_DRAW);
				}
				//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _gpu_outbuff);
				//glBeginTransformFeedback(dml);
			}
			if (primid._ele_buf_len==0)
			{
				glDrawArrays(dml, 0, primid._vertex_buf_len);
			}
			else
			{
			       //LOGE("elelen=%d\n",primid._ele_buf_len);
				glDrawElements(dml, primid._ele_buf_len, GL_UNSIGNED_INT, 0);
			}
			if (_pt._with_feedback)
			{
				//glEndTransformFeedback();
				glFlush();
			}
		}
		else
		{
			static int timer=0;
			timer++;
			if(timer>120)
			{
				timer=0;
				LOGE("mismathced\n");
				auto& ps_sd = _ps_mtl->get_shader();
				LOGE("prm=");
				for(auto& ifm:_ps_prm->_ele_format)
				{
					LOGE( "%x,", ifm );
				}
				LOGE("\n");
				                    
	                    LOGE( "attrlist:" );
	                    auto& attr_list = ps_sd->get_attr_list();
	                    for( auto& iattr : attr_list )
	                    {
	                         LOGE( "name:%s,type%d,", iattr._name.c_str(), iattr._variable_type );
	                    }
	                    LOGE( "\n" );
			}
		}
		//ft_base::draw();
	}

}
