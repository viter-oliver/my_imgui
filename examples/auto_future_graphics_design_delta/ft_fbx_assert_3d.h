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
		struct intl_pt
		{
			int _select_list; //当前选择的fbx索引
			//model
			ImVec3 _translation;
			ImVec3 _scale;
			ImVec3 _rotation;
			//view
			ImVec3 _camera_eye;
			ImVec3 _camera_center;
			ImVec3 _camera_up;
			//proj
			float _proj_fov{45.f};
			int _proj_width{800};
			int _proj_height{600};
			float _proj_linear{1.f};
			float _proj_far{100.f};

			//light
			ImVec3 _light_color;
			ImVec3 _light_pos;

			intl_pt() : _select_list(0), _scale(1.f, 1.f, 1.f), _camera_center(0.f, 0.f, 0.f), _camera_up(0.f, 1.f, 0.f){}
		};
		intl_pt _fbx_assert_pt;
		/** referencing the color material */
		shared_ptr<material> _pmaterial;

		shared_ptr<af_texture> _texture;
		/** referencing the cube primitive object */
		shared_ptr<primitive_object> _pcube_prim;

	public:
		ft_fbx_assert_3d();
		~ft_fbx_assert_3d();
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int len = sizeof(intl_pt);
			vplist.emplace_back(&_fbx_assert_pt, len);
			len += plen;
			return len;
		}
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