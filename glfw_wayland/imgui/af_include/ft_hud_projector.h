#pragma once
#include "ft_base.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace auto_future
{
     class AFG_EXPORT ft_hud_projector :
     public ft_base
     {
          DEF_STRUCT_WITH_INIT( intl_pt, _pj_pt,
                                ( af_vec3,_view_pos ),
                                (af_vec3,_center_of_prj),
                                (af_vec3,_up),
                                (float,_fovy),
                                (float,_near),
                                (float,_far),
                                (bool,_test_depth),
                                (af_vec4,_bk_clr))
          unsigned int _fboId = { 0 };
          unsigned int _colorTextId = { 0 };
          unsigned int _depthStencilTextId = { 0 };
          void release_resource();
     public:
          ft_hud_projector();
          ~ft_hud_projector();
          void link();
          void draw_frames();
          bool handle_mouse();
          af_vec3* get_view_pos()
          {
               return &_pj_pt._view_pos;
          }
          af_vec3* get_center_of_prj()
          {
               return &_pj_pt._center_of_prj;
          }
          af_vec3* get_up()
          {
               return &_pj_pt._up;
          }
          float get_fovy()
          {
               return _pj_pt._fovy;
          }
          float get_near()
          {
               return _pj_pt._near;
          }
          float get_far()
          {
               return _pj_pt._far;
          }
         void set_fovy(float fovy)
         {
              _pj_pt._fovy=fovy;
         }
         void set_far(float ffar)
         {
              _pj_pt._far = ffar;
         }
         void set_near(float fnear)
         {
              _pj_pt._near=fnear;
         }
     };
     REGISTER_CONTROL(ft_hud_projector)
}

