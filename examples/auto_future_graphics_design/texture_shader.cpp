#include "texture_shader.h"
#include "control_common_def.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "SOIL.h"
#include "texture.h"
#include "camera.h"
// Shader sources
static const GLchar* vertexSource = R"glsl(
    #version 330 core
	layout(location = 0) in vec3 position;
	layout(location = 1) in vec2 textCoord;
	uniform mat4 projection;
	uniform mat4 view;
	uniform mat4 model;
	out vec2 TextCoord;
	void main()
	{
		 gl_Position = projection * view * model * vec4(position, 1.0);
         //gl_Position = vec4(position, 1.0);
		TextCoord = textCoord;
	}
)glsl";
static const GLchar* fragmentSource = R"glsl(
    #version 330 core
	in vec2 TextCoord;
	uniform sampler2D text;
	out vec4 color;
	void main()
	{
		color = texture(text, TextCoord);
	}
)glsl";

GLfloat cubeVertices[] = {
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// A
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f,	// B
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f,	// C
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f,	// C
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,	// D
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// A


	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,	// E
	-0.5f, 0.5f, -0.5f, 0.0, 1.0f,   // H
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	// G
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	// G
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f,	// F
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,	// E

	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,	// D
	-0.5f, 0.5f, -0.5f, 1.0, 1.0f,   // H
	-0.5f, -0.5f, -0.5f, 1.0f, 0.0f,	// E
	-0.5f, -0.5f, -0.5f, 1.0f, 0.0f,	// E
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// A
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,	// D

	0.5f, -0.5f, -0.5f, 1.0f, 0.0f,	// F
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	// G
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f,	// C
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f,	// C
	0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// B
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f,	// F

	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	// G
	-0.5f, 0.5f, -0.5f, 0.0, 1.0f,   // H
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,	// D
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,	// D
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,	// C
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	// G

	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// A
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,// E
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f,	// F
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f,	// F
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f,	// B
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// A
};
Camera camera(glm::vec3(0.f, 0.f, 6.f));
texture_shader::texture_shader(tri_mesh& tgmesh)
	:basic_shader(tgmesh)
{
	int txtwidth, txtheight;
	cubeTextId = TextureHelper::load2DTexture("../../res/container.jpg", txtwidth,txtheight);
	//vertex shader
	_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_vertex_shader, 1, &vertexSource, NULL);
	glCompileShader(_vertex_shader);
	//fragment shader
	_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_fragment_shader, 1, &fragmentSource, NULL);
	glCompileShader(_fragment_shader);
	//link
	_shader_program = glCreateProgram();
	glAttachShader(_shader_program, _vertex_shader);
	glAttachShader(_shader_program, _fragment_shader);
	glBindFragDataLocation(_shader_program, 0, "outColor");
	glLinkProgram(_shader_program);
	glUseProgram(_shader_program);
	glBindVertexArray(tgmesh._vao);
	glBindBuffer(GL_ARRAY_BUFFER, tgmesh._vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	// 顶点位置数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 顶点纹理数据
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	_model = glGetUniformLocation(_shader_program, "model");
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, glm::vec3(1.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(_model, 1, GL_FALSE, glm::value_ptr(model));
	_view = glGetUniformLocation(_shader_program, "view");
	/*glm::mat4 view = glm::lookAt(
		glm::vec3(-1.5f, -1.0f, -1.5f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)
		);*/
	glm::mat4 view = camera.getViewMatrix();
	glUniformMatrix4fv(_view, 1, GL_FALSE, glm::value_ptr(view));
	_proj = glGetUniformLocation(_shader_program, "projection");
	glm::mat4 proj = glm::perspective(camera.mouse_zoom,800.f/ 600.f, 1.0f, 10.f);//glm::perspective(glm::radians(60.0f), base_ui_component::screenw / base_ui_component::screenh, 1.0f, 10.0f);
	glUniformMatrix4fv(_proj, 1, GL_FALSE, glm::value_ptr(proj));
	glUniform1i(glGetUniformLocation(_shader_program, "text"), 0);
}


texture_shader::~texture_shader()
{
}

void texture_shader::load_tri_mesh()
{
	
}

void texture_shader::render_tri_mesh()
{
	GLint last_program, last_array_buffer, last_element_array_buffer, last_vertex_array;
	glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	//glEnable(GL_DEPTH_TEST);
	glUseProgram(_shader_program);
	glBindVertexArray(_tmesh._vao);
	//glBindVertexArray(cubeVAOId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTextId);
	
	// 绘制第一个立方体

	glDrawArrays(GL_TRIANGLES, 0, 36);
	

	glUseProgram(last_program);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
}

void texture_shader::set_vertex_wrold_matrix(glm::mat4& vtmat)
{
	glUniformMatrix4fv(_model, 1, GL_FALSE, glm::value_ptr(vtmat));
}
