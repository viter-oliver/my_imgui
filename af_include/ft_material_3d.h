#pragma once
#include "ft_base.h"
#include "af_material.h"
#include "af_primitive_object.h"
namespace auto_future
{
	class AFG_EXPORT ft_material_3d :
		public ft_base
	{
		enum
		{
			en_gl_points,
			en_gl_lines,
			eg_gl_line_loop,
			en_gl_line_strip,
			en_gl_triangles,
			en_gl_triangle_strip,
			en_gl_triangle_fan,
			en_gl_count
		};
          enum en_trans_order
          {
               en_translate_scale_rotate,
               en_translate_rotate_scale,
               en_scale_translate_rotate,
               en_scale_rotate_translate,
               en_rotate_translate_scale,
               en_rotate_scale_translate,
               en_trans_order_cnt
          };
          enum en_rotate_order
          {
               en_rotate_x_y_z,
               en_rotate_x_z_y,
               en_rotate_y_x_z,
               en_rotate_y_z_x,
               en_rotate_z_x_y,
               en_rotate_z_y_x,
               en_rotate_order_cnt
          };
		DEF_STRUCT_WITH_INIT(intl_pt, _pt,
			(int, _draw_mode, {en_gl_triangles}),
			(char, _primitive_name[FILE_NAME_LEN]),
			(char, _material_name[FILE_NAME_LEN]),
			(char, _uf_model_name[FILE_NAME_LEN]),
			(char, _uf_view_name[FILE_NAME_LEN]),
			(char, _uf_proj_name[FILE_NAME_LEN]),
			(camera,_cam),
			(projection,_pj),
            ( int, _trans_order, {0} ),
            ( float, _trans_translation_x, { 0.f } ),
            ( float, _trans_translation_y, { 0.f } ),
            ( float, _trans_translation_z, { 0.f } ),
            ( float, _trans_scale_x, {1.f} ),
            ( float, _trans_scale_y, {1.f} ),
            ( float, _trans_scale_z, {1.f} ),
            ( int,_rotate_order, {0} ),
            ( float, _trans_rotation_x, {0.f} ),
            ( float, _trans_rotation_y, {0.f} ),
            ( float, _trans_rotation_z, {0.f} ),
            ( float, _alpha_nml, {1.f} ),
		  (bool, _with_feedback, {false}))
		bool _matched{ false };
        bool _same_sclae { false };
		ps_primrive_object _ps_prm;
		ps_mtl _ps_mtl;
		GLuint _gpu_outbuff = { 0 };
		vector<float> _output_buff;
	public:
		ft_material_3d();
		~ft_material_3d();
		void link();
		void draw();
		void set_cam_posx(float posx)
		{
			_pt._cam._position.x = posx;
		}
		void set_cam_posy(float posy)
		{
			_pt._cam._position.y = posy;
		}
		void set_cam_posz(float posz)
		{
			_pt._cam._position.z = posz;
		}
		
		void set_cam_dirx(float dirx)
		{
			_pt._cam._direction.x = dirx;
		}
		void set_cam_diry(float diry)
		{
			_pt._cam._direction.y = diry;
		}
		void set_cam_dirz(float dirz)
		{
			_pt._cam._direction.z = dirz;
		}

		void set_cam_upx(float upx)
		{
			_pt._cam._up.x = upx;
		}
		void set_cam_upy(float upy)
		{
			_pt._cam._up.y = upy;
		}
		void set_cam_upz(float upz)
		{
			_pt._cam._up.z = upz;
		}

		void set_pj_fovy(float fovy)
		{
			_pt._pj._fovy = fovy;
		}
		void set_pj_near(float fnear)
		{
			_pt._pj._near = fnear;
		}
		void set_pj_far(float ffar)
		{
			_pt._pj._far = ffar;
		}

		void set_trans_scalex(float sx)
		{
			_pt._trans_scale_x = sx;
		}
		void set_trans_scaley(float sy)
		{
			_pt._trans_scale_y = sy;
		}
		void set_trans_scalez(float sz)
		{
			_pt._trans_scale_z = sz;
		}

		void set_trans_rtx(float rx)
		{
			_pt._trans_rotation_x = rx;
		}
		void set_trans_rty(float ry)
		{
			_pt._trans_rotation_y = ry;
		}
		void set_trans_rtz(float rz)
		{
			_pt._trans_rotation_z = rz;
		}

		void set_trans_tlx(float tx)
		{
			_pt._trans_translation_x = tx;
		}
		void set_trans_tly(float ty)
		{
			_pt._trans_translation_y = ty;
		}
		void set_trans_tlz(float tz)
		{
			_pt._trans_translation_z = tz;
		}
		bool get_output_vertex(vector<float>& overtex);

	};
	REGISTER_CONTROL(ft_material_3d)
}
