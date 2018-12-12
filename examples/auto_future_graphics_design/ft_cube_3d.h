#pragma once
#include "ft_base.h"
#include "basic3d.h"
#include "material.h"
#include "primitive_object.h"
namespace auto_future
{
	class AFG_EXPORT ft_cube_3d :
		public ft_base
	{
		DEF_STRUCT(intl_pt, _pt, 
			(ImVec3, _translation_shd), (ImVec3, _scale_stn), (ImVec3, _rotation_srd))
		/** referencing the color material */
		shared_ptr<material> _pmaterial;
		/** referencing the cube primitive object */
		shared_ptr<primitive_object> _pcube_prim;
	public:
		ft_cube_3d();
		~ft_cube_3d();
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

#endif
	};
	REGISTER_CONTROL(ft_cube_3d)
}