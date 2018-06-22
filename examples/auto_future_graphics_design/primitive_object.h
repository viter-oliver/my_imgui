
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
	GLfloat* _pvertex_buf;
	GLuint _vertex_buf_len;
	GLushort* _pele_buf;
	GLushort _ele_buf_len;
	primitive_object()
		:_pvertex_buf(0), _vertex_buf_len(0), _pele_buf(0), _ele_buf_len(0)
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
		if (_pvertex_buf)
		{
			delete[] _pvertex_buf;
		}
		if (_pele_buf)
		{
			delete[] _pele_buf;
		}
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
	void set_ele_format(initializer_list<GLubyte> ele_fm)
	{
		_ele_format = ele_fm;
	}
	void prepare_vertex_data(GLfloat* pvertex_data, GLuint vetexlen, GLushort* pele_buff = 0, GLushort ele_cnt = 0)
	{
		_pvertex_buf = new GLfloat[vetexlen];
		_vertex_buf_len = vetexlen;
		int sz = sizeof(GLfloat)*_vertex_buf_len;
		printf("sz=%d\n", sz);

		memcpy(_pvertex_buf, pvertex_data, sizeof(GLfloat)*vetexlen);
		if (pele_buff)
		{
			_pele_buf = new GLushort[ele_cnt];
			_ele_buf_len = ele_cnt;

			memcpy(_pele_buf, pele_buff, ele_cnt*sizeof(GLushort));
		}
	}
	void load_vertex_data();
};

typedef map<string, shared_ptr<primitive_object>> mp_primitive;
/**
*@brief put all primitive defined object into g_primitive_list
*/
void init_internal_primitive_list();
/** the primitive object of g_primitive_list will be referenced by some ui components */
extern mp_primitive g_primitive_list;