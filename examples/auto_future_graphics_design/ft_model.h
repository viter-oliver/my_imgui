#pragma once
#include "ft_base.h"
class ft_model :
	public ft_base
{
public:
	ft_model();
	~ft_model();
};

REGISTER_CONTROL(ft_model)