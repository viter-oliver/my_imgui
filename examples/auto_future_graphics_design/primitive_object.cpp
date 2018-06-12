#include "primitive_object.h"
const float half_side_len = 0.5f;

GLfloat primitive_cube::_vertices[]= {
	-half_side_len, -half_side_len, -half_side_len, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	half_side_len, -half_side_len, -half_side_len, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	half_side_len, -half_side_len, half_side_len, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	-half_side_len, -half_side_len, half_side_len, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	-half_side_len, half_side_len, -half_side_len, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	half_side_len, half_side_len, -half_side_len, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	half_side_len, half_side_len, half_side_len, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-half_side_len, half_side_len, half_side_len, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	};

GLushort primitive_cube::_indices[] = {
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
primitive_cube::primitive_cube()
	:primitive_object()
{
	
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(0));//position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));//color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat),(void*)(6*sizeof(GLfloat)));//texture
	glBindVertexArray(0);
}


void init_primitive_list()
{
	g_primitive_list["cube"] = make_shared<primitive_cube>();
}
mp_primitive g_primitive_list;