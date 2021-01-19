
#include "ft_trans.h"
#include "af_primitive_object.h"

namespace auto_future
{

     ft_trans::ft_trans()
     {
     
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
          reg_property_handle( &_pt, 0, [this]( void* memb_adress )
          {
               ImGui::Combo( "trans order:", &_pt._trans_order, str_trans_order, en_trans_order_cnt );
          } );

          reg_property_handle( &_pt, 7, [this]( void* memb_adress )
          {
               ImGui::Combo( "rotate order:", &_pt._rotate_order, str_rotate_oder, en_rotate_order_cnt );
          } );
#endif
     }
     void ft_trans::transform( glm::mat4& model )
     {
          glm::vec3 gtranslate( _pt._trans_translation_x, _pt._trans_translation_y, _pt._trans_translation_z );
          glm::vec3 gscale( _pt._trans_scale_x, _pt._trans_scale_y, _pt._trans_scale_z );

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
     }
}
