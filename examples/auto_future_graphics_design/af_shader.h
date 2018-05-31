#pragma once
/*
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"*/
#include <vector>
#include <map>
#include <functional>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif
#include "fab.h"
using namespace fab;
using namespace std;
struct shader_variable
{
	GLenum _variable_type;
	GLuint _location;
	shader_variable(){}
	shader_variable(GLenum vtp, GLuint loc) :_variable_type(vtp), _location(loc){}
};
typedef map<string, shader_variable> mshader_variable_list;
class af_shader
{
	mshader_variable_list _att_list;
	mshader_variable_list _unf_list;
	GLuint _shader_program_id, _vertex_shader, _fragment_shader;
	string _obj_name;
	bool _valid;
public:
	af_shader(const GLchar* vertex_shader_source, const GLchar* fragment_shader_source);
	~af_shader();
	bool is_valid(){ return _valid; }
	void use(){ glUseProgram(_shader_program_id); }
	bool vertex_att_pointer(initializer_list<string> att_name_list);
	//template<typename T> bool uniform(string unf_name, GLsizei icnt, T* pvalue);
	bool uniform(string unf_name, GLsizei icnt, float* pvalue);
	bool uniform(string unf_name, GLsizei icnt, int* pvalue);
	bool uniform(string unf_name, GLsizei icnt, double* pvalue);

	bool uniform(string unf_name, int ivalue);

};

