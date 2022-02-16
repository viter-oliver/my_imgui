#pragma once
#include "ft_base.h"
#include "af_material.h"
#include "af_primitive_object.h"
namespace auto_future
{
	class AFG_EXPORT ft_cube_3d :
		public ft_base
	{
		DEF_STRUCT_WITH_INIT(intl_pt, _pt, 
			(af_vec3, _translation_shd),
			(af_vec3, _scale_stn),
			(af_vec3, _rotation_srd))
		/** referencing the color material */
		shared_ptr<material> _pmaterial;
		/** referencing the cube primitive object */
		shared_ptr<primitive_object> _pcube_prim;
	public:
		ft_cube_3d();
		~ft_cube_3d();
		void draw();

	};
	REGISTER_CONTROL(ft_cube_3d)
}