#include "ft_cube_3d.h"
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace zl_future
{
	ft_cube_3d::ft_cube_3d()
		:ft_base()
	{
		const auto& mut = g_material_list.find("mtl_color");
		_pmaterial = mut->second;
		const auto& pmut = g_primitive_list.find("cube");
		_pcube_prim = pmut->second;
		glm::mat4 view = glm::lookAt(
			glm::vec3(-1.5f, 0.0f, -1.5f),//e
			glm::vec3(0.0f, 0.0f, 0.0f),//center
			glm::vec3(0.0f, 1.0f, 0.0f)//up
			);
		_pmaterial->set_value("view", glm::value_ptr(view), 16);
		glm::mat4 proj = glm::perspective(glm::radians(60.0f), 800.0f / 600.0f, 1.0f, 10.0f);
		_pmaterial->set_value("proj", glm::value_ptr(proj), 16);
		glm::mat4 model = glm::mat4(1.0f);
		/*model = glm::rotate(
			model,
			time* glm::radians(30.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
			);*/
		_pmaterial->set_value("model", glm::value_ptr(model), 16);

	}


	ft_cube_3d::~ft_cube_3d()
	{
		//delete _pshader;

	}

	void ft_cube_3d::draw()
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
		//_pshader->use();

		//static auto t_start = std::chrono::high_resolution_clock::now();
		//auto t_now = std::chrono::high_resolution_clock::now();
		//float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
		//glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(
		//	model,
		//	time* glm::radians(30.0f),
		//	glm::vec3(0.0f, 1.0f, 0.0f)
		//	);
		////_pshader->set_uniform(string("model"), 1, glm::value_ptr(model));
		//_pmaterial->set_value("model", glm::value_ptr(model), 16);
		_pmaterial->use();
		glBindVertexArray(_pcube_prim->_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _pcube_prim->_ebo);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (const void*)0);
	}
#if !defined(DISABLE_DEMO)

#endif
}