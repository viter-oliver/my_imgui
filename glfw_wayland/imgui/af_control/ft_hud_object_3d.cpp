#include "ft_hud_object_3d.h"
#include "ft_hud_projector.h"
#include "ft_trans.h"

const char* hud_obj_sd_vs=R"glsl(
#version 300 es
precision mediump float;
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

out vec2 Textcoord;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
void main()
{
	float nml= length( normalize( normal ) );
    gl_Position = proj * view * model* vec4(position, 1.0);
    Textcoord = uv * nml;
}
)glsl";
const char* hud_obj_sd_fs=R"glsl(
#version 300 es
precision mediump float;
in vec2 Textcoord;
out vec4 o_clr;
uniform sampler2D text;
uniform float alpha;
void main()
{
   vec4 fcolor = texture (text, Textcoord);
   fcolor.a = alpha * fcolor.a;
   o_clr = fcolor;// texture2D(text, Textcoord);
}
)glsl";
namespace auto_future
{
     ps_shader ft_hud_object_3d::_phud_object_sd =nullptr;
     ft_hud_object_3d::ft_hud_object_3d()
     {

          _in_p._sizew = _in_p._sizeh = 1000.f;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		  _pt_tb._attached_image[0] = '\0';
		  _pt_tb._attached_obj[0] = '\0';
          reg_property_handle( &_pt_tb, 2, [this]( void* member_address )
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
		reg_property_handle(&_pt_tb, 3, [this](void* member_address)
		{
			if (_ps_prm)
			{
				ImGui::Text("Attached primitive:%s", _pt_tb._attached_obj);
				ImGui::SameLine();
				if (ImGui::Button("Delink##primitive"))
				{
					_ps_prm = nullptr;
				}
			}
			else
			{
				ImGui::InputText("Attached primitive:", _pt_tb._attached_obj, FILE_NAME_LEN);
				if (ImGui::Button("Import"))
				{
					auto itxt = g_primitive_list.find(_pt_tb._attached_obj);
					if (itxt != g_primitive_list.end())
					{
						_ps_prm = itxt->second;
					}
				}
			}
		});

#endif
     }

     ft_hud_object_3d::~ft_hud_object_3d()
     {
     
     }

     void ft_hud_object_3d::link()
     {
		 auto iat = g_mtexture_list.find(_pt_tb._attached_image);
          if (iat!=g_mtexture_list.end())
          {
               _pat_image = iat->second;
          }
		  auto iprm = g_primitive_list.find(_pt_tb._attached_obj);
		  if (iprm != g_primitive_list.end())
		  {
			  _ps_prm = iprm->second;
		  }
          if( !ft_hud_object_3d::_phud_object_sd )
          {
			  ft_hud_object_3d::_phud_object_sd = make_shared<af_shader>(hud_obj_sd_vs, hud_obj_sd_fs);
          }
     }

     void ft_hud_object_3d::draw()
     {
		 if (!_pat_image || !_ps_prm)
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
          _phud_object_sd->use();
          _phud_object_sd->uniform( "view", glm::value_ptr( view ) );
          float w, h;
          p_prj->get_size( w, h );
		  float aspect = w / h;
		  float near_value = _pt_tb._near > 0.f ? _pt_tb._near : p_prj->get_near();
		  float far_value = _pt_tb._far > 0.f ? _pt_tb._far : p_prj->get_far();

		  glm::mat4 proj = glm::perspective(glm::radians(p_prj->get_fovy()), aspect, near_value, far_value);
          _phud_object_sd->uniform( "proj", glm::value_ptr( proj ) );
		  _phud_object_sd->uniform("alpha", &_pt_tb._alpha);
          glm::mat4 model;
		  for (auto& item : _vchilds){
			  ft_trans* ptrans_item = (ft_trans*)(item);
			  ptrans_item->transform(model);
		  }
		  auto chd_cnt=p_prj->child_count();
		  for (int ix = 0; ix < chd_cnt; ++ix){
			  auto pchild = p_prj->get_child(ix);
			  string type_name = typeid(*pchild).name();
			  string type_show = type_name.substr(sizeof("class autofuture::"));
			  if (type_show == "ft_trans"){
				  auto ptrans = (ft_trans*)pchild;
				  ptrans->transform(model);
			  }
		  }
          _phud_object_sd->uniform( "model", glm::value_ptr( model ) );
          glActiveTexture( GL_TEXTURE0 );
          glBindTexture( GL_TEXTURE_2D, _pat_image->_txt_id() );
          _phud_object_sd->uniform( "text", 0 );
		  glBindVertexArray(_ps_prm->_vao);
		  if (0 == _ps_prm->_ele_buf_len){
			  glDrawArrays(GL_TRIANGLES, 0, _ps_prm->_vertex_buf_len);
		  } else {
			  glDrawElements(GL_TRIANGLES, _ps_prm->_ele_buf_len, GL_UNSIGNED_INT, 0);
		  }
		 
     }

}