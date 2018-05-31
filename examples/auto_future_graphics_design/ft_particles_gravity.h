#pragma once
#include "ft_base.h"
#include "af_shader.h"

class ft_particles_gravity :
	public ft_base
{
	GLuint _vbo_uv,_vbo_pos,_vbo_color;
	GLuint _txt_unit;
	GLuint _vao;
	shared_ptr<af_shader> _particle_shader;
	double lastTime;
public:
	ft_particles_gravity();
	~ft_particles_gravity();
	void draw();
};

REGISTER_CONTROL(ft_particles_gravity)