#pragma once
#include "ft_base.h"
#include "af_material.h"
#include "af_primitive_object.h"
namespace auto_future
{
	class AFG_EXPORT ft_material_2d :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(intl_pt, _pt,
			(int, _draw_mode, {en_gl_triangles}),
			(char, _primitive_name[FILE_NAME_LEN]),
			(char, _material_name[FILE_NAME_LEN]))
		bool _matched{ false };
		ps_primrive_object _ps_prm;
		ps_mtl _ps_mtl;
	public:
		ft_material_2d();
		~ft_material_2d(){}
		void link();
		void draw();
	};
	REGISTER_CONTROL(ft_material_2d)
}
