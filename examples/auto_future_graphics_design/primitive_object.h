
#pragma once
#include <map>
#include <string>
#include <memory>
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
struct primitive_object
{
	GLuint _vao,_vbo,_ebo;
	//float* _pvalue;
	//GLuint _value_len;
	primitive_object()
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
};
struct primitive_cube :public primitive_object
{

	static GLfloat _vertices[];
	static GLushort _indices[];
	primitive_cube();
};
typedef map<string, shared_ptr<primitive_object>> mp_primitive;
/**
*@brief put all primitive defined object into g_primitive_list
*/
void init_primitive_list();
/*!< the primitive object of g_primitive_list will be referenced by some ui components */
extern mp_primitive g_primitive_list;