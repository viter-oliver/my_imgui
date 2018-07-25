#pragma once
#include "ft_base.h"
#include "basic3d.h"
#include "material.h"
#include "primitive_object.h"
namespace auto_future
{
	class AFG_EXPORT ft_cube :
		public ft_base
	{
		//tri_mesh _mesh;
		//render transformation
		struct intl_pt
		{
			ImVec3 _translation;
			ImVec3 _scale;
			ImVec3 _rotation;
			//int _shader_instance_index;
			intl_pt() :_scale(1.f, 1.f, 1.f) {}
		};
		intl_pt _pt;
		/** referencing the color material */
		shared_ptr<material> _pmaterial;
		/** referencing the cube primitive object */
		shared_ptr<primitive_object> _pcube_prim;
	public:
		ft_cube();
		~ft_cube();
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int len = sizeof(intl_pt);
			vplist.emplace_back(&_pt, len);
			len += plen;
			return len;
		}
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page();
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};
	REGISTER_CONTROL(ft_cube)
}