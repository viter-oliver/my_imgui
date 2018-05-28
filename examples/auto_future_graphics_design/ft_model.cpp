#include "ft_model.h"
#include "SOIL.h"
#include "texture.h"
GLuint ft_model::_vao = 0;
GLuint ft_model::_vbo = 0;

unsigned short ft_model::_model_instance_cnt = 0;

ft_model::ft_model()
	:ft_base()
	, _pshader(g_shader_list["texture_shader"][0].get())
{
	if (_model_instance_cnt == 0)
	{
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);

		//vector<string> attr_name_list = { "position", "qcolor", "texcoord" };
		//_pshader->loading_shader_attributes_from_avbo(_vao, _vbo, g_vertices, sizeof(g_vertices), \
			attr_name_list, _ebo, g_indices, sizeof(g_indices));
	}
	_model_instance_cnt++;
}


ft_model::~ft_model()
{
	_model_instance_cnt--;
	if (_model_instance_cnt == 0)
	{
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);
	}
}
extern string g_cureent_project_file_path;
const char* mesh_fold = "mesh_res\\";
void ft_model::load_mesh_data_2_vertices()
{
	string str_texture_file = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
	str_texture_file += mesh_fold;
	string str_mesh_file = str_texture_file;
	str_texture_file += _pt._texture_file;
	_pt._textureId = TextureHelper::load2DTexture(str_texture_file.c_str(), \
		_pt._txt_width, _pt._txt_height, GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA);
	str_mesh_file += _pt._mesh_data_file;

	loadFBXFile(str_mesh_file.c_str(), _vertData);

}

void ft_model::load_vertics_2_vbo()
{

	//glBindVertexArray(_vao);
	vector<string> attr_name_list = { "position", "textcoord","normal" };
	_pshader->loading_shader_attributes_from_avbo(_vao, _vbo, &_vertData[0], sizeof(Vertex)* _vertData.size(), attr_name_list);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _pt._textureId);
	_pshader->set_uniform_text(string("text"), 0);
}
#include <chrono>
void ft_model::draw()
{
	if (_vertData.size() > 0)
	{
		_pshader->use();
		glBindVertexArray(_vao);
		/*static auto t_start = std::chrono::high_resolution_clock::now();
		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(
			model,
			time* glm::radians(30.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
			);
		_pshader->set_uniform(string("model"), 1, glm::value_ptr(model));*/
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,_pt._textureId);
	

		glDrawArrays(GL_TRIANGLES, 0, _vertData.size());
	}

}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void ft_model::draw_peroperty_page()
{
	ImGui::InputText("-texture file", _pt._texture_file, FILE_NAME_LEN);
	ImGui::InputText("-mesh data file", _pt._mesh_data_file, FILE_NAME_LEN);
	if (ImGui::Button("loading mesh data"))
	{
		if (_vertData.size()>0)
		{
			_vertData.clear();
		}
		load_mesh_data_2_vertices();
		load_vertics_2_vbo();
	}

}
bool ft_model::init_from_json(Value& jvalue)
{
	return true;
}
bool ft_model::init_json_unit(Value& junit)
{
	return true;
}
#endif