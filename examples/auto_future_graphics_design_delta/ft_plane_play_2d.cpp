#include "ft_plane_play_2d.h"
#include "common_functions.h"
namespace auto_future
{
	GLfloat _plain_vertices[] = {
		-0.5f, -0.5f,
		0.5f, -0.5f, 
		-0.5f, 0.5f,
		0.5f, 0.5f, 
	};
	
	ft_plane_play_2d::ft_plane_play_2d()
		:ft_base(),_pt(), _puv(0), _uv_len(0)
	{
		_pt._scale = { 1.f, 1.f };
		memset(_pt._materil_name, 0, FILE_NAME_LEN);
		memset(_pt._texture_name, 0, FILE_NAME_LEN);
		memset(_pt._texture_fmt_name, 0, FILE_NAME_LEN);

		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo_pos);
		glGenBuffers(1, &_vbo_uv);
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_plain_vertices), _plain_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		glBindBuffer(GL_ARRAY_BUFFER, _vbo_uv);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_plain_vertices), 0, GL_DYNAMIC_DRAW);//GL_STREAM_DRAW
        glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
	}

	void ft_plane_play_2d::link()
	{
		
		auto amtl = g_material_list.find(_pt._materil_name);
		if (amtl!=g_material_list.end())
		{
			_pmaterial = amtl->second;
		}
		auto atex = g_mtexture_list.find(_pt._texture_name);
		if (atex!=g_mtexture_list.end())
		{
			_texture = atex->second;
		}
		auto amuf = g_mfiles_list.find(_pt._texture_fmt_name);
		if (amuf!=g_mfiles_list.end())
		{
			_puv = get_txt_uvs((char*)amuf->second->_pbin, _uv_len);
		}
	}

	void ft_plane_play_2d::draw()
	{
		if (!_puv||!_pmaterial||!_texture)
		{
			return;
		}
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model,glm::vec3(_pt._trans.x, _pt._trans.y, 0));
		model = glm::scale(model, glm::vec3(_pt._scale.x, _pt._scale.y, 1.f));
		model = glm::rotate(model, _pt._rotation_z*glm::radians(1.f), glm::vec3(0.0f, 0.0f, 1.0f));
		_pmaterial->set_value("model", glm::value_ptr(model), 16);
		_pmaterial->use();
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo_uv);
		float* puv = _puv + _pt._frame_index * 8;
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * 8, puv);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture->_txt_id);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void ft_plane_play_2d::draw_peroperty_page(int property_part)
	{
		ft_base::draw_peroperty_page();
		ImGui::Spacing();
		ImGui::InputText("material:", _pt._materil_name, FILE_NAME_LEN);
		ImGui::SameLine();
		if (!_pmaterial&&ImGui::Button("loading..."))
		{
			auto amtl = g_material_list.find(_pt._materil_name);
			if (amtl != g_material_list.end())
			{
				_pmaterial = amtl->second;
			}
			else
			{
				ImGui::Text("fail to find material:%s", _pt._materil_name);
			}
		}
		ImGui::Spacing();
		ImGui::InputText("texture:", _pt._texture_name, FILE_NAME_LEN);
		ImGui::SameLine();
		if (!_texture&&ImGui::Button("load..."))
		{
			auto atxt = g_mtexture_list.find(_pt._texture_name);
			if (atxt != g_mtexture_list.end())
			{
				_texture = atxt->second;
			}
			else
			{
				ImGui::Text("fail to find texture:%s", _pt._texture_name);
			}
		}
		ImGui::Spacing();
		ImGui::InputText("texture format:", _pt._texture_fmt_name, FILE_NAME_LEN);
		ImGui::SameLine();
		if (!_puv&&ImGui::Button("load..."))
		{
			auto atxt_fmt = g_mfiles_list.find(_pt._texture_fmt_name);
			if (atxt_fmt != g_mfiles_list.end())
			{
				_puv = get_txt_uvs((char*)atxt_fmt->second->_pbin, _uv_len);
			}
			else
			{
				ImGui::Text("fail to find texture format:%s", _pt._texture_fmt_name);
			}
		}
		ImGui::Spacing();
		int frame_cnt = get_frames_count();
		ImGui::SliderInt("frame index:", &_pt._frame_index, 0, frame_cnt);
		ImGui::Text("Translation:");
		ImGui::SliderFloat("tx", &_pt._trans.x, -100.f, 100.f,"%.4f",0.05f);
		ImGui::SliderFloat("ty", &_pt._trans.y, -100.f, 100.f, "%.4f", 0.05f);
		ImGui::Text("scale:");
		ImGui::SliderFloat("sx", &_pt._scale.x, -10.f, 10.f);
		ImGui::SliderFloat("sy", &_pt._scale.y, -10.f, 10.f);
		ImGui::Text("Rotation:");
		ImGui::SliderFloat("rz", &_pt._rotation_z, -360.f, 360.f);


	}

	bool ft_plane_play_2d::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		strcpy(_pt._materil_name, jvalue["material name"].asCString());
		strcpy(_pt._texture_name, jvalue["texture name"].asCString());
		strcpy(_pt._texture_fmt_name, jvalue["texture format"].asCString());
		_pt._frame_index = jvalue["frame index"].asInt();
		link();
		return true;
	}

	bool ft_plane_play_2d::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		junit["material name"] = _pt._materil_name;
		junit["texture name"] = _pt._texture_name;
		junit["texture format"] = _pt._texture_fmt_name;
		junit["frame index"]=_pt._frame_index;
		return true;
	}
#endif
}