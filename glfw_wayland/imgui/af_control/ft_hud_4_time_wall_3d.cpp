#include "ft_hud_4_time_wall_3d.h"
#include "ft_hud_projector.h"
const char* hud_4_wall_shd_name = "sd_hud_4_wall";
const char* hud_4_wall_prm_name = "prm_hud_4_wall";
const char* hud_sd_4_wall_vs = R"glsl(
#version 300 es
precision mediump float;
layout(location=0) in vec3 position;
layout(location=1) in vec2 textCoord;
out vec2 TextCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float c[4];
uniform float h;
uniform float of;
void main()
{
    vec3 pos=position;
    float posy0=pos.y;
    float z=pos.z*0.01;
    const float fs=100.0;
    pos.x=(c[3]*z*z*z+c[2]*z*z+c[1]*z+c[0])*fs+of;
    if(posy0>0.1){
       pos.y=h;
    }
    
    gl_Position = projection * view * model * vec4(pos, 1.0);
    TextCoord = textCoord;
}
)glsl";
const char* hud_sd_4_wall_fs = R"glsl(
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
     ps_shader ft_hud_4_time_wall_3d::_phud_sd = nullptr;
     ps_primrive_object ft_hud_4_time_wall_3d::_ps_prm = nullptr;

     ft_hud_4_time_wall_3d::ft_hud_4_time_wall_3d()
     {
          /*if( !_phud_sd )
          {
               _phud_sd = make_shared<af_shader>( hud_sd_4_wall_vs, hud_sd_4_wall_fs );
          }
        */

          _pt_tb._attached_image[ 0 ] = '\0';
          _pt_tb._coeff_hac[ 0 ] = _pt_tb._coeff_hac[ 1 ] = _pt_tb._coeff_hac[ 2 ] = _pt_tb._coeff_hac[ 3 ] = 0.f;
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

     ft_hud_4_time_wall_3d::~ft_hud_4_time_wall_3d()
     {

     }
     const int curve_len = 100;
	 const float unit_len = 100.f;// 1000.f;
     const int point_cnt = curve_len * 2 + 2;

     void ft_hud_4_time_wall_3d::link()
     {
          auto iat = g_mtexture_list.find( _pt_tb._attached_image );
          if( iat != g_mtexture_list.end() )
          {
               _pat_image = iat->second;
          }         
          if( !ft_hud_4_time_wall_3d::_phud_sd )
          {
              ft_hud_4_time_wall_3d::_phud_sd = make_shared<af_shader>( hud_sd_4_wall_vs, hud_sd_4_wall_fs );
              ft_hud_4_time_wall_3d::_ps_prm = make_shared<primitive_object>();
              int demension = 5;
              auto data_cnt = point_cnt*demension;
              GLfloat* vertices = new GLfloat[ data_cnt ];
              float uv_unit = 1.f / (float)curve_len;
              for( int ix = 0; ix < curve_len + 1; ++ix )
              {
                   auto base_id = ix * 2 * demension;
                   vertices[ base_id ] = 0;//x->
                   vertices[ base_id + 1 ] = 0;//y->
                   vertices[ base_id + 2 ] = ix*unit_len;//z->
                   vertices[ base_id + 3 ] = 0;
                   vertices[ base_id + 4 ] = uv_unit * ix;

                   vertices[ base_id + 5 ] = 0;//x->
                   vertices[ base_id + 6 ] = 1;//y->
                   vertices[ base_id + 7 ] = ix*unit_len;//z->
                   vertices[ base_id + 8 ] = 1;
                   vertices[ base_id + 9 ] = uv_unit * ix;
              }

              ft_hud_4_time_wall_3d::_ps_prm->set_ele_format( { 3, 2 } );
              ft_hud_4_time_wall_3d::_ps_prm->load_vertex_data( vertices, data_cnt );
              delete[] vertices;
          }
     }

     void ft_hud_4_time_wall_3d::draw()
     {
          if( !_pat_image )
          {
               return;
          }
          ft_hud_projector* p_prj = (ft_hud_projector*)get_parent();
          af_vec3* pview_pos = p_prj->get_view_pos();
          af_vec3* pcenter = p_prj->get_center_of_prj();
          af_vec3* pup = p_prj->get_up();
          glm::vec3 cam_pos( pview_pos->x, pview_pos->y, pview_pos->z );
          glm::vec3 cam_dir( pcenter->x, pcenter->y, pcenter->z );
          glm::vec3 cam_up( pup->x, pup->y, pup->z );
          glm::mat4 view = glm::lookAt( cam_pos, cam_dir, cam_up );
          _phud_sd->use();
          _phud_sd->uniform( "view", glm::value_ptr( view ) );
          float w, h;
          p_prj->get_size( w, h );
          float aspect = w / h;
          float near_value = _pt_tb._near > 0.f ? _pt_tb._near : p_prj->get_near();
		float far_value = _pt_tb._far > 0.f ? _pt_tb._far : p_prj->get_far();

		glm::mat4 proj = glm::perspective(glm::radians(p_prj->get_fovy()), aspect, near_value, far_value);
          _phud_sd->uniform( "projection", glm::value_ptr( proj ) );
          glm::mat4 trans;
          trans = glm::translate(
               trans,
               glm::vec3( _pt_tb._tanslation_x, _pt_tb._tanslation_y, _pt_tb._tanslation_z )
               );
          _phud_sd->uniform( "model", glm::value_ptr( trans ) );
          _phud_sd->uniform( "c[0]", _pt_tb._coeff_hac );
          _phud_sd->uniform( "h", &_pt_tb._height );
		  _phud_sd->uniform("of", &_pt_tb._offset );

          glActiveTexture( GL_TEXTURE0 );
          glBindTexture( GL_TEXTURE_2D, _pat_image->_txt_id() );
          _phud_sd->uniform( "text_at", 0 );
          glBindVertexArray( _ps_prm->_vao );
          glDrawArrays( GL_TRIANGLE_STRIP, 0, point_cnt );
     }
}