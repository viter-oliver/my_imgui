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
/**
* @brief basic variable type of the shader  \n
*/

struct shader_variable
{
	GLenum _variable_type;
	GLuint _location;
	GLuint _size;
	shader_variable(){}
	shader_variable(GLenum vtp, GLuint loc, GLuint size) :_variable_type(vtp), _location(loc),_size(size){}
}; 
struct attribute_variable
{
	string _name;
	GLenum _variable_type;
	GLuint _location;
	GLuint _size;
};
using attr_vect = vector<attribute_variable>;
typedef map<string, shader_variable> mshader_variable_list;
/**
* @brief  af_shader, process unit of gpu for rendering ui components \n
*/
class af_shader
{
	/** all the attribute variables  of shader */
	attr_vect _att_list;
     /** all the out variables of shader*/
     attr_vect _out_list;
	/** all the type of uniform variables of shader */
	mshader_variable_list _unf_list;
	GLuint _shader_program_id = { 0 };
	string _vs_code, _fs_code;
     GLuint  _vertex_shader = { 0 };
     GLuint _frag_shader = { 0 };
	string _name;
	/** whether shader is valid */
	/** vertex shader source code and fragment shader source code */
	bool _valid{ false };
     //bool _be_feedback { false };
public:
	bool _vs_code_valid{ false }, _fs_code_valid{ false };
	af_shader(GLenum binFormat, void* bin, GLsizei binLength);
	~af_shader();
	string get_name() { return _name; }
	void set_name(string name){ _name = name; };
	GLuint program_id()
	{
		return _shader_program_id;
	}
	mshader_variable_list& get_uf_defs(){ return _unf_list; }
	
	void refresh_viarable_list();
	af_shader(const GLchar* vertex_shader_source, const GLchar* fragment_shader_source);
	
	bool build_vs_code(string& vs_code);
	bool build_fs_code(string& fs_code);
	void link();
     //void specify_transfeedback(bool relink=false);
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	string& get_vs_code(){ return _vs_code; }
	string& get_fs_code(){ return _fs_code; }

public:
	void refresh_sourcecode(string& vertex_shader_source, string& fragment_shader_source);
	string compile_error_info;
	string _vs_name, _fs_name;
	bool _read_only{ false };
	bool _vs_selected{ false };
	bool _fs_selected{ false };
	void reset_sel(){ _vs_selected = _fs_selected = false; }
#endif
	bool match_format(vector<GLubyte>& fmt);
     attr_vect& get_attr_list()
     {
          return _att_list;
     }
	bool is_valid(){ return _valid; }
	void use(){ 
		glUseProgram(_shader_program_id); 

	}
	bool vertex_att_pointer();
	//template<typename T> bool uniform(string unf_name, GLsizei icnt, T* pvalue);
	bool uniform(string unf_name, float* pvalue);
	bool uniform(string unf_name, int* pvalue);
	bool uniform(string unf_name, double* pvalue);
	bool uniform(string unf_name, int ivalue);
};
using ps_shader = shared_ptr<af_shader>;
typedef map<string,ps_shader> maf_shader;
extern maf_shader g_af_shader_list;