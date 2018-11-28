#include "texture_shader.h"
#include "control_common_def.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "SOIL.h"
#include "texture.h"
#include "camera.h"
// Shader sources
static const GLchar* ts_vertexSource = R"glsl(
    attribute vec3 position;
    attribute vec2 textcoord;
    attribute vec3 normal;

    varying vec2 Textcoord;
	uniform mat4 proj;
	uniform mat4 view;
	uniform mat4 model;
	void main()
	{
		 gl_Position = proj * view * model * vec4(position, 1.0);
		Textcoord = textcoord;
	}
)glsl";
static const GLchar* ts_fragmentSource = R"glsl(
	varying vec2 Textcoord;
	uniform sampler2D text;
	void main()
	{
		gl_FragColor = texture(text, Textcoord);
	}
)glsl";


texture_shader::texture_shader()
{
		//vertex shader
	_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_vertex_shader, 1, &ts_vertexSource, NULL);
	glCompileShader(_vertex_shader);
#define CHECK_SHADER_STATUS
#ifdef CHECK_SHADER_STATUS
	GLint status;
	glGetShaderiv(_vertex_shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		glGetShaderInfoLog(_vertex_shader, 512, NULL, buffer);
		printf("shader error:%s\n", buffer);

	}
#endif
	//fragment shader
	_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_fragment_shader, 1, &ts_fragmentSource, NULL);
	glCompileShader(_fragment_shader);
#ifdef CHECK_SHADER_STATUS
	glGetShaderiv(_vertex_shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		glGetShaderInfoLog(_vertex_shader, 512, NULL, buffer);
		printf("shader2 error:%s\n", buffer);
	}
#endif
	//link
	_shader_program_id = glCreateProgram();
	glAttachShader(_shader_program_id, _vertex_shader);
	glAttachShader(_shader_program_id, _fragment_shader);
	glBindFragDataLocation(_shader_program_id, 0, "outColor");
	glLinkProgram(_shader_program_id);
	glUseProgram(_shader_program_id);
	
	GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 16; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length

	glGetProgramiv(_shader_program_id, GL_ACTIVE_ATTRIBUTES, &count);
	printf("Active Attributes: %d\n", count);

	for (i = 0; i < count; i++)
	{
		glGetActiveAttrib(_shader_program_id, (GLuint)i, bufSize, &length, &size, &type, name);

		printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
	}

	glGetProgramiv(_shader_program_id, GL_ACTIVE_UNIFORMS, &count);
	printf("Active Uniforms: %d\n", count);

	for (i = 0; i < count; i++)
	{
		glGetActiveUniform(_shader_program_id, (GLuint)i, bufSize, &length, &size, &type, name);

		printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
	}

	_mattr_list["position"] = shader_attribute(en_attr_vec3, glGetAttribLocation(_shader_program_id, "position"));
	_mattr_list["textcoord"] = shader_attribute(en_attr_vec2, glGetAttribLocation(_shader_program_id, "textcoord"));
	_mattr_list["normal"] = shader_attribute(en_attr_vec3, glGetAttribLocation(_shader_program_id, "normal"));
	GLuint _model = glGetUniformLocation(_shader_program_id, "model");
	glm::mat4 model;
	model= glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	glUniformMatrix4fv(_model, 1, GL_FALSE, glm::value_ptr(model));
	_munf_list["model"] = shader_uniform(en_unf_mat4, _model);
	//GL_FLOAT
	GLuint _view = glGetUniformLocation(_shader_program_id, "view");
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
		);
	glUniformMatrix4fv(_view, 1, GL_FALSE, glm::value_ptr(view));
	_munf_list["view"] = shader_uniform(en_unf_mat4, _view);

	GLuint _proj = glGetUniformLocation(_shader_program_id, "proj");
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), 800.0f / 600.0f, 1.0f, 100.0f);

	glUniformMatrix4fv(_proj, 1, GL_FALSE, glm::value_ptr(proj));
	_munf_list["proj"] = shader_uniform(en_unf_mat4, _proj);
	_munf_list["text"] = shader_uniform(en_unf_tex, glGetUniformLocation(_shader_program_id, "text"));
}


