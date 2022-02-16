
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ft_modeling_3d.h"
#include "af_shader_source_code.h"
#include "af_model.h"
#include "af_primitive_object.h"
#include "res_output.h"
#include "ft_trans.h"
#include <sstream>
namespace auto_future
{
     ft_modeling_3d::ft_modeling_3d()
     {
          memset( _pty_page._model_name, 0, FILE_NAME_LEN );
          memset( _pty_page._txt_diffuse, 0, FILE_NAME_LEN );
          memset( _pty_page._txt_specular, 0, FILE_NAME_LEN );

          _pty_page._cam = { { 0.f, 0.f, 3.f }, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f } };
          _pty_page._pj = { 60.f, 0.1f, 100.f };
         // _pty_page._trans = { { 0.15f, 0.15f, 0.15f }, { 0, 0, 0 }, { 0, 0, 0 }, true };
          _pty_page._light_ambient_clr = { 0.2f, 0.2f, 0.2f };
          _pty_page._light_diffuse_clr = { 0.5f, 0.5f, 0.5f };
          _pty_page._light_specular_clr = { 1.f, 1.f, 1.f };
          _pty_page._draw_mode = en_gl_triangles;
          //_pty_page._light_position_shd = { -1.f, 1.f, -2.f };
#if !defined(DISABLE_DEMO)
          string str_modeling( modeling );
          auto shd_modeling = g_af_shader_list.find( str_modeling );
          if( shd_modeling == g_af_shader_list.end() )
          {
               _pshd_modeling = make_shared<af_shader>( modeling_vs, modeling_fs );
               _pshd_modeling->set_name( str_modeling );
               _pshd_modeling->_vs_name = str_modeling + ".vs";
               _pshd_modeling->_fs_name = str_modeling + ".fs";
               g_af_shader_list[ modeling ] = _pshd_modeling;
          }
          else
          {
               _pshd_modeling = shd_modeling->second;
          }

          reg_property_handle( &_pty_page, 0, [this]( void*member_address )
          {
               if( _pmodel )
               {
                    ImGui::Text( "Model name:%s", _pty_page._model_name );
                    ImGui::SameLine();
                    if( ImGui::Button( "Delink##model_name" ) )
                    {
                         _pmodel = nullptr;
                    }
               }
               else
               {
                    ImGui::InputText( "Model name:", _pty_page._model_name, FILE_NAME_LEN );
                    if( ImGui::Button( "import" ) )
                    {
                         auto imodel = g_mmodel_list.find( _pty_page._model_name );
                         if( imodel != g_mmodel_list.end() )
                         {
                              _pmodel = imodel->second;
                              bounding_box mbox;
                              auto& mdlist = *_pmodel;
                              for( auto& amesh : mdlist )
                              {
                                   auto& box = amesh._box;
                                   if( box._xmin < mbox._xmin )
                                   {
                                        mbox._xmin = box._xmin;
                                   }
                                   if( box._xmax > mbox._xmax )
                                   {
                                        mbox._xmax = box._xmax;
                                   }
                                   if( box._ymin < mbox._ymin )
                                   {
                                        mbox._ymin = box._ymin;
                                   }
                                   if( box._ymax > mbox._ymax )
                                   {
                                        mbox._ymax = box._ymax;
                                   }
                                   if( box._zmin < mbox._zmin )
                                   {
                                        mbox._zmin = box._zmin;
                                   }
                                   if( box._zmax > mbox._zmax )
                                   {
                                        mbox._zmax = box._zmax;
                                   }
                              }
                              auto zthickness = mbox._zmax - mbox._zmin;
                              auto yheight = mbox._ymax - mbox._zmin;

                              _pty_page._cam._position = { ( mbox._xmax + mbox._xmin )*0.5f, \
                                   ( mbox._ymax + mbox._ymin )*0.5,mbox._zmin-zthickness };
                              _pty_page._cam._direction = _pty_page._cam._position;
                              _pty_page._cam._direction.z = mbox._zmin;
                              _pty_page._cam._up = { 0.f, 1.f, 0.f };
                              _pty_page._pj._near = mbox._zmin;
                              _pty_page._pj._far= mbox._zmax;
#define PI 3.1415926545
                              _pty_page._pj._fovy = 2 * atanf( yheight*0.5 / zthickness )*180/PI;
                              _pty_page._light_posx = _pty_page._cam._position.x;
                              _pty_page._light_posy = _pty_page._cam._position.y;
                              _pty_page._light_posz = _pty_page._cam._position.z;

                         }
                    }
               }

          } );
          reg_property_handle( &_pty_page, 10, [this]( void* member_address )
          {
               if( _pdiffuse )
               {
                    ImGui::Text( "Diffuse texture:%s", _pty_page._txt_diffuse );
                    ImGui::SameLine();
                    if( ImGui::Button( "Delink##diffuse" ) )
                    {
                         _pdiffuse = nullptr;
                    }
               }
               else
               {
                    ImGui::InputText( "Diffuse texture:", _pty_page._txt_diffuse, FILE_NAME_LEN );
                    if( ImGui::Button( "Import" ) )
                    {
                         auto itxt = g_mtexture_list.find( _pty_page._txt_diffuse );
                         if( itxt != g_mtexture_list.end() )
                         {
                              _pdiffuse = itxt->second;
                         }
                    }
               }
          } );
          reg_property_handle( &_pty_page, 11, [this]( void* member_address )
          {
               if( _pspecular )
               {
                    ImGui::Text( "Specular texture:%s", _pty_page._txt_specular );
                    ImGui::SameLine();
                    if( ImGui::Button( "Delink##diffuse" ) )
                    {
                         _pspecular = nullptr;
                    }
               }
               else
               {
                    ImGui::InputText( "Specular texture:", _pty_page._txt_specular, FILE_NAME_LEN );
                    if( ImGui::Button( "Import" ) )
                    {
                         auto itxt = g_mtexture_list.find( _pty_page._txt_specular );
                         if( itxt != g_mtexture_list.end() )
                         {
                              _pspecular = itxt->second;
                         }
                    }
               }
          } );
          reg_property_handle( &_pty_page, 14, [this]( void* memb_adress )
          {
               ImGui::Combo( "Draw mode:", &_pty_page._draw_mode, draw_mode, en_gl_count );
          } );
          reg_property_handle( &_pty_page, 15, [this]( void* memb_adress )
          {
               ImGui::Combo( "trans order:", &_pty_page._trans_order, str_trans_order, en_trans_order_cnt );
          } );
          reg_property_handle( &_pty_page, 22, [this]( void* memb_adress )
          {
               ImGui::Combo( "rotate order:", &_pty_page._rotate_order, str_rotate_oder, en_rotate_order_cnt );
          } );
#endif
     }
     ft_modeling_3d::~ft_modeling_3d()
     {}
#if !defined(DISABLE_DEMO)
     void ft_modeling_3d::view_components_in_world_space()
     {
          ImGui::Text( "View direction:" ); ImGui::SameLine();
          ImGui::RadioButton( "X", &_dir_view, 0 ); ImGui::SameLine();
          ImGui::RadioButton( "Y", &_dir_view, 1 ); ImGui::SameLine();
          ImGui::RadioButton( "Z", &_dir_view, 2 );
          ImVec2 winpos = ImGui::GetWindowPos();
          ImVec2 pos0 = winpos + ImVec2( 150, 180 );
          ImDrawList* draw_list = ImGui::GetWindowDrawList();
          ImU32 col = ImGui::GetColorU32( ImGuiCol_HeaderActive );
          draw_list->AddCircle( pos0, 100, col, 25 );
          ImVec2 pos_left = { pos0.x - 100, pos0.y };
          ImVec2 pos_right = { pos0.x + 100, pos0.y };
          ImVec2 pos_top = { pos0.x, pos0.y - 100 };
          ImVec2 pos_bottom = { pos0.x, pos0.y + 100 };
          draw_list->AddLine( pos_left, pos_right, col );
          draw_list->AddLine( pos_top, pos_bottom, col );
          ImVec2 lt_pos = pos0;
          af_vec3  ltPos { _pty_page._light_posx, _pty_page._light_posy, _pty_page._light_posz };
          switch( _dir_view )
          {
               case 0:
                    lt_pos += ImVec2( ltPos.y, ltPos.z );
                    break;
               case 1:
                    lt_pos += ImVec2( ltPos.x, ltPos.z );
                    break;
               case 2:
                    lt_pos += ImVec2( ltPos.x, ltPos.y );
                    break;
          }
          af_vec3& lt_spcol = _pty_page._light_specular_clr;
          ImVec4 vlt_col = { lt_spcol.x, lt_spcol.y, lt_spcol.z, 1 };
          ImU32 lt_col = ImGui::ColorConvertFloat4ToU32( vlt_col );
          draw_list->AddCircleFilled( lt_pos, 5, lt_col );
     }
#endif	

	void ft_modeling_3d::link()
	{
		auto imodel = g_mmodel_list.find(_pty_page._model_name);
		if (imodel != g_mmodel_list.end())
		{
		_pmodel = imodel->second;
		}
		auto itxt = g_mtexture_list.find( _pty_page._txt_diffuse );
		if( itxt != g_mtexture_list.end() )
		{
		    _pdiffuse = itxt->second;
		}
		itxt = g_mtexture_list.find( _pty_page._txt_specular );
		if( itxt != g_mtexture_list.end() )
		{
		    _pspecular = itxt->second;
		}

		string str_modeling( modeling );
		auto shd_modeling = g_af_shader_list.find( modeling );
		if( shd_modeling == g_af_shader_list.end() )
		{
		    _pshd_modeling = make_shared<af_shader>( modeling_vs, modeling_fs );
		    _pshd_modeling->set_name( str_modeling );

		    g_af_shader_list[ modeling ] = _pshd_modeling;
		}
		else
		{
		    _pshd_modeling = shd_modeling->second;
		}
	}
	void ft_modeling_3d::draw()
	{
		if (!_pmodel)
		{
			return;
		}
		af_model& my_model = *_pmodel;
		af_shader& my_shader =*_pshd_modeling;
		my_shader.use();
		glm::mat4 model;
          for( auto& item : _vchilds )
          {
               ft_trans* ptrans_item = (ft_trans*)item;
               ptrans_item->transform( model );
          }
          glm::vec3 gtranslate( _pty_page._trans_translation_x, _pty_page._trans_translation_y, _pty_page._trans_translation_z );
          glm::vec3 gscale( _pty_page._trans_scale_x, _pty_page._trans_scale_y, _pty_page._trans_scale_z );
          auto& _pt = _pty_page;
          function<void()> f_rotate[ en_rotate_order_cnt ] =
          {
               [&]()
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
                    model = glm::scale( model, gscale );
                    model = glm::translate( model, gtranslate );
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
		const auto& cam_pos = _pty_page._cam._position;
		const auto& cam_dir = _pty_page._cam._direction;
		const auto& cam_up = _pty_page._cam._up;
		glm::vec3 gcam_pos(cam_pos.x, cam_pos.y, cam_pos.z);
		glm::vec3 gcam_dir(cam_dir.x, cam_dir.y, cam_dir.z);
		glm::vec3 gcam_up(cam_up.x, cam_up.y, cam_up.z);
		view = glm::lookAt(gcam_pos, gcam_dir, gcam_up);
		glm::mat4 proj;
		const auto& cproj = _pty_page._pj;
		auto pparent = get_parent();
		float w, h;
		pparent->get_size(w, h);
		float aspect = w / h;
		proj = glm::perspective(glm::radians(cproj._fovy), aspect, cproj._near, cproj._far);
		my_shader.uniform("viewPos", glm::value_ptr(gcam_pos));
		my_shader.uniform("model", glm::value_ptr(model));
		my_shader.uniform("view", glm::value_ptr(view));
		my_shader.uniform("projection", glm::value_ptr(proj));
		my_shader.uniform("light_ambient", (float*)&_pty_page._light_ambient_clr);
		my_shader.uniform("light_diffuse", (float*)&_pty_page._light_diffuse_clr);
		my_shader.uniform("light_specular", (float*)&_pty_page._light_specular_clr);
		my_shader.uniform("light_position", (float*)&_pty_page._light_posx);
		my_shader.uniform("light_constant", &_pty_page._light_constant_hac);
		my_shader.uniform("light_linear", &_pty_page._light_linear_hac);
		my_shader.uniform("light_quadratic", &_pty_page._light_quadratic_hac);

		for (auto& amesh:my_model)
		{
			
			const auto& primid=*amesh._ps_prm_id;
			
               int ix = 0, itx_cnt = 0;
               if( _pdiffuse )
               {
                    glActiveTexture( GL_TEXTURE0 + itx_cnt );
                    glBindTexture( GL_TEXTURE_2D, _pdiffuse->_txt_id() );

                    my_shader.uniform( "texture_diffuse0", itx_cnt++ );
               }
               if( _pspecular )
               {
                    glActiveTexture( GL_TEXTURE0 + itx_cnt );
                    glBindTexture( GL_TEXTURE_2D, _pspecular->_txt_id() );

                    my_shader.uniform( "texture_specular0", itx_cnt );
               }
               if( 0==itx_cnt )
               {
                    auto& ps_diffuse_list = amesh._ps_text_diffuse_list;
			     auto& ps_specular_list = amesh._ps_text_specular_list;

			     if (ps_diffuse_list.size()==0)
			     {
				     auto& diffuse_list = amesh._text_diffuse_list;
				     for (auto& diff:diffuse_list)
				     {
					     ps_diffuse_list.emplace_back();
					     auto& txt_diff = ps_diffuse_list.back();
					const auto& itxt = g_mtexture_list.find(diff);
					     if (itxt!=g_mtexture_list.end())
					     {
						     txt_diff = itxt->second;
					     }
				     }
				
				     //auto& height_list = amesh._text_height_list;
				     //auto& ambient_list = amesh._text_ambient_list;

			     }
			     if (ps_specular_list.size() == 0)
			     {
				     auto& specular_list = amesh._text_specular_list;
				     for (auto& spec:specular_list)
				     {
					     ps_specular_list.emplace_back();
					     auto& txt_spec = ps_specular_list.back();
					const auto& itxt = g_mtexture_list.find(spec);
					     if (itxt != g_mtexture_list.end())
					     {
						     txt_spec = itxt->second;
					     }
				     }
			     }
			     //auto& height_list = amesh._text_height_list;
			     //auto& ambient_list = amesh._text_ambient_list;
			     for (auto& diff:ps_diffuse_list)
			     {
				     auto& txt = *diff;
				     glActiveTexture(GL_TEXTURE0 + itx_cnt);
				     glBindTexture(GL_TEXTURE_2D, txt._txt_id());
				     stringstream stm;
				     stm << "texture_diffuse" << ix++;
				     my_shader.uniform(stm.str(), itx_cnt++);
			     }
			     ix = 0;
			     for (auto& specular : ps_specular_list)
			     {
				     auto& txt = *specular;
				     glActiveTexture(GL_TEXTURE0 + itx_cnt);
				     glBindTexture(GL_TEXTURE_2D, txt._txt_id());
				     stringstream stm;
				     stm << "texture_specular" << ix++;
				     my_shader.uniform(stm.str(), itx_cnt++);
			     }

               }
               static GLuint draw_model[ en_gl_count ] =
               {
                    GL_POINTS,
                    GL_LINES,
                    GL_LINE_LOOP,
                    GL_LINE_STRIP,
                    GL_TRIANGLES,
                    GL_TRIANGLE_STRIP,
                    GL_TRIANGLE_FAN,
               };
               GLuint& dml = draw_model[ _pty_page._draw_mode ];
			glBindVertexArray(primid._vao);
               glDrawElements( dml, primid._ele_buf_len, GL_UNSIGNED_INT, 0 );
		}
	}

}
