#pragma once
#include "basic3d.h"
class texture_shader :
	public basic_shader
{
public:
	texture_shader();
	//void render();
};
REG_SHADER(texture_shader);
