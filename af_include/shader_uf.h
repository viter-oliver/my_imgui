#pragma once
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif 
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "json/json.h"
using namespace Json;
#endif

#include <stdexcept>
#define ASSERT_MESSAGE(condition, message) if (!(condition)) throw std::runtime_error(message);
#include <string>
#include "fab.h"
#include "af_type.h"
#include "res_output.h"
#include "platform_def.h"
using namespace fab;
using namespace std;
class AFG_EXPORT shader_uf
{
protected:
	GLuint _el_size;
	GLuint _usize;
	GLenum _utype;
public:
	string _unf_name;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	virtual void edit(string obj_name) = 0;
	virtual void output_2_json(Value& jvalue);
	virtual void init_from_json(Value& jvalue);
#endif
	virtual void* get_data_head() = 0;
	shader_uf(GLuint usize,GLuint el_sz) :_usize(usize),_el_size(el_sz){}
	void set_type(GLenum uft){ _utype = uft; }
	GLenum get_type(){ return _utype; }
	virtual ~shader_uf(){}
	virtual GLuint usize(){ return _usize; }
	virtual GLuint elsize(){ return _el_size; }
	virtual GLuint data_len(){ return _usize*_el_size; }
	virtual void link(){}
	virtual void set_to_loaction(GLuint location)=0;

};
struct fac_shader_uf
{
	template<typename T>
	struct shader_uf_assist
	{
		static T* create_new_obj(GLuint usize, GLuint el_sz)
		{
			return new T(usize, el_sz);
		}
		shader_uf_assist(string key)
		{
			get().Register(key, shader_uf_assist<T>::create_new_obj);
		}
	
	};
	static Factory<shader_uf>& get()
	{
		static Factory<shader_uf> instance;
		return instance;
	}
};

#define REG_SHADER_UF(shd_type) static fac_shader_uf::shader_uf_assist<shd_type> reg_##shd_type(#shd_type)


class AFG_EXPORT shader_uf_float :public shader_uf
{
	float* _pfvalue;
public:
	shader_uf_float(GLuint usize, GLuint el_sz) :shader_uf(usize, el_sz)
	{
		_pfvalue = new float[_usize*_el_size]; memset(_pfvalue, 0, _usize*_el_size*sizeof(float));
	}
	GLuint data_len(){ return _usize*_el_size*sizeof(float); }
	void* get_data_head()
	{
		return _pfvalue;
	}
	void set_to_loaction(GLuint location)
	{
		float* pview = _pfvalue;
		//for (int ii = 0; ii < 16; ++ii)
		//{
		//	printf("%f,", *pview++);
		//	if (ii % 4 == 3)
		//	{
		//		printf("\n");
		//	}
		//}
		switch (_utype)
		{
		case GL_FLOAT:
			glUniform1fv(location, _usize, _pfvalue);
			break;
		case GL_FLOAT_VEC2:
			glUniform2fv(location, _usize, _pfvalue);
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(location, _usize, _pfvalue);
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(location, _usize, _pfvalue);
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(location, _usize, GL_FALSE, _pfvalue);
			break;
		case GL_FLOAT_MAT2x3:
			glUniformMatrix2x3fv(location, _usize, GL_FALSE,_pfvalue);
			break;
		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(location, _usize, GL_FALSE, _pfvalue);
			break;
		case GL_FLOAT_MAT3x2:
			glUniformMatrix3x2fv(location, _usize, GL_FALSE,_pfvalue);
			break;
		case GL_FLOAT_MAT2x4:
			glUniformMatrix2x4fv(location, _usize, GL_FALSE,_pfvalue);
			break;
		case GL_FLOAT_MAT4x2:
			glUniformMatrix4x2fv(location, _usize, GL_FALSE,_pfvalue);
			break;
		case GL_FLOAT_MAT3x4:
			glUniformMatrix3x4fv(location, _usize, GL_FALSE,_pfvalue);
			break;
		case GL_FLOAT_MAT4x3:
			glUniformMatrix4x3fv(location, _usize, GL_FALSE,_pfvalue);
			break;
		default:
			printf("invalid type:%d\n", _utype);
			break;
		}
	}
	void set_fvalue(float* pfvalue, GLuint len)
	{

		GLuint rlen = min(len, _usize*_el_size);
		memcpy(_pfvalue, pfvalue, rlen*sizeof(float));
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
     void edit( string obj_name );
	void output_2_json(Value& jvalue);
	void init_from_json(Value& jvalue);
#endif

	~shader_uf_float()
	{
		delete[] _pfvalue;
	}
};
REG_SHADER_UF(shader_uf_float);
#if 0
class shader_uf_bool :public shader_uf
{
     bool* _pbvalue;
public:
     shader_uf_int( GLuint usize, GLuint el_sz ) :shader_uf( usize, el_sz )
     {
          _pivalue = new int[ _usize*_el_size ]; memset( _pivalue, 0, _usize*_el_size*sizeof( int ) );
     }
     GLuint data_len()
     {
          return _usize*_el_size*sizeof( int );
     }

     void* get_data_head()
     {
          return _pivalue;
     }
     void set_to_loaction( GLuint location )
     {
          switch( _utype )
          {
               case GL_BOOL:
                    glUniform1iv( location, _usize, _pbvalue );
                    break;
               case GL_BOOL_VEC2:
                    glUniform2iv( location, _usize, _pbvalue );
                    break;
               case GL_BOOL_VEC2:
                    glUniform3iv( location, _usize, _pbvalue );
                    break;
               case GL_BOOL_VEC4:
                    glUniform4iv( location, _usize, _pbvalue );
                    break;
               default:
                    printf( "invalid type:%d\n", _utype );
                    break;
          }
     }
     void set_bvalue( bool* pbvalue, GLuint len )
     {
          GLuint rlen = min( len, _usize*_el_size );
          memcpy( _pbvalue, pbvalue, rlen*sizeof( int ) );
     }
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
     void edit( string obj_name );
     void output_2_json( Value& jvalue );
     void init_from_json( Value& jvalue );
#endif

     ~shader_uf_bool()
     {
          delete[] _pbvalue;
     }
};
REG_SHADER_UF(shader_uf_bool );
#endif
class shader_uf_int :public shader_uf
{
	int* _pivalue;
public:
	shader_uf_int(GLuint usize, GLuint el_sz ) :shader_uf(usize, el_sz)
	{
		_pivalue = new int[_usize*_el_size]; memset(_pivalue, 0, _usize*_el_size*sizeof(int));
	}
	GLuint data_len(){ return _usize*_el_size*sizeof(int); }

	void* get_data_head()
	{
		return _pivalue;
	}
	void set_to_loaction(GLuint location)
	{
		switch (_utype)
		{
		case GL_INT:
			glUniform1iv(location, _usize, _pivalue);
			break;
		case GL_INT_VEC2:
			glUniform2iv(location, _usize, _pivalue);
			break;
		case GL_INT_VEC3:
			glUniform3iv(location, _usize, _pivalue);
			break;
		case GL_INT_VEC4:
			glUniform4iv(location, _usize, _pivalue);
			break;
		default:
			printf("invalid type:%d\n", _utype);
			break;
		}
	}
	void set_ivalue(int* pivalue, GLuint len)
	{
		GLuint rlen = min(len, _usize*_el_size);
		memcpy(_pivalue, pivalue, rlen*sizeof(int));
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
     void edit( string obj_name );
	void output_2_json(Value& jvalue);
	void init_from_json(Value& jvalue);
#endif

	~shader_uf_int()
	{
		delete[] _pivalue;
	}
};
REG_SHADER_UF(shader_uf_int);

class AFG_EXPORT shader_uf_uint :public shader_uf
{
	unsigned int* _puivalue;
public:

	shader_uf_uint(GLuint usize, GLuint el_sz) :shader_uf(usize, el_sz)
	{
		_puivalue = new unsigned int[_usize*_el_size]; memset(_puivalue, 0, _usize*_el_size*sizeof(unsigned int));
	}
	GLuint data_len(){ return _usize*_el_size*sizeof(unsigned int); }

	void* get_data_head()
	{
		return _puivalue;
	}
	void set_to_loaction(GLuint location)
	{
		switch (_utype)
		{
		case GL_UNSIGNED_INT:
			glUniform1uiv(location, _usize, _puivalue);
			break;
		case GL_UNSIGNED_INT_VEC2:
			glUniform2uiv(location, _usize, _puivalue);
			break;
		case GL_UNSIGNED_INT_VEC3:
			glUniform3uiv(location, _usize, _puivalue);
			break;
		case GL_UNSIGNED_INT_VEC4:
			glUniform4uiv(location, _usize, _puivalue);
			break;
		default:
			printf("invalid type:%d\n", _utype);
			break;
		}
	}
	void set_uivalue(unsigned int* puivalue, GLuint len)
	{
		GLuint rlen = min(len, _usize*_el_size);
		memcpy(_puivalue, puivalue, rlen*sizeof(unsigned int));
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
     void edit( string obj_name );
	void output_2_json(Value& jvalue);
	void init_from_json(Value& jvalue);
#endif

	~shader_uf_uint()
	{
		delete[] _puivalue;
	}
};
REG_SHADER_UF(shader_uf_uint);
class AFG_EXPORT shader_uf_double : public shader_uf
{
	double* _pdvalue;
public:
	shader_uf_double(GLuint usize, GLuint el_sz ) :shader_uf(usize, el_sz)
	{
		_pdvalue = new double[_usize*_el_size]; memset(_pdvalue, 0, _usize*_el_size*sizeof(double));
	}
	GLuint data_len(){ return _usize*_el_size*sizeof(double); }

	void* get_data_head()
	{
		return _pdvalue;
	}
	void set_to_loaction(GLuint location)
	{
		switch (_utype)
		{
		case GL_DOUBLE:
			glUniform1dv(location, _usize, _pdvalue);
			break;
		case GL_DOUBLE_VEC2:
			glUniform2dv(location, _usize, _pdvalue);
			break;
		case GL_DOUBLE_VEC3:
			glUniform3dv(location, _usize, _pdvalue);
			break;
		case GL_DOUBLE_VEC4:
			glUniform4dv(location, _usize, _pdvalue);
			break;
		case GL_DOUBLE_MAT4:
			glUniformMatrix4dv(location, _usize, GL_FALSE, _pdvalue);
			break;
		case GL_DOUBLE_MAT2x3:
			glUniformMatrix2x3dv(location, _usize, GL_FALSE, _pdvalue);
			break;
		case GL_DOUBLE_MAT3:
			glUniformMatrix3dv(location, _usize, GL_FALSE, _pdvalue);
			break;
		case GL_DOUBLE_MAT3x2:
			glUniformMatrix3x2dv(location, _usize, GL_FALSE, _pdvalue);
			break;
		case GL_DOUBLE_MAT2x4:
			glUniformMatrix2x4dv(location, _usize, GL_FALSE, _pdvalue);
			break;
		case GL_DOUBLE_MAT4x2:
			glUniformMatrix4x2dv(location, _usize, GL_FALSE, _pdvalue);
			break;
		case GL_DOUBLE_MAT3x4:
			glUniformMatrix3x4dv(location, _usize, GL_FALSE, _pdvalue);
			break;
		case GL_DOUBLE_MAT4x3:
			glUniformMatrix4x3dv(location, _usize, GL_FALSE, _pdvalue);
			break;
		default:
			printf("invalid type:%d\n", _utype);
			break;
		}
	}
	void set_dvalue(double* pdvalue, GLuint len)
	{
		GLuint rlen = min(len, _usize*_el_size);
		memcpy(_pdvalue, pdvalue, rlen*sizeof(double));
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
     void edit( string obj_name );
	void output_2_json(Value& jvalue);
	void init_from_json(Value& jvalue);
#endif

	~shader_uf_double()
	{
		delete[] _pdvalue;
	}
};
REG_SHADER_UF(shader_uf_double);

class AFG_EXPORT shader_uf_txt :public shader_uf
{
	char _txt_name[FILE_NAME_LEN];
	ps_af_texture _pdtxt;
	static GLuint   _sample_index;
public:
	shader_uf_txt(GLuint usize, GLuint el_sz) :shader_uf(usize, el_sz)
	{
		memset(_txt_name, 0, FILE_NAME_LEN);
		_pdtxt = nullptr;
	}
	static void reset_sample_index()
	{
		_sample_index = 0;
	}

	void set_to_loaction(GLuint location)
	{
		if (!_pdtxt)
		{
			return;
		}
		glActiveTexture(GL_TEXTURE0 + _sample_index);
		glBindTexture(GL_TEXTURE_2D, _pdtxt->_atxt_id);
		glUniform1i(location, _sample_index);
		_sample_index++;
		/*
		switch (_utype)
		{
		case GL_SAMPLER_2D:
		case GL_SAMPLER_1D:
		case GL_SAMPLER_3D:

			glUniform1i(location, _sample_index);
			break;

		default:
			printf("invalid type:%d\n", _utype);
			break;
		}*/
	}
	GLuint data_len(){ 
		return strlen(_txt_name); 
	}

	void* get_data_head()
	{
		return &_txt_name[0];
	}
	GLuint usize(){ return 1; }
	GLuint elsize(){ return strlen(_txt_name); }
	void link();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
     void edit( string obj_name );
	void output_2_json(Value& jvalue);
	void init_from_json(Value& jvalue);
#endif

	~shader_uf_txt()
	{
	}
};
REG_SHADER_UF(shader_uf_txt);