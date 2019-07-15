#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_sphere_3d :
		public ft_base
	{
	public:
		ft_sphere_3d();
		~ft_sphere_3d();
	};
	REGISTER_CONTROL(ft_sphere_3d)
}