#include "ft_cube.h"
//#include "color_shader.h"
/*
      3              2


0             1


      7              6


4              5
*/
const float half_side_len = 0.25f;
GLfloat g_vertices[] = {
	-half_side_len, -half_side_len, -half_side_len, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	half_side_len, -half_side_len, -half_side_len, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	half_side_len, -half_side_len, half_side_len, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	-half_side_len, -half_side_len, half_side_len, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	-half_side_len, half_side_len, -half_side_len, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	half_side_len, half_side_len, -half_side_len, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	half_side_len, half_side_len, half_side_len, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-half_side_len, half_side_len, half_side_len, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
};

GLushort g_indices[] = {
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
GLuint ft_cube::_vao = 0;
GLuint ft_cube::_vbo = 0;
GLuint ft_cube::_ebo = 0;

unsigned short ft_cube::_cube_instance_cnt = 0;

ft_cube::ft_cube()
	:ft_base()
	, _pshader(g_shader_list["color_shader"][0].get())
	
{
	if (_cube_instance_cnt==0)
	{
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
		glGenBuffers(1, &_ebo);

		vector<string> attr_name_list = { "position", "qcolor", "texcoord" };
		_pshader->loading_shader_attributes_from_avbo(_vao, _vbo, g_vertices, sizeof(g_vertices), \
			attr_name_list, _ebo, g_indices, sizeof(g_indices));
	}
	_cube_instance_cnt++;
	
	//_pshader = new color_shader();

}


ft_cube::~ft_cube()
{
	//delete _pshader;
	_cube_instance_cnt--;
	if (_cube_instance_cnt==0)
	{
		glDeleteVertexArrays(1,&_vao);
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ebo);
	}
}
#include <chrono>
void ft_cube::draw()
{
	_pshader->use();
	glBindVertexArray(_vao);
	//glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	//munf_list& mfunlt = _pshader->get_unf_list();
	/*
	glm::mat4 model;
	model = glm::scale(model, _pt._scale);
	model = glm::rotate(model, _pt._rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, _pt._rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, _pt._rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, _pt._translation);
	_pshader->set_uniform(string("model"), 1, glm::value_ptr(model));*/
	static auto t_start = std::chrono::high_resolution_clock::now();
	auto t_now = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(
		model,
		time* glm::radians(30.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
		);
	_pshader->set_uniform(string("model"), 1, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (const void*)0);
}
#if !defined(IMGUI_WAYLAND)
void ft_cube::draw_peroperty_page()
{
}

bool ft_cube::init_from_json(Value& jvalue)
{
	return true;
}

bool ft_cube::init_json_unit(Value& junit)
{
	return true;
}
#endif