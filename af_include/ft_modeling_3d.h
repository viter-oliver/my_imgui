#pragma once
#include "ft_base.h"
#include "af_shader.h"
#include "af_model.h"
namespace auto_future
{

	class AFG_EXPORT ft_modeling_3d :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(pty_page, _pty_page,
			(char, _model_name[FILE_NAME_LEN]),
			(camera, _cam),
			(projection, _pj),
			(transformation, _trans),//directional_light,point_light
			(af_vec3, _light_ambient_clr),
			(af_vec3, _light_diffuse_clr),
			(af_vec3, _light_specular_clr),
			(af_vec3,_light_position_shd),
			(float, _light_constant,{1.f}),
			(float, _light_linear,{0.09f}),
			(float, _light_quadratic, { 0.032f }),
               (char,_txt_diffuse[FILE_NAME_LEN]),
               (char,_txt_specular[ FILE_NAME_LEN ] )
			)
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
		}
		camera& get_cam()
		{
			return _pty_page._cam;
		}
		projection& get_proj()
		{
			return _pty_page._pj;
		}
		transformation& get_trans()
		{
			return _pty_page._trans;
		}
	};
	REGISTER_CONTROL(ft_modeling_3d)
}