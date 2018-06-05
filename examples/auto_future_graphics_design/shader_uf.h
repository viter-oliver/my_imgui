#pragma once
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif 
#include <stdexcept>
#define ASSERT_MESSAGE(condition, message) if (!(condition)) throw std::runtime_error(message);

class shader_uf
{
protected:
	GLuint _usize;
public:
	shader_uf() :_usize(1){}
	shader_uf(GLuint usize) :_usize(usize){}
	GLuint usize(){ return _usize; }
	virtual void set_to_loaction(GLuint location)=0;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	virtual void edit() = 0;
#endif
};

class shader_uf_float :public shader_uf
{
	float* _pfvalue;
public:
	shader_uf_float() :shader_uf(){ _pfvalue = new float[_usize]; }
	shader_uf_float(GLuint usize) :shader_uf(usize){ _pfvalue = new float[usize]; }
	void set_to_loaction(GLuint location)
	{
		glUniform1fv(location, _usize, _pfvalue);
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	virtual void edit();
#endif
	float& operator[](int index)
	{
		ASSERT_MESSAGE(index < _usize,"invalid index")
		return _pfvalue[index];
	}
	~shader_uf_float()
	{
		delete[] _pfvalue;
	}
};

class shader_uf_int :public shader_uf
{
	int* _pivalue;
public:
	shader_uf_int() :shader_uf(1){ _pivalue = new int[_usize]; }
	shader_uf_int(GLuint usize) :shader_uf(usize){ _pivalue = new int[_usize]; }
	void set_to_loaction(GLuint location)
	{
		glUniform1iv(location,_usize, _pivalue);
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	virtual void edit();
#endif
	int& operator[](int index)
	{
		ASSERT_MESSAGE(index < _usize, "invalid index")
		return _pivalue[index];
	}
	~shader_uf_int()
	{
		delete[] _pivalue;
	}
};

class shader_uf_uint :public shader_uf
{
	unsigned int* _puivalue;
public:

	shader_uf_uint() :shader_uf(1){ _puivalue = new unsigned int[_usize]; }
	shader_uf_uint(GLuint usize) :shader_uf(usize){ _puivalue = new unsigned int[_usize]; }
	void set_to_loaction(GLuint location)
	{
		glUniform1uiv(location, _usize, _puivalue);
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	virtual void edit();
#endif
	unsigned int& operator[](int index)
	{
		ASSERT_MESSAGE(index < _usize, "invalid index")
		return _puivalue[index];
	}
	~shader_uf_uint()
	{
		delete[] _puivalue;
	}
};

class shader_uf_double :public shader_uf
{
	double* _pdvalue;
public:
	shader_uf_double() :shader_uf(1){ _pdvalue = new double[_usize]; }
	shader_uf_double(GLuint usize) :shader_uf(usize){ _pdvalue = new double[_usize]; }
	void set_to_loaction(GLuint location)
	{
		glUniform1dv(location, _usize, _pdvalue);
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	virtual void edit();
#endif
	double& operator[](int index)
	{
		ASSERT_MESSAGE(index < _usize, "invalid index")
		return _pdvalue[index];
	}
	~shader_uf_double()
	{
		delete[] _pdvalue;
	}
};
/*
class shader_uf
{
	GLenum _type;
public:
	union 
	{
		float fvalue;
		float fvec2[2];
		float fvec3[3];
		float fvec4[4];
		float fmat3[9];
		float fmat4[16];
		float fmat2_3[6];
		float fmat2_4[8];
		float fmat3_4[12];

		double dvalue;
		double dvec2[2];
		double dvec3[3];
		double dvec4[4];
		double dmat3[9];
		double dmat4[16];
		double dmat2_3[6];
		double dmat2_4[8];
		double dmat3_4[12];

		int ivalue;
		int ivec2[2];
		int ivec3[3];
		int ivec4[4];
		

		unsigned int uivalue;
		unsigned int uivec2[2];
		unsigned int uivec3[3];
		unsigned int uivec4[4];
		
	} uf_value;
public:
	shader_uf() :_type(GL_DOUBLE_MAT4)
	{
		memset(uf_value.dmat4, 0, 16 * sizeof(double));
	}
	shader_uf(GLenum tp) :_type(tp)
	{
		memset(uf_value.dmat4, 0, 16 * sizeof(double));
	}
	GLenum value_type(){ return _type; }
	shader_uf(float fv) :_type(GL_FLOAT){ uf_value.fvalue = fv; }
	shader_uf(float fv0, float fv1) :_type(GL_FLOAT_VEC2){ uf_value.fvec2[0] = fv0;uf_value.fvec2[1] = fv1; }
	shader_uf(float fv0, float fv1,float fv2):_type(GL_FLOAT_VEC3)
	{ 
		uf_value.fvec3[0] = fv0;uf_value.fvec3[1] = fv1;uf_value.fvec3[2] = fv2; 
	}
	shader_uf(float fv0, float fv1,float fv2,float fv3) :_type(GL_FLOAT_VEC4)
	{ 
		uf_value.fvec4[0] = fv0;uf_value.fvec4[1] = fv1;uf_value.fvec4[2] = fv2;uf_value.fvec4[3] = fv3; 
	}
	shader_uf(float fv0, float fv1, float fv2, float fv3, float fv4, float fv5, float fv6, float fv7, float fv8) :_type(GL_FLOAT_MAT3)
	{
		uf_value.fmat3[0] = fv0; uf_value.fmat3[1] = fv1; uf_value.fmat3[2] = fv2; uf_value.fmat3[3] = fv3;
		uf_value.fmat3[4] = fv4; uf_value.fmat3[5] = fv5; uf_value.fmat3[6] = fv6; uf_value.fmat3[7] = fv7;	uf_value.fmat3[8] = fv8;
	}
	shader_uf(float fv0, float fv1, float fv2, float fv3,
		float fv4, float fv5, float fv6, float fv7, 
		float fv8, float fv9, float fv10, float fv11,
		float fv12, float fv13, float fv14, float fv15) :_type(GL_FLOAT_MAT4)
	{ 
		uf_value.fmat4[0] = fv0; uf_value.fmat4[1] = fv1; uf_value.fmat4[2] = fv2; uf_value.fmat4[3] = fv3;
		uf_value.fmat4[4] = fv4; uf_value.fmat4[5] = fv5; uf_value.fmat4[6] = fv6; uf_value.fmat4[7] = fv7;
		uf_value.fmat4[8] = fv8; uf_value.fmat4[9] = fv9; uf_value.fmat4[10] = fv10; uf_value.fmat4[11] = fv11;
		uf_value.fmat4[12] = fv12; uf_value.fmat4[13] = fv13; uf_value.fmat4[14] = fv14; uf_value.fmat4[15] = fv15;
	}
	shader_uf(float fv0, float fv1, float fv2, float fv3,
		float fv4, float fv5) :_type(GL_FLOAT_MAT2x3)
	{ 
		uf_value.fmat2_3[0] = fv0; uf_value.fmat2_3[1] = fv1; uf_value.fmat2_3[2] = fv2; uf_value.fmat2_3[3] = fv3;
		uf_value.fmat2_3[4] = fv4; uf_value.fmat2_3[5] = fv5;
	}
	shader_uf(float fv0, float fv1, float fv2, float fv3,
		float fv4, float fv5, float fv6, float fv7) :_type(GL_FLOAT_MAT2x4)
	{ 
		uf_value.fmat2_4[0] = fv0; uf_value.fmat2_4[1] = fv1; uf_value.fmat2_4[2] = fv2; uf_value.fmat2_4[3] = fv3;
		uf_value.fmat2_4[4] = fv4; uf_value.fmat2_4[5] = fv5; uf_value.fmat2_4[6] = fv6; uf_value.fmat2_4[7] = fv7;
	}
	shader_uf(float fv0, float fv1, float fv2, float fv3,
		float fv4, float fv5, float fv6, float fv7, 
		float fv8, float fv9, float fv10, float fv11) :_type(GL_FLOAT_MAT3x4)
	{ 
		uf_value.fmat3_4[0] = fv0; uf_value.fmat3_4[1] = fv1; uf_value.fmat3_4[2] = fv2; uf_value.fmat3_4[3] = fv3;
		uf_value.fmat3_4[4] = fv4; uf_value.fmat3_4[5] = fv5; uf_value.fmat3_4[6] = fv6; uf_value.fmat3_4[7] = fv7;
		uf_value.fmat3_4[8] = fv8; uf_value.fmat3_4[9] = fv9; uf_value.fmat3_4[10] = fv10; uf_value.fmat3_4[11] = fv11;
	}

	shader_uf(double dv) :_type(GL_DOUBLE){ uf_value.dvalue = dv; }
	shader_uf(double dv0, double dv1) :_type(GL_DOUBLE_VEC2){ uf_value.dvec2[0] = dv0; uf_value.dvec2[1] = dv1; }
	shader_uf(double dv0, double dv1, double dv2) :_type(GL_DOUBLE_VEC3)
	{
		uf_value.dvec3[0] = dv0; uf_value.dvec3[1] = dv1; uf_value.dvec3[2] = dv2;
	}
	shader_uf(double dv0, double dv1, double dv2, double dv3) :_type(GL_DOUBLE_VEC4)
	{
		uf_value.dvec4[0] = dv0; uf_value.dvec4[1] = dv1; uf_value.dvec4[2] = dv2; uf_value.dvec4[3] = dv3;
	}
	shader_uf(double dv0, double dv1, double dv2, double dv3, double dv4, double dv5, double dv6, double dv7, double dv8) :_type(GL_DOUBLE_MAT3)
	{
		uf_value.dmat3[0] = dv0; uf_value.dmat3[1] = dv1; uf_value.dmat3[2] = dv2; uf_value.dmat3[3] = dv3;
		uf_value.dmat3[4] = dv4; uf_value.dmat3[5] = dv5; uf_value.dmat3[6] = dv6; uf_value.dmat3[7] = dv7;	uf_value.dmat3[8] = dv8;
	}
	shader_uf(double dv0, double dv1, double dv2, double dv3,
		double dv4, double dv5, double dv6, double dv7,
		double dv8, double dv9, double dv10, double dv11,
		double dv12, double dv13, double dv14, double dv15) :_type(GL_DOUBLE_MAT4)
	{
		uf_value.dmat4[0] = dv0; uf_value.dmat4[1] = dv1; uf_value.dmat4[2] = dv2; uf_value.dmat4[3] = dv3;
		uf_value.dmat4[4] = dv4; uf_value.dmat4[5] = dv5; uf_value.dmat4[6] = dv6; uf_value.dmat4[7] = dv7;
		uf_value.dmat4[8] = dv8; uf_value.dmat4[9] = dv9; uf_value.dmat4[10] = dv10; uf_value.dmat4[11] = dv11;
		uf_value.dmat4[12] = dv12; uf_value.dmat4[13] = dv13; uf_value.dmat4[14] = dv14; uf_value.dmat4[15] = dv15;
	}
	shader_uf(double dv0, double dv1, double dv2, double dv3,
		double dv4, double dv5) :_type(GL_DOUBLE_MAT2x3)
	{
		uf_value.dmat2_3[0] = dv0; uf_value.dmat2_3[1] = dv1; uf_value.dmat2_3[2] = dv2; uf_value.dmat2_3[3] = dv3;
		uf_value.dmat2_3[4] = dv4; uf_value.dmat2_3[5] = dv5;
	}
	shader_uf(double dv0, double dv1, double dv2, double dv3,
		double dv4, double dv5, double dv6, double dv7) :_type(GL_DOUBLE_MAT2x4)
	{
		uf_value.dmat2_4[0] = dv0; uf_value.dmat2_4[1] = dv1; uf_value.dmat2_4[2] = dv2; uf_value.dmat2_4[3] = dv3;
		uf_value.dmat2_4[4] = dv4; uf_value.dmat2_4[5] = dv5; uf_value.dmat2_4[6] = dv6; uf_value.dmat2_4[7] = dv7;
	}
	shader_uf(double dv0, double dv1, double dv2, double dv3,
		double dv4, double dv5, double dv6, double dv7,
		double dv8, double dv9, double dv10, double dv11) :_type(GL_DOUBLE_MAT3x4)
	{
		uf_value.dmat3_4[0] = dv0; uf_value.dmat3_4[1] = dv1; uf_value.dmat3_4[2] = dv2; uf_value.dmat3_4[3] = dv3;
		uf_value.dmat3_4[4] = dv4; uf_value.dmat3_4[5] = dv5; uf_value.dmat3_4[6] = dv6; uf_value.dmat3_4[7] = dv7;
		uf_value.dmat3_4[8] = dv8; uf_value.dmat3_4[9] = dv9; uf_value.dmat3_4[10] = dv10; uf_value.dmat3_4[11] = dv11;
	}


	shader_uf(int iv) :_type(GL_INT){ uf_value.ivalue = iv; }
	shader_uf(int iv0, int iv1) :_type(GL_INT_VEC2){ uf_value.ivec2[0] = iv0; uf_value.ivec2[1] = iv1; }
	shader_uf(int iv0, int iv1, int iv2) :_type(GL_INT_VEC3)
	{
		uf_value.ivec3[0] = iv0; uf_value.ivec3[1] = iv1; uf_value.ivec3[2] = iv2;
	}
	shader_uf(int iv0, int iv1, int iv2, int iv3) :_type(GL_INT_VEC4)
	{
		uf_value.ivec4[0] = iv0; uf_value.ivec4[1] = iv1; uf_value.ivec4[2] = iv2; uf_value.ivec4[3] = iv3;
	}

	//shader_uf(unsigned int uiv) :_type(GL_UNSIGNED_INT){ uf_value.uivalue = uiv; }
	shader_uf(unsigned int uiv0, unsigned int uiv1) :_type(GL_UNSIGNED_INT_VEC2){ uf_value.uivec2[0] = uiv0; uf_value.uivec2[1] = uiv1; }
	shader_uf(unsigned int uiv0, unsigned int uiv1, unsigned int uiv2) :_type(GL_UNSIGNED_INT_VEC3)
	{
		uf_value.uivec3[0] = uiv0; uf_value.uivec3[1] = uiv1; uf_value.uivec3[2] = uiv2;
	}
	shader_uf(unsigned int uiv0, unsigned int uiv1, unsigned int uiv2, unsigned int uiv3) :_type(GL_UNSIGNED_INT_VEC4)
	{
		uf_value.uivec4[0] = uiv0; uf_value.uivec4[1] = uiv1; uf_value.uivec4[2] = uiv2; uf_value.uivec4[3] = uiv3;
	}

};
*/
