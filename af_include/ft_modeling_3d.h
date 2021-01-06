#pragma once
#include "ft_base.h"
#include "af_shader.h"
#include "af_model.h"
#include "af_shader_source_code.h"
namespace auto_future
{

	class AFG_EXPORT ft_modeling_3d :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(pty_page, _pty_page,
			(char, _model_name[FILE_NAME_LEN]),
               ( af_vec3, _light_ambient_clr ),
			(af_vec3, _light_diffuse_clr),
			(af_vec3, _light_specular_clr),
               ( float, _light_posx, {0.f} ),
               ( float, _light_posy, { 0.f } ),
               ( float, _light_posz, { 0.f } ),
			(float, _light_constant_hac,{1.f}),
               ( float, _light_linear_hac, { 0.01f } ),
               ( float, _light_quadratic_hac, { 0.001f } ),
               (char,_txt_diffuse[FILE_NAME_LEN]),
               (char,_txt_specular[ FILE_NAME_LEN ] ),
			(camera, _cam),
			(projection, _pj),
               ( int, _draw_mode, { en_gl_triangles } ),
               ( int, _trans_order, { 0 } ),
               ( float, _trans_translation_x, { 0.f } ),
               ( float, _trans_translation_y, { 0.f } ),
               ( float, _trans_translation_z, { 0.f } ),
               ( float, _trans_scale_x, { 1.f } ),
               ( float, _trans_scale_y, { 1.f } ),
               ( float, _trans_scale_z, { 1.f } ),
               ( int, _rotate_order, { 0 } ),
               ( float, _trans_rotation_x, { 0.f } ),
               ( float, _trans_rotation_y, { 0.f } ),
               ( float, _trans_rotation_z, { 0.f } ))
		shared_ptr<af_shader> _pshd_modeling;
		shared_ptr<af_model> _pmodel;
          ps_af_texture _pdiffuse;
          ps_af_texture _pspecular;
	public:
		ft_modeling_3d();
		~ft_modeling_3d();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		int _dir_view{0};
		void view_components_in_world_space();
#endif		
		void draw();
		void link()
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
		camera& get_cam()
		{
			return _pty_page._cam;
		}
		projection& get_proj()
		{
			return _pty_page._pj;
		}
          void set_trans_scalex( float sx )
          {
               _pty_page._trans_scale_x = sx;
          }
          void set_trans_scaley( float sy )
          {
               _pty_page._trans_scale_y = sy;
          }
          void set_trans_scalez( float sz )
          {
               _pty_page._trans_scale_z = sz;
          }

          void set_trans_rtx( float rx )
          {
               _pty_page._trans_rotation_x = rx;
          }
          void set_trans_rty( float ry )
          {
               _pty_page._trans_rotation_y = ry;
          }
          void set_trans_rtz( float rz )
          {
               _pty_page._trans_rotation_z = rz;
          }

          void set_trans_tlx( float tx )
          {
               _pty_page._trans_translation_x = tx;
          }
          void set_trans_tly( float ty )
          {
               _pty_page._trans_translation_y = ty;
          }
          void set_trans_tlz( float tz )
          {
               _pty_page._trans_translation_z = tz;
          }

          float get_trans_tlx()
          {
               return _pty_page._trans_translation_x;
          }
          float get_trans_tly()
          {
               return _pty_page._trans_translation_y;
          }
          float get_trans_tlz()
          {
               return _pty_page._trans_translation_z;
          }
	};
	REGISTER_CONTROL(ft_modeling_3d)
}