#pragma once
#include "ft_base.h"
#include "af_material.h"
#include "res_output.h"
namespace auto_future
{
	class AFG_EXPORT ft_particles_3d :
		public ft_base
	{
		GLuint _vbo_uv, _vbo_pos, _vbo_color;
		GLuint _vao;
		ps_shader _ps_sd_particle;
		shared_ptr<af_texture> _texture;
		double lastTime;
		enum particles_algorithm
		{
			en_normal,
			en_gravity,
			en_fountain,
			en_fire,
			en_fire_with_smoke,
		};

		DEF_STRUCT_WITH_INIT(pcl_intl,_pt,
			(af_vec3, _pos0_shd),
			(af_vec3, _v0_shd),
			(af_vec3, _a0_shd),
			(char, _txt_particle_name[FILE_NAME_LEN]),
			(float, _life, {5.f}),
			(int, _pa, {en_normal}))
	public:
		ft_particles_3d();
		~ft_particles_3d();
		void link();
		void draw();

	};

	REGISTER_CONTROL(ft_particles_3d)
}