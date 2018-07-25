#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_sphere :
		public ft_base
	{
	public:
		ft_sphere();
		~ft_sphere();
	};
	REGISTER_CONTROL(ft_sphere)
}