
#pragma once
#include <map>
#include <string>
#include <memory>
#include <vector>
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
};

typedef map<string, shared_ptr<primitive_object>> mp_primitive;
/**
*@brief put all primitive defined object into g_primitive_list
*/
void init_internal_primitive_list();
/** the primitive object of g_primitive_list will be referenced by some ui components */
extern mp_primitive g_primitive_list;