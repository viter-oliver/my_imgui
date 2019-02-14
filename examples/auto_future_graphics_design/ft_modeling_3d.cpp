#include "ft_modeling_3d.h"
#include "af_shader_source_code.h"
#include "af_model.h"
#include "primitive_object.h"
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
		auto shd_modeling = g_af_shader_list.find(modeling);
		if (shd_modeling==g_af_shader_list.end())
		{
			_pshd_modeling = make_shared<af_shader>(modeling_vs, modeling_fs);
			_pshd_modeling->set_name(modeling);
			g_af_shader_list[modeling] = _pshd_modeling;
		}
		else
		{
			_pshd_modeling = shd_modeling->second;
		}
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
	}

	ft_modeling_3d::~ft_modeling_3d()
	{
	}
	
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
		my_shader.uniform("model", glm::value_ptr(model));
		my_shader.uniform("view", glm::value_ptr(view));
		my_shader.uniform("projection", glm::value_ptr(proj));
		my_shader.uniform("light.ambient", (float*)&_pty_page._light_ambient_clr);
		my_shader.uniform("light.diffuse", (float*)&_pty_page._light_diffuse_clr);
		my_shader.uniform("light.specular", (float*)&_pty_page._light_specular_clr);
		my_shader.uniform("light.position", (float*)&_pty_page._light_position_shd);
		my_shader.uniform("light.constant", _pty_page._light_constant);
		my_shader.uniform("light.linear", _pty_page._light_linear);
		my_shader.uniform("light.quadratic", _pty_page._light_quadratic);

		//my_shader.uniform("")
		for (auto& amesh:my_model)
		{
			auto& iprmid = g_primitive_list.find(amesh._prm_id);
			if (iprmid==g_primitive_list.end())
			{
				continue;
			}
			auto& primid=*(iprmid->second);
			auto& diffuse_list = amesh._text_diffuse_list;
			auto& specular_list = amesh._text_specular_list;
			//auto& height_list = amesh._text_height_list;
			//auto& ambient_list = amesh._text_ambient_list;
			int ix = 0,itx_cnt=0;
			for (auto& specular:diffuse_list)
			{
				auto& itxt = g_mtexture_list.find(specular);
				if (itxt==g_mtexture_list.end())
				{
					continue;
				}
				auto& txt = *(itxt->second);
				glActiveTexture(GL_TEXTURE0 + itx_cnt);
				glBindTexture(GL_TEXTURE_2D, txt._txt_id());
				stringstream stm;
				stm << "texture_diffuse" << ix++;
				my_shader.uniform(stm.str(), itx_cnt++);
			}
			ix = 0;
			for (auto& specular : specular_list)
			{
				auto& itxt = g_mtexture_list.find(specular);
				if (itxt==g_mtexture_list.end())
				{
					continue;
				}
				auto& txt = *(itxt->second);
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