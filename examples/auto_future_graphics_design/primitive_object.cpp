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
void primitive_object::load_vertex_data()
{
	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*_vertex_buf_len, _pvertex_buf, GL_STATIC_DRAW);

	if (_pele_buf)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*_ele_buf_len, _pele_buf, GL_STATIC_DRAW);
	}

	glBindVertexArray(0);

}


void init_internal_primitive_list()
{
	g_primitive_list["cube"] = make_shared<primitive_object>();
	g_primitive_list["cube"]->set_ele_format({ 3, 3, 2 });
	g_primitive_list["cube"]->prepare_vertex_data(_vertices, sizeof(_vertices) / sizeof(GLfloat), \
		_indices, sizeof(_indices) / sizeof(GLushort));
	g_primitive_list["cube"]->load_vertex_data();
	//g_primitive_list["cube"] = make_shared<primitive_cube>();
}
mp_primitive g_primitive_list;