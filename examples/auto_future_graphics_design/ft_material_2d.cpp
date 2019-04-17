#include "ft_material_2d.h"

namespace auto_future
{
	ft_material_2d::ft_material_2d()
	{
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
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
		reg_property_handle(&_pt, 1, [this](void* memb_adress)
		{
			ImGui::Combo("Draw mode:", &_pt._draw_mode, draw_mode, en_gl_count);
		});
		reg_property_handle(&_pt, 2, [this](void* memb_adress)
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
				if (ImGui::Button("Link"))
				{
					auto& iprm = g_primitive_list.find(_pt._primitive_name);
					if (iprm!=g_primitive_list.end())
					{
						_ps_prm = iprm->second;
						if (_ps_mtl)
						{
							auto& ps_sd = _ps_mtl->get_shader();
							_matched = ps_sd->match_format(_ps_prm->_ele_format);
						}
					}
				}
			}
		});
		reg_property_handle(&_pt, 3, [this](void* memb_adress)
		{
			if (_ps_mtl)
			{
				ImGui::Text("Material name:%s", _pt._primitive_name);
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
				if (ImGui::Button("Link"))
				{
					auto& imtl = g_material_list.find(_pt._material_name);
					if (imtl!=g_material_list.end())
					{
						_ps_mtl = imtl->second;
					}
					if (_ps_prm)
					{
						auto& ps_sd = _ps_mtl->get_shader();
						_matched = ps_sd->match_format(_ps_prm->_ele_format);
					}
				}
			}

		});

#endif
	}

	void ft_material_2d::draw()
	{
		if (_matched)
		{

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
			_ps_mtl->use();
			auto& primid = *_ps_prm;
			glBindVertexArray(primid._vao);
			GLuint& dml = draw_model[_pt._draw_mode];
			if (_pt._draw_array)
			{
				glDrawArrays(dml, 0, primid._ele_buf_len);
			}
			else
			{
				glDrawElements(dml, primid._ele_buf_len, GL_UNSIGNED_INT, 0);
			}
		}
		ft_base::draw();
	}

}
