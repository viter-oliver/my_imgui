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
			(ImVec3, _translation),
			(ImVec3, _scale),
			(ImVec3, _rotation),
			(ImVec3, _camera_eye),
			(ImVec3, _camera_center),
			(ImVec3, _camera_up),
			(float, _proj_fov, {45.f}),
			(int, _proj_width, {800}),
			(int, _proj_height, {600}),
			(float, _proj_linear, {1.f}),
			(float, _proj_far, {100.f}),
			(ImVec3, _light_color),
			(ImVec3, _light_pos))
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
		void draw_peroperty_page(int property_part = -1);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);

		void init_internal_mesh_point();
#endif
	};
	REGISTER_CONTROL(ft_fbx_assert_3d)
}