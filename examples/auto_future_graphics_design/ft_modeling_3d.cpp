#include "ft_modeling_3d.h"
#include "af_shader_source_code.h"
#include "af_model.h"
#include "af_primitive_object.h"
#include "res_output.h"
#include <sstream>
namespace auto_future
{
	ft_modeling_3d::ft_modeling_3d()
	{
		memset(_pty_page._model_name, 0, FILE_NAME_LEN);
		_pty_page._cam = { { 0.f, 0.f, 3.f }, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f } };
		_pty_page._pj = { 60.f,0.1f, 100.f };
		_pty_page._trans = { { 0.15f, 0.15f, 0.15f }, { 0, 0, 0 }, { 0, 0, 0 },true };
		_pty_page._light_ambient_clr = { 0.2f, 0.2f, 0.2f };
		_pty_page._light_diffuse_clr = { 0.5f, 0.5f, 0.5f };
		_pty_page._light_specular_clr = { 1.f, 1.f, 1.f };
		_pty_page._light_position_shd = { -1.f, 1.f, -2.f };
		string str_modeling(modeling);
		auto shd_modeling = g_af_shader_list.find(str_modeling);
		if (shd_modeling==g_af_shader_list.end())
		{
			_pshd_modeling = make_shared<af_shader>(modeling_vs, modeling_fs);
			_pshd_modeling->set_name(str_modeling);
			_pshd_modeling->_vs_name = str_modeling + ".vs";
			_pshd_modeling->_fs_name = str_modeling + ".fs";
			g_af_shader_list[modeling] = _pshd_modeling;
		}
		else
		{
			_pshd_modeling = shd_modeling->second;
		}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_pty_page, 0, [this](void*member_address){
			ImGui::InputText("Model name:", _pty_page._model_name, FILE_NAME_LEN);
			if (ImGui::Button("import"))
			{
				auto imodel = g_mmodel_list.find(_pty_page._model_name);
				if (imodel!=g_mmodel_list.end())
				{
					_pmodel = imodel->second;
				}
			}
		});
		reg_property_handle(&_pty_page, 7, [this](void* member_address){
			ImGui::Text("Light position:");
			auto parent = get_parent();
			float w, h;
			parent->get_size(w, h);
			ImGui::SliderFloat("X", &_pty_page._light_position_shd.x, w, -w);
			ImGui::SliderFloat("Y", &_pty_page._light_position_shd.y, h, -h);
			ImGui::SliderFloat("Z", &_pty_page._light_position_shd.z, h, -h);
		});
#endif
	}

	ft_modeling_3d::~ft_modeling_3d()
	{
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void ft_modeling_3d::view_components_in_world_space()
	{
		ImGui::Text("View direction:");ImGui::SameLine();
		ImGui::RadioButton("X", &_dir_view, 0); ImGui::SameLine();
		ImGui::RadioButton("Y", &_dir_view, 1); ImGui::SameLine();
		ImGui::RadioButton("Z", &_dir_view, 2);
		ImVec2 winpos=ImGui::GetWindowPos();
		ImVec2 pos0 = winpos + ImVec2(150, 180);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
		draw_list->AddCircle(pos0, 100, col, 25);
		ImVec2 pos_left = { pos0.x - 100, pos0.y };
		ImVec2 pos_right = { pos0.x + 100, pos0.y };
		ImVec2 pos_top = { pos0.x, pos0.y - 100 };
		ImVec2 pos_bottom = { pos0.x, pos0.y + 100 };
		draw_list->AddLine(pos_left, pos_right, col);
		draw_list->AddLine(pos_top, pos_bottom, col);
		ImVec2 lt_pos = pos0;
		af_vec3& ltPos = _pty_page._light_position_shd;
		switch (_dir_view)
		{
		case 0:
			lt_pos += ImVec2(ltPos.y, ltPos.z);
			break;
		case 1:
			lt_pos += ImVec2(ltPos.x, ltPos.z);
			break;
		case 2:
			lt_pos += ImVec2(ltPos.x, ltPos.y);
			break;
		}
		af_vec3& lt_spcol = _pty_page._light_specular_clr;
		ImVec4 vlt_col = { lt_spcol.x, lt_spcol.y, lt_spcol.z, 1 };
		ImU32 lt_col = ImGui::ColorConvertFloat4ToU32(vlt_col);
		draw_list->AddCircleFilled(lt_pos, 5, lt_col);
	}
#endif	
	void ft_modeling_3d::draw()
	{
		if (!_pmodel)
		{
			return;
		}
		af_model& my_model = *_pmodel;
		af_shader& my_shader =*_pshd_modeling;
		my_shader.use();
		glm::mat4 model;
		auto& aftr = _pty_page._trans._translation;
		auto& afsc = _pty_page._trans._scale;
		auto& afrt = _pty_page._trans._rotation;
		glm::vec3 gtranslate(aftr.x, aftr.y, aftr.z);
		glm::vec3 gscale(afsc.x, afsc.y, afsc.z);
		model = glm::translate(model, gtranslate);
		model = glm::scale(model, gscale);
		model = glm::rotate(model, afrt.x*glm::radians(1.f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, afrt.y*glm::radians(1.f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, afrt.z*glm::radians(1.f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 view;
		auto& cam_pos = _pty_page._cam._position;
		auto& cam_dir = _pty_page._cam._direction;
		auto& cam_up = _pty_page._cam._up;
		glm::vec3 gcam_pos(cam_pos.x, cam_pos.y, cam_pos.z);
		glm::vec3 gcam_dir(cam_dir.x, cam_dir.y, cam_dir.z);
		glm::vec3 gcam_up(cam_up.x, cam_up.y, cam_up.z);
		view = glm::lookAt(gcam_pos, gcam_dir, gcam_up);
		glm::mat4 proj;
		auto& cproj = _pty_page._pj;
		auto pparent = get_parent();
		float w, h;
		pparent->get_size(w, h);
		float aspect = w / h;
		proj = glm::perspective(glm::radians(cproj._fovy), aspect, cproj._near, cproj._far);
		my_shader.uniform("viewPos", glm::value_ptr(gcam_pos));
		my_shader.uniform("model", glm::value_ptr(model));
		my_shader.uniform("view", glm::value_ptr(view));
		my_shader.uniform("projection", glm::value_ptr(proj));
		my_shader.uniform("light.ambient", (float*)&_pty_page._light_ambient_clr);
		my_shader.uniform("light.diffuse", (float*)&_pty_page._light_diffuse_clr);
		my_shader.uniform("light.specular", (float*)&_pty_page._light_specular_clr);
		my_shader.uniform("light.position", (float*)&_pty_page._light_position_shd);
		my_shader.uniform("light.constant", &_pty_page._light_constant);
		my_shader.uniform("light.linear", &_pty_page._light_linear);
		my_shader.uniform("light.quadratic", &_pty_page._light_quadratic);

		//my_shader.uniform("")
		for (auto& amesh:my_model)
		{
			
			auto& primid=*amesh._ps_prm_id;
			auto& ps_diffuse_list = amesh._ps_text_diffuse_list;
			auto& ps_specular_list = amesh._ps_text_specular_list;
			//auto& ps_height_list = amesh._ps_text_height_list;
			//auto& ps_ambient_list = amesh._ps_text_ambient_list;

			if (ps_diffuse_list.size()==0)
			{
				auto& diffuse_list = amesh._text_diffuse_list;
				for (auto& diff:diffuse_list)
				{
					ps_diffuse_list.emplace_back();
					auto& txt_diff = ps_diffuse_list.back();
					auto& itxt = g_mtexture_list.find(diff);
					if (itxt!=g_mtexture_list.end())
					{
						txt_diff = itxt->second;
					}
				}
				
				//auto& height_list = amesh._text_height_list;
				//auto& ambient_list = amesh._text_ambient_list;

			}
			if (ps_specular_list.size() == 0)
			{
				auto& specular_list = amesh._text_specular_list;
				for (auto& spec:specular_list)
				{
					ps_specular_list.emplace_back();
					auto& txt_spec = ps_specular_list.back();
					auto& itxt = g_mtexture_list.find(spec);
					if (itxt != g_mtexture_list.end())
					{
						txt_spec = itxt->second;
					}
				}
			}
			//auto& height_list = amesh._text_height_list;
			//auto& ambient_list = amesh._text_ambient_list;
			int ix = 0,itx_cnt=0;
			for (auto& diff:ps_diffuse_list)
			{
				auto& txt = *diff;
				glActiveTexture(GL_TEXTURE0 + itx_cnt);
				glBindTexture(GL_TEXTURE_2D, txt._txt_id());
				stringstream stm;
				stm << "texture_diffuse" << ix++;
				my_shader.uniform(stm.str(), itx_cnt++);
			}
			ix = 0;
			for (auto& specular : ps_specular_list)
			{
				auto& txt = *specular;
				glActiveTexture(GL_TEXTURE0 + itx_cnt);
				glBindTexture(GL_TEXTURE_2D, txt._txt_id());
				stringstream stm;
				stm << "texture_specular" << ix++;
				my_shader.uniform(stm.str(), itx_cnt++);
			}
			glBindVertexArray(primid._vao);
			glDrawElements(GL_TRIANGLES, primid._ele_buf_len, GL_UNSIGNED_INT, 0);
		}
	}

}