#pragma once
#include "ft_base.h"
class ft_plane :
	public ft_base
{
public:
	ft_plane();
	~ft_plane();
};

REGISTER_CONTROL(ft_plane)