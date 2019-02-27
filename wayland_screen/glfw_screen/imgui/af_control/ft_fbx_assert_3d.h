#pragma once
#include "ft_base.h"
#include "basic3d.h"
#include "material.h"
#include "res_output.h"
#include "./fbx_save_info.h"
#include "primitive_object.h"
namespace auto_future{
	class AFG_EXPORT ft_fbx_assert_3d :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(intl_pt, _fbx_assert_pt,
			(int, _select_list, {0}),
			(af_vec3, _translation_shd),
			(af_vec3, _scale_stn),
			(af_vec3, _rotation_srd),
			(af_vec3, _camera_eye_shd),
			(af_vec3, _camera_center_stn),
			(af_vec3, _camera_up_srd),
			(float, _proj_fov_urd, {45.f}),
			(int, _proj_width, {800}),
			(int, _proj_height, {600}),
			(float, _proj_linear_uhd, {1.f}),
			(float, _proj_far_uhd, {100.f}),
			(af_vec3, _light_clr),
			(af_vec3, _light_pos_uhd))
			/** referencing the color material */
 
		
		shared_ptr<material> _pmaterial;

		shared_ptr<af_texture> _texture;
		/** referencing the cube primitive object */
		shared_ptr<primitive_object> _pcube_prim;

	public:
		ft_fbx_assert_3d();
		~ft_fbx_assert_3d();
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void init_internal_mesh_point();
#endif
	};
	REGISTER_CONTROL(ft_fbx_assert_3d)
}