#include "af_shader.h"
#include "SOIL.h"
#include "texture.h"
#include <GLFW/glfw3.h>
#include "user_control_imgui.h"
#include "paricles_system.h"
void Gravity::ApplyForce(GParticle& pgp)
{
	pgp._force = pgp._force + _GForce;
}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void Gravity::draw_property()
{
	ImGui::Text("Gravity accelerated velocity:");
	ImGui::SliderFloat("gax", &_GForce.x, -100.f, 100.f);
	ImGui::SliderFloat("gay", &_GForce.y, -100.f, 100.f);
	ImGui::SliderFloat("gaz", &_GForce.z, -100.f, 100.f);
}
#endif

void GViscosity::ApplyForce(GParticle& pgp)
{
	pgp._force = pgp._force - _Kd * pgp._vel;
}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void GViscosity::draw_property()
{
	ImGui::Text("Viscous constants:");
	ImGui::SliderFloat("kd", &_Kd, -100.f, 100.f);
}
#endif

void GSpring::ApplyForce(GParticle& pgp)
{
	int len = (int)SpringInfo.size();
	for (int i = 0; i < len; ++i)
	{
		GParticle *p0 = SpringInfo[i]->_P0;
		GParticle *p1 = SpringInfo[i]->_P1;
		glm::vec3 f0, f1;
		glm::vec3 l = p0->_pos - p1->_pos;
		glm::vec3 ldot = p0->_vel - p1->_vel;
		auto nml = glm::normalize(l);
		f0 = -(_Ks * (nml - SpringInfo[i]->_InitLen) + _Kd * ldot * l / nml) * l / nml;
		f1 = -f0;
		if (p0->is_live())
			p0->_force = p0->_force + f0;
		if (p1->is_live())
			p1->_force = p1->_force + f1;
	}
}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void GSpring::draw_property()
{
	ImGui::Text("damping coefficient:");
	ImGui::SliderFloat("dkd", &_Kd, -100.f, 100.f);
	ImGui::SliderFloat("ks", &_Ks, -100.f, 100.f);
}
#endif
GParticle* GPSystem::find_unused_particle()
{
	for (int ii = _last_used_particl_index; ii < _ParticleCnt; ii++){
		if (!_pParticle[ii].is_live()){
			_last_used_particl_index = ii;
			return &_pParticle[ii];
		}
	}
	for (int ii = 0; ii < _last_used_particl_index; ii++){
		if (!_pParticle[ii].is_live()){
			_last_used_particl_index = ii;
			return &_pParticle[ii];
		}
	}
	return NULL;
}
void GPSystem::EulerStep(const float &dt) //这就是上面说的是用欧拉方法来模拟粒子运动
{
	_valid_particl_cnt = 0;
	for (int ii = 0; ii < _ParticleCnt; ++ii)
	{
		GParticle& ptl = _pParticle[ii];
		if (ptl.is_live())
		{
			for (auto& gfs : _vforces)
			{
				gfs->ApplyForce(ptl);
			}
			ptl._life -= dt;
			glm::vec3 acl = ptl._force / ptl._mass;
			ptl._force = { 0, 0, 0 };
			ptl._vel += dt*acl*0.5f;
			ptl._pos += dt*ptl._vel;
			
			if (ptl._size < 30.f)
			{
				ptl._size++;
			}
			
			_pPos_data[4 * _valid_particl_cnt + 0] = ptl._pos.x;
			_pPos_data[4 * _valid_particl_cnt + 1] = ptl._pos.y;
			_pPos_data[4 * _valid_particl_cnt + 2] = ptl._pos.z;
			_pPos_data[4 * _valid_particl_cnt + 3] = ptl._size;
#ifdef _calcu_color

			_pColor_data[4 * _valid_particl_cnt + 0] = ptl.r;
			_pColor_data[4 * _valid_particl_cnt + 1] = ptl.g;
			_pColor_data[4 * _valid_particl_cnt + 2] = ptl.b;
			_pColor_data[4 * _valid_particl_cnt + 3] = ptl.a;
#endif
			_valid_particl_cnt++;
		}

	}
}
void GPSystem::spawn_particles(int cnt)
{
	for (int ii = 0; ii < cnt; ++ii)
	{
		GParticle* pnewpcl = find_unused_particle();
		if (!pnewpcl)
		{
			return;
		}
		if (_particle_initialize_hand)
		{
			_particle_initialize_hand(pnewpcl);
		}
		else
		{
			pnewpcl->_life = 5.f;
		}
	}
}