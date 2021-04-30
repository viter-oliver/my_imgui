
#pragma once
#include <map>
#include <string>
#include <memory>
#include <vector>
#include "res_output.h"
#include "platform_def.h"
#include <GLFW/glfw3.h>
using namespace std;
/**
* @brief  primitive_object represent basic data struct which will be inputed into\n
* a vertex shader  
*/
enum
{
     en_gl_points,
     en_gl_lines,
     eg_gl_line_loop,
     en_gl_line_strip,
     en_gl_triangles,
     en_gl_triangle_strip,
     en_gl_triangle_fan,
     en_gl_count
};
enum en_trans_order
{
     en_translate_scale_rotate,
     en_translate_rotate_scale,
     en_scale_translate_rotate,
     en_scale_rotate_translate,
     en_rotate_translate_scale,
     en_rotate_scale_translate,
     en_trans_order_cnt
};
enum en_rotate_order
{
     en_rotate_x_y_z,
     en_rotate_x_z_y,
     en_rotate_y_x_z,
     en_rotate_y_z_x,
     en_rotate_z_x_y,
     en_rotate_z_y_x,
     en_rotate_order_cnt
};
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
extern const char* draw_mode[ en_gl_count ];
extern const char* str_trans_order[ en_trans_order_cnt ];
extern const char* str_rotate_oder[ en_rotate_order_cnt ];
#endif
struct primitive_cube;
struct primitive_object
{
	GLuint _vao,_vbo,_ebo;
	vector<GLubyte> _ele_format;
	GLuint _vertex_buf_len;
	GLuint _ele_buf_len;
	string _file_name;
	string _model_name;
	GLuint _mesh_id;
	ps_af_file _ps_file;
	GLuint _mem_usage;
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
	void load_vertex_data(GLfloat* pvertex_data, GLuint vetexlen, GLuint* pele_buff = 0, GLuint ele_cnt = 0, GLuint mem_usage = GL_STATIC_DRAW);
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
extern bool AFG_EXPORT get_prm_data(const char* prm_name,float* pbuff,uint32_t buff_len);
extern bool AFG_EXPORT update_prm_vbo(const char* prm_name, float* pvertex, uint32_t buff_len);