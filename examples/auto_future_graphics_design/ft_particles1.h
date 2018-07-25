#pragma once
#include "ft_base.h"
#include "material.h"
#include "res_output.h"
namespace auto_future
{
	class AFG_EXPORT ft_particles1 :
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
		struct pcl_intl
		{
			ImVec3 _pos0, _v0, _a0;
			float _life, _spread, _y1;
			particles_algorithm _pa;
			pcl_intl()
				:_pos0(0.f, 7.f, -20.f)
				, _v0(0.f, -7.f, 0.f)
				, _a0(0.f, 9.81f, 0.f)
				, _life(5.f), _spread(1.5f), _y1(10)
				, _pa(en_fire)
			{}
		};
		pcl_intl _pt;
	public:
		ft_particles1();
		~ft_particles1();
		void draw();
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int len = sizeof(pcl_intl);
			vplist.emplace_back(&_pt, len);
			len += plen;
			return len;
		}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page();
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};

	REGISTER_CONTROL(ft_particles1)
}