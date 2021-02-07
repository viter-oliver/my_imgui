#include "ft_hud_obj.h"
#include "ft_hud_projection.h"
const char* hud_obj_shd_name = "sd_hud_obj";
const char* hud_obj_prm_name = "prm_hud_obj";
GLfloat vertices[] = {
     -0.5f, -0.5f,0.f, 0.f, 1.f,
     0.5f, -0.5f, 0.f,1.f, 1.f,
     -0.5f, 0.5f, 0.f,0.f, 0.f,
     0.5f, 0.5f,0.f, 1.f, 0.f,
};
const char* hud_sd_vs = R"glsl(
#version 300 es
precision mediump float;
layout(location=0) in vec3 position;
layout(location=1) in vec2 textCoord;
out vec2 TextCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    TextCoord = textCoord;
}
)glsl";
const char* hud_sd_fs = R"glsl(
#version 300 es
precision mediump float;
in vec2 TextCoord;
out vec4 o_clr;
uniform sampler2D text_at;
void main()
{
	o_clr = texture(text_at, TextCoord);
}
)glsl";
namespace auto_future
{
     ft_hud_obj::ft_hud_obj()
     {
          auto isd = g_af_shader_list.find( hud_obj_shd_name );
          if (isd==g_af_shader_list.end())
          {
               _phud_sd = make_shared<af_shader>( hud_sd_vs, hud_sd_fs );
               g_af_shader_list[ hud_obj_shd_name ] = _phud_sd;
          }
          else
          {
               _phud_sd = isd->second;
          }
          auto iprm = g_primitive_list.find( hud_obj_prm_name );
          if (iprm==g_primitive_list.end())
          {
               _ps_prm = make_shared<primitive_object>();
               _ps_prm->set_ele_format( { 3, 2 } );
               _ps_prm->load_vertex_data( vertices, sizeof( vertices ) / sizeof( float ) );
          }
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
          reg_property_handle( &_pt_tb, 0, [this]( void* member_address )
          {
               if( _pat_image )
               {
                    ImGui::Text( "Attached image:%s", _pt_tb._attached_image );
                    ImGui::SameLine();
                    if( ImGui::Button( "Delink##attimage" ) )
                    {
                         _pat_image = nullptr;
                    }
               }
               else
               {
                    ImGui::InputText( "Attached image:", _pt_tb._attached_image, FILE_NAME_LEN );
                    if( ImGui::Button( "Import" ) )
                    {
                         auto itxt = g_mtexture_list.find( _pt_tb._attached_image );
                         if( itxt != g_mtexture_list.end() )
                         {
                              _pat_image = itxt->second;
                         }
                    }
               }
          } );
#endif
     }

     ft_hud_obj::~ft_hud_obj()
     {
     
     }

     void ft_hud_obj::link()
     {
          auto iat = g_mtexture_list.find( _pt_tb._attached_image );
          if (iat!=g_mtexture_list.end())
          {
               _pat_image = iat->second;
          }
     }

     void ft_hud_obj::draw()
     {
          if (!_pat_image)
          {
               return;
          }
          ft_hud_projection* p_prj = (ft_hud_projection*)get_parent();
          af_vec3* pview_pos = p_prj->get_view_pos();
          af_vec3* pcenter = p_prj->get_center_of_prj();
          af_vec3* pup = p_prj->get_up();
          glm::vec3 cam_pos( pview_pos->x, pview_pos->y, pview_pos->z );
          glm::vec3 cam_dir( pcenter->x, pcenter->y, pcenter->z );
          glm::vec3 cam_up( pup->x, pup->y, pup->z );
          glm::mat4 view = glm::lookAt( cam_pos, cam_dir, cam_up );
          _phud_sd->uniform( "view", glm::value_ptr( view ) );
          float w, h;
          p_prj->get_size( w, h );
          float aspect = w / h;
          glm::mat4 proj = glm::perspective( glm::radians( p_prj->get_fovy() ), aspect, p_prj->get_near(), p_prj->get_far() );
          _phud_sd->uniform( "projection", glm::value_ptr( proj ) );
          glm::mat4 trans;
          trans = glm::translate(
               trans,
               glm::vec3( _pt_tb._tanslation_x, _pt_tb._tanslation_y, _pt_tb._tanslation_z )
               );
          _phud_sd->uniform( "model", glm::value_ptr( trans ) );
          _phud_sd->use();
          glBindVertexArray( _ps_prm->_vao );
          glDrawArrays( GL_TRIANGLE_STRIP, 0, _ps_prm->_vertex_buf_len );
     }

}