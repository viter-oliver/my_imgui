#pragma once
#define _calcu_color
namespace auto_future
{
	//������
	struct GParticle
	{
		float _mass, _life, _size;
#ifdef _calcu_color
		unsigned char r, g, b, a;
#endif
		glm::vec3 _pos; //λ��
		glm::vec3 _vel; //�ٶ�
		glm::vec3 _force; //��
		GParticle() :_mass(4.f), _life(-1), _size(1.f){}
		bool is_live(){ return _life > 0.0f; }
	};

	class GForceObj
	{
	public:
		GForceObj(){}
		virtual ~GForceObj(){}
		virtual void ApplyForce(GParticle& pgp) = 0;
#if !defined(DISABLE_DEMO)
		virtual void draw_property(){}
#endif
	};
	//����
	class Gravity :public GForceObj
	{
	protected:
		glm::vec3 _GForce;

	public:
		Gravity(glm::vec3 force = glm::vec3(0.f, -9.8f, 0.f)) :_GForce(force){}
		virtual ~Gravity(){}
		void SetGravity(glm::vec3& force){ _GForce = force; }
		void ApplyForce(GParticle& pgp);
#if !defined(DISABLE_DEMO)
		void draw_property();

#endif
	};
	//
	//���������ӳ����յ�������ճ��ק����������Ӱ���⣬�������ܵ�������Ӱ��
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
	//ճ����
	class GViscosity :public GForceObj
	{
		float _Kd; //ճ�ͳ���
	public:
		GViscosity(const float &kd = 1.0) :_Kd(kd){}
		virtual ~GViscosity(){}

		void ApplyForce(GParticle& pgp);
#if !defined(DISABLE_DEMO)
		void draw_property();
#endif
	};


	//
	// ��������������Ϣ���࣬һ������������2�����ӣ��൱��һ������������2������
	// ������ϵͳ�У�����Ҫͨ����������趨���ɵ���Ȼ���ȣ������ӵ�����
	//
	struct  GSpringInfo
	{
		float _InitLen; //������Ȼ����
		GParticle *_P0; //����1
		GParticle *_P1; //����2
		GSpringInfo(){};
		GSpringInfo(GParticle* P0, GParticle* P1, float len) :_P0(P0), _P1(P1), _InitLen(len){}
	};
	//
	//����
	//
	class GSpring : public GForceObj
	{
		float _Ks, _Kd; //����ϵ��
		vector<GSpringInfo *> SpringInfo;
	public:
		GSpring(const double &Ks, const double &Kd) :_Ks(Ks), _Kd(Kd){}
		virtual ~GSpring(){}
		void AddSpringInfo(GSpringInfo* sif);
		void ApplyForce(GParticle& pgp);
#if !defined(DISABLE_DEMO)
		void draw_property();
#endif

	};

	//����ϵͳ
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
		void clear_particles() //������е���(����������Ϊ0)
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

		void EulerStep(const float &dt); //���������˵������ŷ��������ģ�������˶�


		void AddForce(shared_ptr<GForceObj> pf)//������������ϵͳ
		{
			_vforces.push_back(pf);
		}
		int GetNumParticle() const //��ȡ�����ܸ���
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
#endif
#if !defined(DISABLE_DEMO)
		void draw_property()
		{
			for (auto& af : _vforces)
			{
				af->draw_property();
			}
		}
#endif
	};
}
