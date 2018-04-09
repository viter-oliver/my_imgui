#pragma once
#include "ft_base.h"
class af_model :
	public ft_base
{
public:
	af_model();
	~af_model();
};

REGISTER_CONTROL(af_model)