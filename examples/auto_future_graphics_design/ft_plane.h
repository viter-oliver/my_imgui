#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_plane :
		public ft_base
	{
	public:
		ft_plane();
		~ft_plane();
	};

	REGISTER_CONTROL(ft_plane)
}