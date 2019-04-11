#pragma once
#include "ft_base.h"
#include "af_material.h"
#include "res_output.h"
namespace auto_future
{
	class AFG_EXPORT ft_particles1_3d :
		public ft_base
	{
		GLuint _vbo_uv, _vbo_pos, _vbo_color;
		GLuint _vao;
		shared_ptr<material> _particle_material;
		shared_ptr<af_texture> _texture;
		double lastTime;
		enum particles_algorithm
		{
			en_normal,
			en_gravity,
			en_fountain,
			en_fire,
			en_fire_with_smoke,
			en_alg_cnt
		};
		DEF_STRUCT_WITH_INIT(pcl_intl, _pt,
			(af_vec3, _pos0_shd),
			(af_vec3, _v0_shd),
			(af_vec3, _a0_shd),
			(float, _life, { 5.f }),
			(float, _spread, { 1.5f }),
			(float, _y1, { 10.f }),
			(int, _pa, { en_normal }))
	public:
		ft_particles1_3d();
		~ft_particles1_3d();
		void draw();

	};

	REGISTER_CONTROL(ft_particles1_3d)
}