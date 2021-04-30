#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_plane_3d :
		public ft_base
	{
	public:
		ft_plane_3d();
		~ft_plane_3d();
	};

	REGISTER_CONTROL(ft_plane_3d)
}