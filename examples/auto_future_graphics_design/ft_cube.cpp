#include "ft_cube.h"

ft_cube::ft_cube()
	:ft_base()	
{
	auto& mut = g_material_list.find("color1");
	_pmaterial = mut->second;
	auto& pmut=g_primitive_list.find("cube");
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
	_pmaterial->set_value("model",glm::value_ptr(model),16);

}


ft_cube::~ft_cube()
{
	//delete _pshader;
	
}
#include <chrono>
void ft_cube::draw()
{
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(_pt._translation.x, _pt._translation.y, _pt._translation.z));
	model = glm::scale(model, glm::vec3(_pt._scale.x, _pt._scale.y, _pt._scale.z));
	model = glm::rotate(
		model,
		_pt._rotation.x * glm::radians(1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)
		);

	model = glm::rotate(
		model,
		_pt._rotation.y * glm::radians(1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
		);

	model = glm::rotate(
		model,
		_pt._rotation.z * glm::radians(1.0f),
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
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void ft_cube::draw_peroperty_page()
{
	ImGui::Text("Translation:");
	ImGui::SliderFloat("tx", &_pt._translation.x, -100.f, 100.f);
	ImGui::SliderFloat("ty", &_pt._translation.y, -100.f, 100.f);
	ImGui::SliderFloat("tz", &_pt._translation.z, -100.f, 100.f);
	ImGui::Text("scale:");
	ImGui::SliderFloat("sx", &_pt._scale.x, -10.f, 10.f);
	ImGui::SliderFloat("sy", &_pt._scale.y, -10.f, 10.f);
	ImGui::SliderFloat("sz", &_pt._scale.z, -10.f, 10.f);

	ImGui::Text("Rotation:");
	ImGui::SliderFloat("rx", &_pt._rotation.x, -360.f, 360.f);
	ImGui::SliderFloat("ry", &_pt._rotation.y, -360.f, 360.f);
	ImGui::SliderFloat("rz", &_pt._rotation.z, -360.f, 360.f);

}

bool ft_cube::init_from_json(Value& jvalue)
{
	ft_base::init_from_json(jvalue);
	Value& translation = jvalue["translation"];
	_pt._translation.x = translation["x"].asDouble();
	_pt._translation.y = translation["y"].asDouble();
	_pt._translation.z = translation["z"].asDouble();
	Value& scale = jvalue["scale"];
	_pt._scale.x = scale["x"].asDouble();
	_pt._scale.y = scale["y"].asDouble();
	_pt._scale.z = scale["z"].asDouble();

	return true;
}

bool ft_cube::init_json_unit(Value& junit)
{
	ft_base::init_json_unit(junit);
	Value translation(objectValue);
	translation["x"] = _pt._translation.x;
	translation["y"] = _pt._translation.y;
	translation["z"] = _pt._translation.z;
	junit["translation"] = translation;
	Value scale(objectValue);
	scale["x"] = _pt._scale.x;
	scale["y"] = _pt._scale.y;
	scale["z"] = _pt._scale.z;
	junit["scale"] = scale;

	return true;
}
#endif