
#pragma once
#include <map>
#include <string>
#include <memory>
#include <vector>
#include "res_output.h"
#include "platform_def.h"
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif
using namespace std;
/**
* @brief  primitive_object represent basic data struct which will be inputed into\n
* a vertex shader  
*/
struct primitive_cube;
struct primitive_object
{
	GLuint _vao,_vbo,_ebo;
	vector<GLubyte> _ele_format;
	GLuint _vertex_buf_len;
	GLuint _ele_buf_len;
	string _file_name;
	ps_af_file _ps_file;
	bool _read_only{ true };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	bool _sel{ false };
#endif // !Im

	primitive_object()
		: _vertex_buf_len(0), _ele_buf_len(0)
	{
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
		glGenBuffers(1, &_ebo);
		
	}
	virtual ~primitive_object()
	{
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ebo);
		
	}
	GLubyte get_stride()
	{
		GLubyte gl_stride = 0;
		for (auto&ele:_ele_format)
		{
			gl_stride += ele;
		}
		return gl_stride;
	}
	void set_ele_format(vector<GLubyte> ele_fm)
	{
		_ele_format = ele_fm;
	}

	void load_vertex_data(GLfloat* pvertex_data, GLuint vetexlen, GLuint* pele_buff = 0, GLuint ele_cnt = 0);
	void enableVertex();
};
using ps_primrive_object = shared_ptr<primitive_object>;
typedef map<string, ps_primrive_object> mp_primitive;


/** the primitive object of g_primitive_list will be referenced by some ui components */
extern AFG_EXPORT mp_primitive g_primitive_list;
/**
*@brief refer a internal primitive object
*/
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
extern bool ref_a_intenal_primitive(string& prm_name);
#endif
extern ps_primrive_object AFG_EXPORT get_prm_object(const char* prm_name);