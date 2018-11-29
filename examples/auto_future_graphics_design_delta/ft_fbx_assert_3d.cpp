#include "ft_fbx_assert_3d.h"
#include "primitive_object.h"
#include <chrono>

namespace auto_future
{
	ft_fbx_assert_3d::ft_fbx_assert_3d()
	{
		_fbx_assert_pt._scale.x = 1.f;
		_fbx_assert_pt._scale.y = 1.f;
		_fbx_assert_pt._scale.z = 1.f;
		_fbx_assert_pt._camera_center.x = 0.f;
		_fbx_assert_pt._camera_center.y = 0.f;
		_fbx_assert_pt._camera_center.z = 0.f;
		_fbx_assert_pt._camera_up.x = 0.f;
		_fbx_assert_pt._camera_up.y = 1.f;
		_fbx_assert_pt._camera_up.z = 0.f;

	}

	ft_fbx_assert_3d::~ft_fbx_assert_3d()
	{

	}

	void ft_fbx_assert_3d::draw()
	{
		ft_base::draw();
		if (!g_fbx_name_vector.size())
		{
			return;
		}
		init_internal_mesh_point();
	}

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	static bool get_fbx_item(void* data, int idx, const char** out_str)
	{
		*out_str = g_fbx_name_vector[idx].c_str();
		return true;
	}
	static bool get_mesh_material(void* data, int idx, const char** out_str)
	{
		auto it = g_material_list.begin();
		for (auto i = 0; i < g_material_list.size(); ++i, it++)
		{
			if (i == idx)
				*out_str = it->first.c_str();
		}
		
		return true;
	}

	static bool get_mesh_texture(void* data, int idx, const char** out_str)
	{
		auto it = g_mtexture_list.begin();
		for (auto i = 0; i < g_mtexture_list.size(); ++i, it++)
		{
			if (i == idx)
				*out_str = it->first.c_str();
		}

		return true;
	}

	void ft_fbx_assert_3d::draw_peroperty_page(int property_part)
	{
		ft_base::draw_peroperty_page();
		if (!g_fbx_name_vector.size())
		{
			return;
		}
		ImGui::Text("fbx name:");
		static int _selectIdx = -1;
		ImGui::Combo("fbx name", &_fbx_assert_pt._select_list, &get_fbx_item, &g_fbx_name_vector[_fbx_assert_pt._select_list], g_fbx_name_vector.size());
		
		if (_fbx_assert_pt._select_list != _selectIdx)
		{
			_selectIdx = _fbx_assert_pt._select_list;

			for (auto &it : fbx_info_map)
			{
				if (it.first == g_fbx_name_vector[_fbx_assert_pt._select_list])
				{
					ImGui::Text("mesh count:%d", it.second._mesh_count);
					break;
				}
			}
		}
		ImGui::Text("");
		for (auto &it : fbx_info_map)
		{
			if (it.first == g_fbx_name_vector[_fbx_assert_pt._select_list])
			{
				ImGui::Text("mesh count:%d", it.second._mesh_count);
				auto i = 0;
				for (auto ij = it.second._mesh_info.begin(); ij != it.second._mesh_info.end(); ij++, i++)
				{
					ImGui::Text("");
					ImGui::Text("mesh name:[%d]%s", i, it.second._mesh_info[i]._mesh_name.c_str());
					string str = "material list";
					str += to_string(i);
					
					if (!ij->_material_name.empty())
					{
						ImGui::Text("material name:%s", it.second._mesh_info[i]._material_name.c_str());
					
						if (string::npos != ij->_shader_mode.find("Textured"))
						{
							string texture_str = "texture list";
							texture_str += to_string(i);

							ImGui::Text("texture name:%s", it.second._mesh_info[i]._texture_name.c_str());
						}
					}
				}
				break;
			}
		}
		ImGui::Text("Model");
		ImGui::Text("Translation:");
		ImGui::SliderFloat("tx", &_fbx_assert_pt._translation.x, -100.f, 100.f);
		ImGui::SliderFloat("ty", &_fbx_assert_pt._translation.y, -100.f, 100.f);
		ImGui::SliderFloat("tz", &_fbx_assert_pt._translation.z, -100.f, 100.f);
		ImGui::Text("scale:");
		ImGui::SliderFloat("sx", &_fbx_assert_pt._scale.x, -10.f, 10.f);
		ImGui::SliderFloat("sy", &_fbx_assert_pt._scale.y, -10.f, 10.f);
		ImGui::SliderFloat("sz", &_fbx_assert_pt._scale.z, -10.f, 10.f);

		ImGui::Text("Rotation:");
		ImGui::SliderFloat("rx", &_fbx_assert_pt._rotation.x, -360.f, 360.f);
		ImGui::SliderFloat("ry", &_fbx_assert_pt._rotation.y, -360.f, 360.f);
		ImGui::SliderFloat("rz", &_fbx_assert_pt._rotation.z, -360.f, 360.f);

		ImGui::Text("");
		ImGui::Text("Camera");
		ImGui::Text("Eye:");
		ImGui::SliderFloat("Ex", &_fbx_assert_pt._camera_eye.x, -100.f, 100.f);
		ImGui::SliderFloat("Ey", &_fbx_assert_pt._camera_eye.y, -100.f, 100.f);
		ImGui::SliderFloat("Ez", &_fbx_assert_pt._camera_eye.z, -100.f, 100.f);

		ImGui::Text("Center:");
		ImGui::SliderFloat("Cx", &_fbx_assert_pt._camera_center.x, -10.f, 10.f);
		ImGui::SliderFloat("Cy", &_fbx_assert_pt._camera_center.y, -10.f, 10.f);
		ImGui::SliderFloat("Cz", &_fbx_assert_pt._camera_center.z, -10.f, 10.f);

		ImGui::Text("Up:");
		ImGui::SliderFloat("Ux", &_fbx_assert_pt._camera_up.x, -360.f, 360.f);
		ImGui::SliderFloat("Uy", &_fbx_assert_pt._camera_up.y, -360.f, 360.f);
		ImGui::SliderFloat("Uz", &_fbx_assert_pt._camera_up.z, -360.f, 360.f);

		ImGui::Text("");
		ImGui::Text("Projection");
		ImGui::SliderFloat("FOV", &_fbx_assert_pt._proj_fov, 0.f, 360.f);
		ImGui::SliderInt("width", &_fbx_assert_pt._proj_width, 0, 1920);
		ImGui::SliderInt("height", &_fbx_assert_pt._proj_height, 0, 720);
		ImGui::SliderFloat("linear", &_fbx_assert_pt._proj_linear, 0.f, 100.f);
		ImGui::SliderFloat("far", &_fbx_assert_pt._proj_far, 0.f, 100.f);
		
		ImGui::Text("");
		ImGui::Text("Light");
		ImGui::SliderFloat("LR", &_fbx_assert_pt._light_color.x, 0.f, 1.f);
		ImGui::SliderFloat("LG", &_fbx_assert_pt._light_color.y, 0.f, 1.f);
		ImGui::SliderFloat("LB", &_fbx_assert_pt._light_color.z, 0.f, 1.f);

		ImGui::SliderFloat("LPx", &_fbx_assert_pt._light_pos.x, 0.f, 100.f);
		ImGui::SliderFloat("LPy", &_fbx_assert_pt._light_pos.y, 0.f, 100.f);
		ImGui::SliderFloat("LPz", &_fbx_assert_pt._light_pos.z, 0.f, 100.f);
	}

	bool ft_fbx_assert_3d::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		_fbx_assert_pt._select_list = jvalue["fbx_index"].asInt();

		Value& trans = jvalue["trans"];
		_fbx_assert_pt._translation.x = trans["translationx"].asDouble();
		_fbx_assert_pt._translation.y = trans["translationy"].asDouble();
		_fbx_assert_pt._translation.z = trans["translationz"].asDouble();

		Value& scale = jvalue["scale"];
		_fbx_assert_pt._scale.x = scale["scalex"].asDouble();
		_fbx_assert_pt._scale.y = scale["scaley"].asDouble();
		_fbx_assert_pt._scale.z = scale["scalez"].asDouble();

		Value& ratation = jvalue["ratation"];
		_fbx_assert_pt._rotation.x = ratation["ratationx"].asDouble();
		_fbx_assert_pt._rotation.y = ratation["ratationy"].asDouble();
		_fbx_assert_pt._rotation.z = ratation["ratationz"].asDouble();

		Value& Eye = jvalue["CEye"];
		_fbx_assert_pt._camera_eye.x = Eye["CEyex"].asDouble();
		_fbx_assert_pt._camera_eye.y = Eye["CEyey"].asDouble();
		_fbx_assert_pt._camera_eye.z = Eye["CEyez"].asDouble();

		Value& Center = jvalue["CCenter"];
		_fbx_assert_pt._camera_center.x = Center["CCenterx"].asDouble();
		_fbx_assert_pt._camera_center.y = Center["CCentery"].asDouble();
		_fbx_assert_pt._camera_center.z = Center["CCenterz"].asDouble();

		Value& Up = jvalue["CUp"];
		_fbx_assert_pt._camera_up.x = Up["CUpx"].asDouble();
		_fbx_assert_pt._camera_up.y = Up["CUpy"].asDouble();
		_fbx_assert_pt._camera_up.z = Up["CUpz"].asDouble();

		Value& Proj = jvalue["proj"];
		_fbx_assert_pt._proj_fov = Proj["fov"].asDouble();
		_fbx_assert_pt._proj_width = Proj["width"].asInt();
		_fbx_assert_pt._proj_height = Proj["height"].asInt();
		_fbx_assert_pt._proj_linear = Proj["linear"].asDouble();
		_fbx_assert_pt._proj_far = Proj["far"].asDouble();

		Value& Light = jvalue["Light"];
		_fbx_assert_pt._light_color.x = Light["LCR"].asDouble();
		_fbx_assert_pt._light_color.y = Light["LCG"].asDouble();
		_fbx_assert_pt._light_color.z = Light["LCB"].asDouble();
		_fbx_assert_pt._light_pos.x = Light["LPX"].asDouble();
		_fbx_assert_pt._light_pos.y = Light["LPY"].asDouble();
		_fbx_assert_pt._light_pos.z = Light["LPZ"].asDouble();

		return true;
	}
	bool ft_fbx_assert_3d::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		junit["fbx_index"] = _fbx_assert_pt._select_list;
		Value trans(objectValue);
		trans["translationx"] = _fbx_assert_pt._translation.x;
		trans["translationy"] = _fbx_assert_pt._translation.y;
		trans["translationz"] = _fbx_assert_pt._translation.z;
		junit["trans"] = trans;

		Value scale(objectValue);
		scale["scalex"] = _fbx_assert_pt._scale.x;
		scale["scaley"] = _fbx_assert_pt._scale.y;
		scale["scalez"] = _fbx_assert_pt._scale.z;
		junit["scale"] = scale;

		Value ratation(objectValue);
		ratation["ratationx"] = _fbx_assert_pt._rotation.x;
		ratation["ratationy"] = _fbx_assert_pt._rotation.y;
		ratation["ratationz"] = _fbx_assert_pt._rotation.z;
		junit["ratation"] = ratation;

		Value Eye(objectValue);
		Eye["CEyex"] = _fbx_assert_pt._camera_eye.x;
		Eye["CEyey"] = _fbx_assert_pt._camera_eye.y;
		Eye["CEyez"] = _fbx_assert_pt._camera_eye.z;
		junit["CEye"] = Eye;

		Value Center(objectValue);
		Center["CCenterx"] = _fbx_assert_pt._camera_center.x;
		Center["CCentery"] = _fbx_assert_pt._camera_center.y;
		Center["CCenterz"] = _fbx_assert_pt._camera_center.z;
		junit["CCenter"] = Center;

		Value Up(objectValue);
		Up["CUpx"] = _fbx_assert_pt._camera_up.x;
		Up["CUpy"] = _fbx_assert_pt._camera_up.y;
		Up["CUpz"] = _fbx_assert_pt._camera_up.z;
		junit["CUp"] = Up;

		Value Proj(objectValue);
		Proj["fov"] = _fbx_assert_pt._proj_fov;
		Proj["width"] = _fbx_assert_pt._proj_width;
		Proj["height"] = _fbx_assert_pt._proj_height;
		Proj["linear"] = _fbx_assert_pt._proj_linear;
		Proj["far"] = _fbx_assert_pt._proj_far;
		junit["proj"] = Proj;

		Value Light(objectValue);
		Light["LCR"] = _fbx_assert_pt._light_color.x;
		Light["LCG"] = _fbx_assert_pt._light_color.y;
		Light["LCB"] = _fbx_assert_pt._light_color.z;
		Light["LPX"] = _fbx_assert_pt._light_pos.x;
		Light["LPY"] = _fbx_assert_pt._light_pos.y;
		Light["LPZ"] = _fbx_assert_pt._light_pos.z;
		junit["Light"] = Light;

		return true;
	}

	void ft_fbx_assert_3d::init_internal_mesh_point()
	{
		for (auto &it : fbx_info_map)
		{
			if (it.first == g_fbx_name_vector[_fbx_assert_pt._select_list])
			{
				auto i = 0;
				for (auto ij = it.second._mesh_info.begin(); ij != it.second._mesh_info.end(); ij++, i++)
				{
					auto& pmut = g_primitive_list.find(ij->_mesh_name);
					if (pmut == g_primitive_list.end()) return;
					_pcube_prim = pmut->second;

					glm::mat4 model = glm::mat4(1.f);
					model = glm::translate(model, glm::vec3(_fbx_assert_pt._translation.x, _fbx_assert_pt._translation.y, _fbx_assert_pt._translation.z));
					model = glm::scale(model, glm::vec3(_fbx_assert_pt._scale.x, _fbx_assert_pt._scale.y, _fbx_assert_pt._scale.z));
					model = glm::rotate(
						model,
						_fbx_assert_pt._rotation.x * glm::radians(1.0f),
						glm::vec3(1.0f, 0.0f, 0.0f)
						);

					model = glm::rotate(
						model,
						_fbx_assert_pt._rotation.y * glm::radians(1.0f),
						glm::vec3(0.0f, 1.0f, 0.0f)
						);

					model = glm::rotate(
						model,
						_fbx_assert_pt._rotation.z * glm::radians(1.0f),
						glm::vec3(0.0f, 0.0f, 1.0f)
						);

					glm::mat4 view = glm::mat4(1.f);
					view = glm::lookAt(glm::vec3(_fbx_assert_pt._camera_eye.x, _fbx_assert_pt._camera_eye.y, _fbx_assert_pt._camera_eye.z),
						glm::vec3(_fbx_assert_pt._camera_center.x, _fbx_assert_pt._camera_center.y, _fbx_assert_pt._camera_center.z),
						glm::vec3(_fbx_assert_pt._camera_up.x, _fbx_assert_pt._camera_up.y, _fbx_assert_pt._camera_up.z));

					glm::mat4 proj = glm::perspective(glm::radians(_fbx_assert_pt._proj_fov), (float)_fbx_assert_pt._proj_width / (float)_fbx_assert_pt._proj_height, _fbx_assert_pt._proj_linear, _fbx_assert_pt._proj_far);

					glm::vec4 eye = glm::vec4(_fbx_assert_pt._camera_eye.x, _fbx_assert_pt._camera_eye.y, _fbx_assert_pt._camera_eye.z, 0.f);
					glm::vec4 lightColor = glm::vec4(_fbx_assert_pt._light_color.x, _fbx_assert_pt._light_color.y, _fbx_assert_pt._light_color.z, 1.f);
					glm::vec4 lightPos = glm::vec4(_fbx_assert_pt._light_pos.x, _fbx_assert_pt._light_pos.y, _fbx_assert_pt._light_pos.z, 0.f);

					string _str = "";
					auto it = g_material_list.begin();
					for (auto j = 0; j < g_material_list.size(); j++, it++)
					{
						if (i == j)
						{
							_str = it->first;
							break;
						}
					}

					auto& mut = g_material_list.find(_str);
					if (mut != g_material_list.end())
					{
						_pmaterial = mut->second;
					}
					


					_pmaterial->set_value("model", glm::value_ptr(model), 16);
					_pmaterial->set_value("view", glm::value_ptr(view), 16);
					_pmaterial->set_value("EyePosW", glm::value_ptr(eye), 4);
					_pmaterial->set_value("proj", glm::value_ptr(proj), 16);
					_pmaterial->set_value("LightColor", glm::value_ptr(lightColor), 16);
					_pmaterial->set_value("LightPosW", glm::value_ptr(lightPos), 16);

					_pmaterial->use();
					glBindVertexArray(_pcube_prim->_vao);

					if (string::npos != ij->_shader_mode.find("Textured"))
					{
						auto& mtx = g_mtexture_list.find(ij->_texture_name);
						if (mtx != g_mtexture_list.end())
						{
							_texture = mtx->second;
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, _texture->_txt_id);
						}
					}
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _pcube_prim->_ebo);
					glDrawArrays(GL_TRIANGLES, 0, _pcube_prim->_vertex_buf_len/8);
				}
			}
		}
	}
#endif
}