#include "ft_model_3d.h"
#include "SOIL.h"
#include "texture.h"
#include <chrono>
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "dir_output.h"
extern string g_cureent_directory;
#endif
namespace auto_future
{
	ft_model_3d::ft_model_3d()
		:ft_base(), _tri_cnt(0)
	{
		reg_property_handle(&_pt, 3, [this](void*){
			ImGui::InputText("-mesh data file", _pt._mesh_data_file, FILE_NAME_LEN);
			if (ImGui::Button("loading mesh data"))
			{

				load_mesh_data_2_vertices();
			}
		});

		const auto& mut = g_material_list.find("atexture");
		_pmaterial = mut->second;
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		_pmaterial->set_value("model", glm::value_ptr(model), 16);
		glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 2.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
			);
        _pmaterial->set_value("view", glm::value_ptr(view), 16);
		glm::mat4 proj = glm::perspective(glm::radians(60.0f), 800.0f / 600.0f, 1.0f, 100.0f);
		_pmaterial->set_value("proj", glm::value_ptr(proj), 16);

		const auto& mtx = g_mtexture_list.find("VW_Golf_VII_2013.png");
		_texture = mtx->second;
			//vector<string> attr_name_list = { "position", "qcolor", "texcoord" };
			//_pshader->loading_shader_attributes_from_avbo(_vao, _vbo, g_vertices, sizeof(g_vertices), \
						attr_name_list, _ebo, g_indices, sizeof(g_indices));

	}


	ft_model_3d::~ft_model_3d()
	{

			glDeleteVertexArrays(1, &_vao);
			glDeleteBuffers(1, &_vbo);
	}


	void ft_model_3d::draw()
	{
		if (_tri_cnt)
		{
			glm::mat4 model = glm::mat4(1.f);
			model = glm::translate(model, glm::vec3(_pt._translation_shd.x, _pt._translation_shd.y, _pt._translation_shd.z));
			model = glm::scale(model, glm::vec3(_pt._scale_stn.x, _pt._scale_stn.y, _pt._scale_stn.z));
			model = glm::rotate(
				model,
				_pt._rotation_srd.x * glm::radians(1.0f),
				glm::vec3(1.0f, 0.0f, 0.0f)
				);

			model = glm::rotate(
				model,
				_pt._rotation_srd.y * glm::radians(1.0f),
				glm::vec3(0.0f, 1.0f, 0.0f)
				);

			model = glm::rotate(
				model,
				_pt._rotation_srd.z * glm::radians(1.0f),
				glm::vec3(0.0f, 0.0f, 1.0f)
				);

			_pmaterial->set_value("model", glm::value_ptr(model), 16);
			_pmaterial->use();
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
			glBindTexture(GL_TEXTURE_2D, _texture->_txt_id);


			glDrawArrays(GL_TRIANGLES, 0, _tri_cnt);
		}

	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

	void ft_model_3d::load_mesh_data_2_vertices()
	{
		string str_texture_file = g_cureent_directory + mesh_fold;
		string str_mesh_file = str_texture_file;	
		str_mesh_file += _pt._mesh_data_file;
		vector<Vertex> _vertData;
		loadFBXFile(str_mesh_file.c_str(), _vertData);
		_tri_cnt=_vertData.size();
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER,_vbo);
		glBufferData(GL_ARRAY_BUFFER,_vertData.size()*sizeof(GLfloat)*8,&_vertData[0],GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(5*sizeof(GLfloat)));
	}

	
	bool ft_model_3d::init_from_json(Value& jvalue)
	{
		return true;
	}
	bool ft_model_3d::init_json_unit(Value& junit)
	{
		return true;
	}
#endif
}