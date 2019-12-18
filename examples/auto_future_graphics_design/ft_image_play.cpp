#include "ft_image_play.h"
#include "res_output.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "common_functions.h"
/*
x'=(x-a)cos¦Á+(y-b)sin¦Á+a
y'=-(x-a)sin¦Á+(y-b)cos¦Á+b
*/
namespace auto_future
{
	ft_image_play::ft_image_play()
		:ft_base()
	{
		_img_pt._texture_name[0] = '\0';
		_img_pt._texture_fmt_name[0] = '\0';
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_img_pt, 0, [this](void*){
			if (_ps_texture)
			{
				ImGui::Text("Texture object:%s", _img_pt._texture_name);
				ImGui::SameLine();
				if (ImGui::Button("Delink ...##0"))
				{
					_img_pt._texture_name[0] = '\0';
					_ps_texture = nullptr;
				}
			}
			else
			{
				ImGui::InputText("Texture object", _img_pt._texture_name, FILE_NAME_LEN);
				if (ImGui::Button("Import texture object"))
				{
					auto atex = g_mtexture_list.find(_img_pt._texture_name);
					if (atex != g_mtexture_list.end())
					{
						_ps_texture = atex->second;
					}
				}
			}
		});
		reg_property_handle(&_img_pt, 3, [this](void*){
			if (_vtexture_cd.size()>0)
			{
				ImGui::Text("Format file:%s", _img_pt._texture_fmt_name);
				ImGui::SameLine();
				if (ImGui::Button("Delink...##1"))
				{
					_img_pt._texture_fmt_name[0] = '\0';
					_vtexture_cd.clear();
					_ps_format = nullptr;
				}

			}
			else
			{
				ImGui::InputText("Format file", _img_pt._texture_fmt_name, FILE_NAME_LEN);
				if (ImGui::Button("Import Format file"))
				{
					auto atxt_fmt = g_mfiles_list.find(_img_pt._texture_fmt_name);
					if (atxt_fmt != g_mfiles_list.end())
					{
						_ps_format = atxt_fmt->second;
						get_txt_uv_vector((char*)_ps_format->_pbin, _vtexture_cd);

					}
				}
			}
		});
		reg_property_handle(&_img_pt, 4, [this](void*){
			int frame_cnt = get_frames_count();
			ImGui::SliderInt("frame index:", &_img_pt._frame_index, 0, frame_cnt-1);
		});
#endif
	}
	void ft_image_play::draw()
	{
		if (!_ps_texture || !_vtexture_cd.size())
		{
			return;
		}
		//ft_base::draw();
		vres_txt_cd& ptext_cd = _vtexture_cd;
		if (_img_pt._frame_index >= ptext_cd.size())
		{
			//printf("invalid texture index:%d\n", _img_pt._frame_index);
			return;

		}
		int texture_id =_ps_texture->_txt_id();
		int texture_width = _ps_texture->_width;
		int texture_height = _ps_texture->_height;
		float sizew = _in_p._sizew;
		float sizeh = _in_p._sizeh;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };

		ImVec2 uv0 = ImVec2(ptext_cd[_img_pt._frame_index]._x0 / texture_width, ptext_cd[_img_pt._frame_index]._y0 / texture_height);
		ImVec2 uv1 = ImVec2(ptext_cd[_img_pt._frame_index]._x0 / texture_width, (ptext_cd[_img_pt._frame_index]._y1) / texture_height);
		ImVec2 uv2 = ImVec2((ptext_cd[_img_pt._frame_index]._x1) / texture_width, (ptext_cd[_img_pt._frame_index]._y1) / texture_height);
		ImVec2 uv3 = ImVec2((ptext_cd[_img_pt._frame_index]._x1) / texture_width, (ptext_cd[_img_pt._frame_index]._y0) / texture_height);

		float offsetx = abpos.x - base_pos().x;
		float offsety = abpos.y - base_pos().y;
		ImVec2 axisBasePos = { offsetx + _img_pt._aposx + winpos.x, offsety + _img_pt._aposy + winpos.y };
		if (_img_pt._angle_nml != 0.f)
		{
			pos1 = rotate_point_by_zaxis(pos1, _img_pt._angle_nml, axisBasePos);
			pos2 = rotate_point_by_zaxis(pos2, _img_pt._angle_nml, axisBasePos);
			pos3 = rotate_point_by_zaxis(pos3, _img_pt._angle_nml, axisBasePos);
			pos4 = rotate_point_by_zaxis(pos4, _img_pt._angle_nml, axisBasePos);
		}

		ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		if (is_selected())//draw envelope
		{
			ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
			ImVec2 editunit(edit_unit_len, edit_unit_len);
			ImVec2 pos1a = pos1 - editunit;
			ImVec2 pos1b = pos1 + editunit;
			ImGui::RenderFrame(pos1a, pos1b, col);
			ImVec2 pos2a = pos2 - editunit;
			ImVec2 pos2b = pos2 + editunit;
			ImGui::RenderFrame(pos2a, pos2b, col);
			ImVec2 pos3a = pos3 - editunit;
			ImVec2 pos3b = pos3 + editunit;
			ImGui::RenderFrame(pos3a, pos3b, col);
			ImVec2 pos4a = pos4 - editunit;
			ImVec2 pos4b = pos4 + editunit;
			ImGui::RenderFrame(pos4a, pos4b, col);
			ImVec2 posaa = axisBasePos - editunit;
			ImVec2 posab = axisBasePos + editunit;
			ImGui::RenderFrame(posaa, posab, col);

		}
#endif

	}

	void ft_image_play::link()
	{
		auto atex = g_mtexture_list.find(_img_pt._texture_name);
		if (atex != g_mtexture_list.end())
		{
			_ps_texture = atex->second;
		}
		auto atxt_fmt = g_mfiles_list.find(_img_pt._texture_fmt_name);
		if (atxt_fmt != g_mfiles_list.end())
		{
			_ps_format = atxt_fmt->second;
			get_txt_uv_vector((char*)_ps_format->_pbin, _vtexture_cd);
		}
	}
}
