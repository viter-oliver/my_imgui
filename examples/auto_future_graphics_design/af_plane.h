#pragma once
#include "ft_base.h"
class af_plane :
	public ft_base
{
public:
	af_plane();
	~af_plane();
};

REGISTER_CONTROL(af_plane)