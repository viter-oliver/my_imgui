#include "ft_material_3d.h"

namespace auto_future
{
	ft_material_3d::ft_material_3d()
	{
		strcpy(_pt._uf_model_name, "model");
		strcpy(_pt._uf_view_name, "view");
		strcpy(_pt._uf_proj_name, "projection");

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		_pt._primitive_name[0] = '\0';
		_pt._material_name[0] = '\0';
		static const char* draw_mode[en_gl_count] =
		{
			"GL_POINTS",
			"GL_LINES",
			"GL_LINE_LOOP",
			"GL_LINE_STRIP",
			"GL_TRIANGLES",
			"GL_TRIANGLE_STRIP",
			"GL_TRIANGLE_FAN"
		};
		reg_property_handle(&_pt, 0, [this](void* memb_adress)
		{
			ImGui::Combo("Draw mode:", &_pt._draw_mode, draw_mode, en_gl_count);
		});
		reg_property_handle(&_pt, 1, [this](void* memb_adress)
		{
			if (_ps_prm)
			{
				ImGui::Text("Primitive object:%s", _pt._primitive_name);
				ImGui::SameLine();
				if (ImGui::Button("Delink##prm_obj"))
				{
					_ps_prm = nullptr;
					_matched = false;
				}
			}
			else
			{
				ImGui::InputText("Primitive object:", _pt._primitive_name, FILE_NAME_LEN);
				if (ImGui::Button("Link##prm"))
				{
					auto& iprm = g_primitive_list.find(_pt._primitive_name);
					if (iprm!=g_primitive_list.end())
					{
						_ps_prm = iprm->second;
					}
					else
					{
						string str_prm_nm(_pt._primitive_name);
						if (ref_a_intenal_primitive(str_prm_nm))
						{
							_ps_prm = g_primitive_list[str_prm_nm];
						}
					}
				}
			}
		});
		reg_property_handle(&_pt, 2, [this](void* memb_adress)
		{
			if (_ps_mtl)
			{
				ImGui::Text("Material name:%s", _pt._material_name);
				ImGui::SameLine();
				if (ImGui::Button("Delink##material"))
				{
					_ps_mtl = nullptr;
					_matched = false;
				}
			}
			else
			{
				ImGui::InputText("Material name:", _pt._material_name, FILE_NAME_LEN);
				if (ImGui::Button("Link##mtl"))
				{
					auto& imtl = g_material_list.find(_pt._material_name);
					if (imtl!=g_material_list.end())
					{
						_ps_mtl = imtl->second;
					}
				}
			}
			if (!_matched)
			{
				ImGui::Text("material is unmatched to primitive object");
			}
			
		});

#endif
	}
	void ft_material_3d::link()
	{
		int imatch = 0;
		auto imt=g_material_list.find(_pt._material_name);
		if (imt!=g_material_list.end())
		{
			_ps_mtl = imt->second;
			imatch++;
		}
		
		auto iprm = g_primitive_list.find(_pt._primitive_name);
		if (iprm!=g_primitive_list.end())
		{
			_ps_prm = iprm->second;
			imatch++;
		}
		_matched = imatch==2;
	}
	void ft_material_3d::draw()
	{
#if 1//!defined(IMGUI_DISABLE_DEMO_WINDOWS)

		if (_ps_mtl&&_ps_mtl->is_valid()&&_ps_prm)
		{
			auto& ps_sd = _ps_mtl->get_shader();
			_matched = ps_sd->match_format(_ps_prm->_ele_format);
		}
		else
		{
			_matched = false;
		}
#endif
		if (_matched)
		{
			if (!_ps_mtl->is_valid()||!_ps_prm)
			{
				return;
			}
			static GLuint draw_model[en_gl_count] =
			{
				GL_POINTS,
				GL_LINES,
				GL_LINE_LOOP,
				GL_LINE_STRIP,
				GL_TRIANGLES,
				GL_TRIANGLE_STRIP,
				GL_TRIANGLE_FAN,
			};			
			glm::mat4 model;
			auto& aftr = _pt._trans._translation;
			auto& afsc = _pt._trans._scale;
			auto& afrt = _pt._trans._rotation;
			glm::vec3 gtranslate(aftr.x, aftr.y, aftr.z);
			glm::vec3 gscale(afsc.x, afsc.y, afsc.z);
			model = glm::translate(model, gtranslate);
			model = glm::scale(model, gscale);
			model = glm::rotate(model, afrt.x*glm::radians(1.f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, afrt.y*glm::radians(1.f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, afrt.z*glm::radians(1.f), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 view;
			auto& cam_pos = _pt._cam._position;
			auto& cam_dir = _pt._cam._direction;
			auto& cam_up = _pt._cam._up;
			glm::vec3 gcam_pos(cam_pos.x, cam_pos.y, cam_pos.z);
			glm::vec3 gcam_dir(cam_dir.x, cam_dir.y, cam_dir.z);
			glm::vec3 gcam_up(cam_up.x, cam_up.y, cam_up.z);
			view = glm::lookAt(gcam_pos, gcam_dir, gcam_up);
			glm::mat4 proj;
			auto& cproj = _pt._pj;
			auto pparent = get_parent();
			float w, h;
			pparent->get_size(w, h);
			float aspect = w / h;
			proj = glm::perspective(glm::radians(cproj._fovy), aspect, cproj._near, cproj._far);
			_ps_mtl->set_value(string(_pt._uf_model_name), glm::value_ptr(model), 16);
			_ps_mtl->set_value(string(_pt._uf_view_name), glm::value_ptr(view), 16);
			_ps_mtl->set_value(string(_pt._uf_proj_name), glm::value_ptr(proj), 16);

			_ps_mtl->use();
			auto& primid = *_ps_prm;
			glBindVertexArray(primid._vao);
			GLuint& dml = draw_model[_pt._draw_mode];

			if (primid._ele_buf_len==0)
			{
				glDrawArrays(dml, 0, primid._vertex_buf_len);
			}
			else
			{
				glDrawElements(dml, primid._ele_buf_len, GL_UNSIGNED_INT, 0);
			}
		}
		ft_base::draw();
	}

}
