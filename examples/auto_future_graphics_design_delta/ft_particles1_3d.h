#pragma once
#include "ft_base.h"
#include "material.h"
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
			(ImVec3, _pos0),
			(ImVec3, _v0),
			(ImVec3, _a0),
			(float, _life, { 5.f }),
			(float, _spread, { 1.5f }),
			(float, _y1, { 10.f }),
			(int, _pa, { en_normal }))
	public:
		ft_particles1_3d();
		~ft_particles1_3d();
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page(int property_part);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};

	REGISTER_CONTROL(ft_particles1_3d)
}