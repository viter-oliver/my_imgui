#pragma once
#include "basic3d.h"
class color_shader :
	public basic_shader
{
public:
	color_shader();
	//void render();
};

REG_SHADER(color_shader);