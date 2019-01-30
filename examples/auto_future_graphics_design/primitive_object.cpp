#include "primitive_object.h"
const float half_side_len = 0.5f;

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

GLuint _indices[] = {
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
GLfloat _plain_vertices[] = {
	-half_side_len, -half_side_len, 0.f, 0.0f, 0.0f,
	half_side_len, -half_side_len, 0.f, 1.0f, 0.0f,
	-half_side_len, half_side_len, 0.f, 1.0f, 1.0f,
	half_side_len, half_side_len, 0.f,  0.0f, 1.0f,
};
GLfloat _direction[] =
{
	0, 0, 0,
	1.f, 0, 0,
};
GLfloat _arrow[]=
{
	0.75f,-0.25f,0.f,
	  1.f,   0.f,0.f,
	0.75f,0.25f,0.f,
};
void primitive_object::load_vertex_data(GLfloat* pvertex_data, GLuint vetexlen, GLuint* pele_buff, GLuint ele_cnt)
{
	_vertex_buf_len = vetexlen;
	_ele_buf_len = ele_cnt;
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*_vertex_buf_len, pvertex_data, GL_STATIC_DRAW);
	if (pele_buff)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*_ele_buf_len, pele_buff, GL_STATIC_DRAW);
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
	glBindVertexArray(0);
}


void init_internal_primitive_list()
{
	auto acube = g_primitive_list.find("cube");
	if (acube==g_primitive_list.end())
	{
		g_primitive_list["cube"] = make_shared<primitive_object>();
		g_primitive_list["cube"]->set_ele_format({ 3, 3, 2 });
		g_primitive_list["cube"]->load_vertex_data(_vertices, sizeof(_vertices) / sizeof(GLfloat), \
			_indices, sizeof(_indices) / sizeof(GLuint));
	}
	auto aplain = g_primitive_list.find("plain");
	if (aplain==g_primitive_list.end())
	{
		g_primitive_list["plain"] = make_shared<primitive_object>();
		g_primitive_list["plain"]->set_ele_format({ 3, 2 });
		g_primitive_list["plain"]->load_vertex_data(_plain_vertices, sizeof(_plain_vertices) / sizeof(GLfloat));
	}
	//g_primitive_list["cube"] = make_shared<primitive_cube>();
}
mp_primitive g_primitive_list;