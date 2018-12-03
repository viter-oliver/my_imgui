#include "ft_fbx_assert_3d.h"
#include "primitive_object.h"
#include <chrono>

namespace auto_future
{
	static bool get_fbx_item(void* data, int idx, const char** out_str)
	{
		*out_str = g_fbx_name_vector[idx].c_str();
		return true;
	}
	ft_fbx_assert_3d::ft_fbx_assert_3d()
	{
		_fbx_assert_pt._scale_stn = { 1.f, 1.f, 1.f };
		_fbx_assert_pt._camera_center_stn = { 0.f, 0.f, 0.f };
		_fbx_assert_pt._camera_up_srd = { 0.f, 1.f, 0.f };

		reg_property_handle(&_fbx_assert_pt, 0, [this](void* memb_address){
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
		});
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


	bool ft_fbx_assert_3d::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		_fbx_assert_pt._select_list = jvalue["fbx_index"].asInt();

		Value& trans = jvalue["trans"];
		_fbx_assert_pt._translation_shd.x = trans["translationx"].asDouble();
		_fbx_assert_pt._translation_shd.y = trans["translationy"].asDouble();
		_fbx_assert_pt._translation_shd.z = trans["translationz"].asDouble();

		Value& scale = jvalue["scale"];
		_fbx_assert_pt._scale_stn.x = scale["scalex"].asDouble();
		_fbx_assert_pt._scale_stn.y = scale["scaley"].asDouble();
		_fbx_assert_pt._scale_stn.z = scale["scalez"].asDouble();

		Value& ratation = jvalue["ratation"];
		_fbx_assert_pt._rotation_srd.x = ratation["ratationx"].asDouble();
		_fbx_assert_pt._rotation_srd.y = ratation["ratationy"].asDouble();
		_fbx_assert_pt._rotation_srd.z = ratation["ratationz"].asDouble();

		Value& Eye = jvalue["CEye"];
		_fbx_assert_pt._camera_eye_shd.x = Eye["CEyex"].asDouble();
		_fbx_assert_pt._camera_eye_shd.y = Eye["CEyey"].asDouble();
		_fbx_assert_pt._camera_eye_shd.z = Eye["CEyez"].asDouble();

		Value& Center = jvalue["CCenter"];
		_fbx_assert_pt._camera_center_stn.x = Center["CCenterx"].asDouble();
		_fbx_assert_pt._camera_center_stn.y = Center["CCentery"].asDouble();
		_fbx_assert_pt._camera_center_stn.z = Center["CCenterz"].asDouble();

		Value& Up = jvalue["CUp"];
		_fbx_assert_pt._camera_up_srd.x = Up["CUpx"].asDouble();
		_fbx_assert_pt._camera_up_srd.y = Up["CUpy"].asDouble();
		_fbx_assert_pt._camera_up_srd.z = Up["CUpz"].asDouble();

		Value& Proj = jvalue["proj"];
		_fbx_assert_pt._proj_fov_urd = Proj["fov"].asDouble();
		_fbx_assert_pt._proj_width = Proj["width"].asInt();
		_fbx_assert_pt._proj_height = Proj["height"].asInt();
		_fbx_assert_pt._proj_linear_uhd = Proj["linear"].asDouble();
		_fbx_assert_pt._proj_far_uhd = Proj["far"].asDouble();

		Value& Light = jvalue["Light"];
		_fbx_assert_pt._light_clr.x = Light["LCR"].asDouble();
		_fbx_assert_pt._light_clr.y = Light["LCG"].asDouble();
		_fbx_assert_pt._light_clr.z = Light["LCB"].asDouble();
		_fbx_assert_pt._light_pos_uhd.x = Light["LPX"].asDouble();
		_fbx_assert_pt._light_pos_uhd.y = Light["LPY"].asDouble();
		_fbx_assert_pt._light_pos_uhd.z = Light["LPZ"].asDouble();

		return true;
	}
	bool ft_fbx_assert_3d::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		junit["fbx_index"] = _fbx_assert_pt._select_list;
		Value trans(objectValue);
		trans["translationx"] = _fbx_assert_pt._translation_shd.x;
		trans["translationy"] = _fbx_assert_pt._translation_shd.y;
		trans["translationz"] = _fbx_assert_pt._translation_shd.z;
		junit["trans"] = trans;

		Value scale(objectValue);
		scale["scalex"] = _fbx_assert_pt._scale_stn.x;
		scale["scaley"] = _fbx_assert_pt._scale_stn.y;
		scale["scalez"] = _fbx_assert_pt._scale_stn.z;
		junit["scale"] = scale;

		Value ratation(objectValue);
		ratation["ratationx"] = _fbx_assert_pt._rotation_srd.x;
		ratation["ratationy"] = _fbx_assert_pt._rotation_srd.y;
		ratation["ratationz"] = _fbx_assert_pt._rotation_srd.z;
		junit["ratation"] = ratation;

		Value Eye(objectValue);
		Eye["CEyex"] = _fbx_assert_pt._camera_eye_shd.x;
		Eye["CEyey"] = _fbx_assert_pt._camera_eye_shd.y;
		Eye["CEyez"] = _fbx_assert_pt._camera_eye_shd.z;
		junit["CEye"] = Eye;

		Value Center(objectValue);
		Center["CCenterx"] = _fbx_assert_pt._camera_center_stn.x;
		Center["CCentery"] = _fbx_assert_pt._camera_center_stn.y;
		Center["CCenterz"] = _fbx_assert_pt._camera_center_stn.z;
		junit["CCenter"] = Center;

		Value Up(objectValue);
		Up["CUpx"] = _fbx_assert_pt._camera_up_srd.x;
		Up["CUpy"] = _fbx_assert_pt._camera_up_srd.y;
		Up["CUpz"] = _fbx_assert_pt._camera_up_srd.z;
		junit["CUp"] = Up;

		Value Proj(objectValue);
		Proj["fov"] = _fbx_assert_pt._proj_fov_urd;
		Proj["width"] = _fbx_assert_pt._proj_width;
		Proj["height"] = _fbx_assert_pt._proj_height;
		Proj["linear"] = _fbx_assert_pt._proj_linear_uhd;
		Proj["far"] = _fbx_assert_pt._proj_far_uhd;
		junit["proj"] = Proj;

		Value Light(objectValue);
		Light["LCR"] = _fbx_assert_pt._light_clr.x;
		Light["LCG"] = _fbx_assert_pt._light_clr.y;
		Light["LCB"] = _fbx_assert_pt._light_clr.z;
		Light["LPX"] = _fbx_assert_pt._light_pos_uhd.x;
		Light["LPY"] = _fbx_assert_pt._light_pos_uhd.y;
		Light["LPZ"] = _fbx_assert_pt._light_pos_uhd.z;
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
					model = glm::translate(model, glm::vec3(_fbx_assert_pt._translation_shd.x, _fbx_assert_pt._translation_shd.y, _fbx_assert_pt._translation_shd.z));
					model = glm::scale(model, glm::vec3(_fbx_assert_pt._scale_stn.x, _fbx_assert_pt._scale_stn.y, _fbx_assert_pt._scale_stn.z));
					model = glm::rotate(
						model,
						_fbx_assert_pt._rotation_srd.x * glm::radians(1.0f),
						glm::vec3(1.0f, 0.0f, 0.0f)
						);

					model = glm::rotate(
						model,
						_fbx_assert_pt._rotation_srd.y * glm::radians(1.0f),
						glm::vec3(0.0f, 1.0f, 0.0f)
						);

					model = glm::rotate(
						model,
						_fbx_assert_pt._rotation_srd.z * glm::radians(1.0f),
						glm::vec3(0.0f, 0.0f, 1.0f)
						);

					glm::mat4 view = glm::mat4(1.f);
					view = glm::lookAt(glm::vec3(_fbx_assert_pt._camera_eye_shd.x, _fbx_assert_pt._camera_eye_shd.y, _fbx_assert_pt._camera_eye_shd.z),
						glm::vec3(_fbx_assert_pt._camera_center_stn.x, _fbx_assert_pt._camera_center_stn.y, _fbx_assert_pt._camera_center_stn.z),
						glm::vec3(_fbx_assert_pt._camera_up_srd.x, _fbx_assert_pt._camera_up_srd.y, _fbx_assert_pt._camera_up_srd.z));

					glm::mat4 proj = glm::perspective(glm::radians(_fbx_assert_pt._proj_fov_urd), (float)_fbx_assert_pt._proj_width / (float)_fbx_assert_pt._proj_height, _fbx_assert_pt._proj_linear_uhd, _fbx_assert_pt._proj_far_uhd);

					glm::vec4 eye = glm::vec4(_fbx_assert_pt._camera_eye_shd.x, _fbx_assert_pt._camera_eye_shd.y, _fbx_assert_pt._camera_eye_shd.z, 0.f);
					glm::vec4 lightColor = glm::vec4(_fbx_assert_pt._light_clr.x, _fbx_assert_pt._light_clr.y, _fbx_assert_pt._light_clr.z, 1.f);
					glm::vec4 lightPos = glm::vec4(_fbx_assert_pt._light_pos_uhd.x, _fbx_assert_pt._light_pos_uhd.y, _fbx_assert_pt._light_pos_uhd.z, 0.f);

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