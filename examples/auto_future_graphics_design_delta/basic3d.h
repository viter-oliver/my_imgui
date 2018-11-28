#pragma once
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <vector>
#include <map>
#include <functional>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif
#include "fab.h"
using namespace fab;
using namespace std;
#define  FILE_NAME_LEN   50
enum en_attribute_type
{
	en_attr_float,
	en_attr_vec2,
	en_attr_vec3,
	en_attr_vec4,
	en_attr_type_count
};
struct shader_attribute
{
	en_attribute_type _type;
	GLuint _location;
	shader_attribute() :_type(en_attr_type_count), _location(0){}
	shader_attribute(en_attribute_type type, GLuint location) :_type(type), _location(location){}
};
typedef map<string,shader_attribute> mattr_list;

enum en_uniform_type
{
	en_unf_float,
	en_unf_vec2,
	en_unf_vec3,
	en_unf_vec4,
	en_unf_mat4,
	en_unf_tex,
	en_unf_count,

};
struct shader_uniform
{
	en_uniform_type _type;
	GLuint _location;
	shader_uniform() :_type(en_unf_count), _location(0){}
	shader_uniform(en_uniform_type type, GLuint location) :_type(type), _location(location){}
};
typedef map<string, shader_uniform> munf_list;

class basic_shader
{
protected:
	mattr_list _mattr_list;
	munf_list _munf_list;
	GLuint _shader_program_id, _vertex_shader, _fragment_shader;
	string _obj_name;
public:
	basic_shader(){}
	~basic_shader(){
		glDeleteProgram(_shader_program_id);
		glDeleteShader(_vertex_shader);
		glDeleteShader(_fragment_shader);
	}
	void use(){
		glUseProgram(_shader_program_id); 
	}
	mattr_list& get_attr_list(){ return _mattr_list; }
	munf_list& get_unf_list(){ return _munf_list; }
	string& objname(){ return _obj_name; }
	void set_obj_name(string& objn){ _obj_name = objn; }
	GLuint shader_program_id(){ return _shader_program_id; }
	bool loading_shader_attributes_from_avbo(GLuint vao, GLuint vbo, const GLvoid* pvertices, int cnt_vertices, vector<string>& attr_name_list, GLuint ebo = 0, GLushort* pindices = 0, int cnt_indics = 0);
	bool set_uniform(string& unf_name, GLsizei icnt,float* falue);
	bool set_uniform_text(string& unf_name, GLint v0){ return set_uniform(unf_name, v0,0); }
};

extern map<string, vector<unique_ptr<basic_shader>>> g_shader_list;
//extern Factory<basic_shader> g_shader_factory;
extern void instantiating_internal_shader();
extern  Factory<basic_shader>& get_shader_fc();
template<typename T>
struct shader_assist
{
	shader_assist(string key){ get_shader_fc().Register(key, []{return new T(); }); }
};
#define REG_SHADER(shd_type) static shader_assist<shd_type> reg_##shd_type(#shd_type)
