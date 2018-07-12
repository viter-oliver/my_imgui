#pragma once
#define _calcu_color
//粒子类
struct GParticle
{
	float _mass, _life, _size;
#ifdef _calcu_color
	unsigned char r, g, b, a;
#endif
	glm::vec3 _pos; //位置
	glm::vec3 _vel; //速度
	glm::vec3 _force; //力
	GParticle() :_mass(4.f), _life(-1), _size(1.f){}
	bool is_live(){ return _life > 0.0f; }
};

class GForceObj
{
public:
	GForceObj(){}
	virtual ~GForceObj(){}
	virtual void ApplyForce(GParticle& pgp) = 0;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	virtual void draw_property(){}
#endif
};
//重力
class Gravity :public GForceObj
{
protected:
	glm::vec3 _GForce;

public:
	Gravity(glm::vec3& force = glm::vec3(0.f, -9.8f, 0.f)) :_GForce(force){}
	virtual ~Gravity(){}
	void SetGravity(glm::vec3& force){ _GForce = force; }
	void ApplyForce(GParticle& pgp);
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void draw_property();
	
#endif
};
//
//外力，粒子除了收到重力，粘滞拽力，弹力的影响外，还可以受到外力的影响
//
class GExtForce : public Gravity
{
public:
	GExtForce(glm::vec3& force) :Gravity(force){}
	virtual ~GExtForce(){}
	void ApplyForce(GParticle& pgp)
	{
		Gravity::ApplyForce(pgp);
		_GForce.x = 0.f;
		_GForce.y = 0.f;
		_GForce.z = 0.f;
	}
};
//粘滞力
class GViscosity :public GForceObj
{
	float _Kd; //粘滞常数
public:
	GViscosity(const float &kd = 1.0) :_Kd(kd){}
	virtual ~GViscosity(){}

	void ApplyForce(GParticle& pgp);
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void draw_property();
#endif
};


//
// 关于描述弹力信息的类，一个弹力连接了2个粒子，相当于一个弹簧连接了2个粒子
// 在粒子系统中，我们要通过这个类来设定弹簧的自然长度，和连接的粒子
//
struct  GSpringInfo
{
	float _InitLen; //弹簧自然长度
	GParticle *_P0; //粒子1
	GParticle *_P1; //粒子2
	GSpringInfo(){};
	GSpringInfo(GParticle* P0, GParticle* P1, float len) :_P0(P0), _P1(P1), _InitLen(len){}
};
//
//弹力
//
class GSpring : public GForceObj
{
	float _Ks, _Kd; //阻尼系数
	vector<GSpringInfo *> SpringInfo;
public:
	GSpring(const double &Ks, const double &Kd) :_Ks(Ks), _Kd(Kd){}
	virtual ~GSpring(){}
	void AddSpringInfo(GSpringInfo* sif);
	void ApplyForce(GParticle& pgp);
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void draw_property();
#endif

};

//粒子系统
//
class GPSystem
{
	GParticle* _pParticle{ NULL };
	int _ParticleCnt{ 0 };
	GLfloat* _pPos_data{ NULL };
#ifdef _calcu_color
	GLubyte* _pColor_data{ NULL };
#endif
	int _valid_particl_cnt{ 0 };
	vector<shared_ptr<GForceObj>> _vforces;
	int _last_used_particl_index{ 0 };
	function<void(GParticle*)> _particle_initialize_hand;
public:
	GPSystem(){}
	~GPSystem()
	{
		clear_particles();
	}
	void create_paricle(int cnt)
	{
		clear_particles();
		_ParticleCnt = cnt;
		_pParticle = new GParticle[cnt];
		_pPos_data = new GLfloat[cnt * 4];
#ifdef _calcu_color
		_pColor_data = new unsigned char[cnt * 4];
#endif
	}
	void add_particle_set(function<void(GParticle*)> phandle)
	{
		_particle_initialize_hand = phandle;
	}
	void clear_particles() //清空所有的力(设置所有力为0)
	{
		if (_pParticle)
		{
			delete[] _pParticle;
			delete[] _pPos_data;
#ifdef _calcu_color
			delete[] _pColor_data;
#endif
		}


		_pParticle = 0;
		_ParticleCnt = 0;
		_valid_particl_cnt = 0;
	}

	void EulerStep(const float &dt); //这就是上面说的是用欧拉方法来模拟粒子运动
	

	void AddForce(shared_ptr<GForceObj> pf)//添加力到粒子系统
	{
		_vforces.push_back(pf);
	}
	int GetNumParticle() const //获取粒子总个数
	{
		return _ParticleCnt;
	}
	GParticle* find_unused_particle();
	void spawn_particles(int cnt);
	int valid_particle_cnt()
	{
		return _valid_particl_cnt;
	}
	GLfloat* get_pPos_data()
	{
		return _pPos_data;
	}
	int valid_data_count()
	{
		return _valid_particl_cnt * 4;
	}
#ifdef _calcu_color
	GLubyte* get_pCol_data()
	{
		return _pColor_data;
	}
#endif;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void draw_property()
	{
		for (auto& af : _vforces)
		{
			af->draw_property();
		}
	}
#endif
};
