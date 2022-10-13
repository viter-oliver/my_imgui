#include "ft_light_scene.h"
#include "common_functions.h"
#include "af_primitive_object.h"
namespace auto_future
{
     ft_light_scene::ft_light_scene()
     {
          _in_p._sizew = 1500;
          _in_p._sizeh = 1000;
          _pj_pt._view_pos = { -41.f, 130.f, -500.f };
          _pj_pt._center_of_prj = { -41.f, 90.f, 542.f };
		  
          _pj_pt._up = { 0, 1.f, 0 };
          _pj_pt._fovy = 20;
          _pj_pt._near = 20;
          _pj_pt._far = 100000.f;
          _pj_pt._test_depth = false;
		  _pj_pt._bk_clr = { 0.2f, 0.2f, 0.2f, 1.f };
		  _pj_pt._light_pos = { 41.f, 140, -500 };
		  _pj_pt._light_ambient_clr = { 1, 1, 1 };
		  _pj_pt._light_diffuse_clr = { 1, 1, 1 };
		  _pj_pt._light_specular_clr = { 1, 1, 1 };
		  _pj_pt._translate = { 0, 0, 0 };
		  _pj_pt._scale = { 1, 1, 1 };
		  _pj_pt._roration = { 0, 0, 0 };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		  reg_property_handle(&_pj_pt, 10, [this](void* memb_adress)
		  {
			  ImGui::Combo("trans order:", &_pj_pt._trans_order, str_trans_order, en_trans_order_cnt);
		  });

		  reg_property_handle(&_pj_pt, 11, [this](void* memb_adress)
		  {
			  ImGui::Combo("rotate order:", &_pj_pt._rotate_order, str_rotate_oder, en_rotate_order_cnt);
		  });
          reg_property_handle( &_pj_pt, 16, [this]( void* memb_adress )
          {
               ImGui::ColorEdit4( "Color of background", (float*)&_pj_pt._bk_clr, ImGuiColorEditFlags_RGB );
               if( !_fboId )
               {
                    if( ImGui::Button( "Create frame buffer" ) )
                    {
                         prepareFBO1( _colorTextId, _depthStencilTextId, _fboId, _in_p._sizew, _in_p._sizeh );
                    }
               }
               else
               {
                    if( ImGui::Button( "Recreate frame buffer" ) )
                    {
                         release_resource();
                         prepareFBO1( _colorTextId, _depthStencilTextId, _fboId, _in_p._sizew, _in_p._sizeh );
                    }
               }
          } );
#endif
     }
     void ft_light_scene::release_resource()
     {
          if( _colorTextId )
               glDeleteTextures( 1, &_colorTextId );
          if( _depthStencilTextId )
               glDeleteTextures( 1, &_depthStencilTextId );
          if( _fboId )
          {
               glDeleteFramebuffers( 1, &_fboId );
               _fboId = 0;
          }
     }
     ft_light_scene::~ft_light_scene()
     {
          release_resource();
     }
     void ft_light_scene::link()
     {
          prepareFBO1( _colorTextId, _depthStencilTextId, _fboId, _in_p._sizew, _in_p._sizeh );

     }
	 void ft_light_scene::transform(glm::mat4& model){

		 glm::vec3 gtranslate(_pj_pt._translate.x, _pj_pt._translate.y, _pj_pt._translate.z);
		 glm::vec3 gscale(_pj_pt._scale.x, _pj_pt._scale.y, _pj_pt._scale.z);

		 function<void()> f_rotate[en_rotate_order_cnt] =
		 {
			 [&]()
			 {
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.x), glm::vec3(1.0f, 0.0f, 0.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.y), glm::vec3(0.0f, 1.0f, 0.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.z), glm::vec3(0.0f, 0.0f, 1.0f));
			 },
				 [&]()
			 {
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.x), glm::vec3(1.0f, 0.0f, 0.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.z), glm::vec3(0.0f, 0.0f, 1.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.y), glm::vec3(0.0f, 1.0f, 0.0f));
			 },
				 [&]()
			 {
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.y), glm::vec3(0.0f, 1.0f, 0.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.x), glm::vec3(1.0f, 0.0f, 0.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.z), glm::vec3(0.0f, 0.0f, 1.0f));
			 },
				 [&]()
			 {
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.y), glm::vec3(0.0f, 1.0f, 0.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.z), glm::vec3(0.0f, 0.0f, 1.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.x), glm::vec3(1.0f, 0.0f, 0.0f));
			 },
				 [&]()
			 {
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.z), glm::vec3(0.0f, 0.0f, 1.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.x), glm::vec3(1.0f, 0.0f, 0.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.y), glm::vec3(0.0f, 1.0f, 0.0f));
			 },
				 [&]()
			 {
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.z), glm::vec3(0.0f, 0.0f, 1.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.y), glm::vec3(0.0f, 1.0f, 0.0f));
				 model = glm::rotate(model, glm::radians(_pj_pt._roration.x), glm::vec3(1.0f, 0.0f, 0.0f));
			 },
		 };
		 function<void()> f_trans[en_trans_order_cnt] =
		 {
			 [&]()
			 {
				 f_rotate[_pj_pt._rotate_order]();
				 model = glm::scale(model, gscale);
				 model = glm::translate(model, gtranslate);
			 },
				 [&]()
			 {
				 model = glm::scale(model, gscale);
				 f_rotate[_pj_pt._rotate_order]();
				 model = glm::translate(model, gtranslate);
			 },
				 [&]()
			 {
				 f_rotate[_pj_pt._rotate_order]();
				 model = glm::translate(model, gtranslate);
				 model = glm::scale(model, gscale);
			 },
				 [&]()
			 {
				 model = glm::translate(model, gtranslate);
				 f_rotate[_pj_pt._rotate_order]();
				 model = glm::scale(model, gscale);
			 },
				 [&]()
			 {
				 model = glm::scale(model, gscale);
				 model = glm::translate(model, gtranslate);
				 f_rotate[_pj_pt._rotate_order]();
			 },
				 [&]()
			 {
				 model = glm::translate(model, gtranslate);
				 model = glm::scale(model, gscale);
				 f_rotate[_pj_pt._rotate_order]();
			 },
		 };
		 f_trans[_pj_pt._trans_order]();
	 }
     void ft_light_scene::draw_frames()
     {
          if( !_fboId )
          {
               return;
          }
          GLint last_viewport[ 4 ];
          glGetIntegerv( GL_VIEWPORT, last_viewport );
          GLint last_fmid;
          glGetIntegerv( GL_FRAMEBUFFER_BINDING, &last_fmid );
          glBindFramebuffer( GL_FRAMEBUFFER, _fboId );
          glViewport( 0, 0, _in_p._sizew, _in_p._sizeh );
          glEnable( GL_BLEND );
          glBlendEquation( GL_FUNC_ADD );
          glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
          //glEnable(GL_DEPTH_TEST);
          //glDepthFunc(GL_LESS);
          //glDisable( GL_CULL_FACE );
          if( _pj_pt._test_depth )
          {
               glEnable( GL_DEPTH_TEST );
          }
          else
          {
               glDisable( GL_DEPTH_TEST );
          }
          //glDisable(GL_SCISSOR_TEST);
          glClearColor( _pj_pt._bk_clr.x, _pj_pt._bk_clr.y, _pj_pt._bk_clr.z, _pj_pt._bk_clr.w );
          glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
          //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          for( auto it : _vchilds )
          {
               if( it->is_visible() )
               {
                    it->draw_frames();
               }
          }
          glBindFramebuffer( GL_FRAMEBUFFER, last_fmid );
          glViewport( last_viewport[ 0 ], last_viewport[ 1 ], (GLsizei)last_viewport[ 2 ], (GLsizei)last_viewport[ 3 ] );
          ImVec2 abpos = absolute_coordinate_of_base_pos();
          ImVec2 winpos = ImGui::GetWindowPos();
          float sizew = _in_p._sizew;
          float sizeh = _in_p._sizeh;
          ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
          ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
          ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
          ImVec2 pos4 = { pos1.x + sizew, pos1.y };
          //glActiveTexture(GL_TEXTURE0);
          //glBindTexture(GL_TEXTURE_2D, _colorTextId);
          //ImGui::ImageQuad((ImTextureID)_colorTextId, pos1, pos2, pos3, pos4, ImVec2(0, 0), ImVec2(0, 1), ImVec2(1, 1), ImVec2(1, 0));
          ImGui::ImageQuad( (ImTextureID)_colorTextId, pos1, pos2, pos3, pos4, ImVec2( 1, 1 ), ImVec2( 1, 0 ), ImVec2( 0, 0 ), ImVec2( 0, 1 ) );
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
          if( is_selected() )//draw envelope
          {
               ImU32 col = ImGui::GetColorU32( ImGuiCol_HeaderActive );
               ImVec2 editunit( edit_unit_len, edit_unit_len );
               ImVec2 pos1a = pos1 - editunit;
               ImVec2 pos1b = pos1 + editunit;
               ImGui::RenderFrame( pos1a, pos1b, col );
               ImVec2 pos2a = pos2 - editunit;
               ImVec2 pos2b = pos2 + editunit;
               ImGui::RenderFrame( pos2a, pos2b, col );
               ImVec2 pos3a = pos3 - editunit;
               ImVec2 pos3b = pos3 + editunit;
               ImGui::RenderFrame( pos3a, pos3b, col );
               ImVec2 pos4a = pos4 - editunit;
               ImVec2 pos4b = pos4 + editunit;
               ImGui::RenderFrame( pos4a, pos4b, col );
          }
#endif
     }

     bool ft_light_scene::handle_mouse()
     {
          return true;
     }

}

