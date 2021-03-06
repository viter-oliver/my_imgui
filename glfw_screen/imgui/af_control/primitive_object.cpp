#include "primitive_object.h"
const float half_side_len = 0.25f;

GLfloat _vertices[] = {
	-half_side_len, -half_side_len, -half_side_len, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	half_side_len, -half_side_len, -half_side_len, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	half_side_len, -half_side_len, half_side_len, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	-half_side_len, -half_side_len, half_side_len, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	-half_side_len, half_side_len, -half_side_len, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	half_side_len, half_side_len, -half_side_len, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	half_side_len, half_side_len, half_side_len, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-half_side_len, half_side_len, half_side_len, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
};

GLushort _indices[] = {
	0, 4, 5,
	5, 1, 0,

	3, 2, 6,
	6, 7, 3,

	7, 4, 0,
	0, 3, 7,

	6, 2, 1,
	1, 5, 6,

	0, 1, 2,
	2, 3, 0,

	4, 7, 6,
	6, 5, 4
};
void primitive_object::load_vertex_data(GLfloat* pvertex_data, GLuint vetexlen, GLushort* pele_buff, GLushort ele_cnt)
{
	_vertex_buf_len = vetexlen;
	_ele_buf_len = ele_cnt;
	#ifdef GLFW_INCLUDE_ES3
	glBindVertexArray(_vao);
	#endif
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*_vertex_buf_len, pvertex_data, GL_STATIC_DRAW);
	if (pele_buff)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*_ele_buf_len, pele_buff, GL_STATIC_DRAW);
	}	
	GLuint idx = 0;
	GLubyte stride = get_stride();
	int pointer = 0;
	for (auto& el : _ele_format)
	{
		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, el, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(pointer*sizeof(GLfloat)));//
		pointer += el;
		idx++;
	}
	#ifdef GLFW_INCLUDE_ES3
	glBindVertexArray(0);
	#endif
}


void init_internal_primitive_list()
{
	auto acube = g_primitive_list.find("cube");
	if (acube==g_primitive_list.end())
	{
		g_primitive_list["cube"] = make_shared<primitive_object>();
		g_primitive_list["cube"]->set_ele_format({ 3, 3, 2 });
		g_primitive_list["cube"]->load_vertex_data(_vertices, sizeof(_vertices) / sizeof(GLfloat), \
			_indices, sizeof(_indices) / sizeof(GLushort));
	}
	//g_primitive_list["cube"] = make_shared<primitive_cube>();
}
mp_primitive g_primitive_list;