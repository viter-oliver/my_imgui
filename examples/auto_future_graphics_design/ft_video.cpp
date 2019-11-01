#include "ft_video.h"
#include "res_output.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "common_functions.h"
/*
x'=(x-a)cosα+(y-b)sinα+a
y'=-(x-a)sinα+(y-b)cosα+b
*/
namespace auto_future
{
	void ft_video::init_txt_obj()
	{
		glGenTextures(1, &_txt_id);
		glBindTexture(GL_TEXTURE_2D, _txt_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Step3 设定filter参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _img_pt._txt_width, _img_pt._txt_height,
			0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	}
	void ft_video::init_pbo()
	{
		auto data_size = _img_pt._txt_width*_img_pt._txt_height * 3;
		glGenBuffers(2, _pboIds);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pboIds[0]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, data_size, 0, GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pboIds[1]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, data_size, 0, GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
	ft_video::ft_video()
		:ft_base()
	{
		init_txt_obj();

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_img_pt, 0, [this](void*){
			if (ImGui::InputInt("width of texture", &_img_pt._txt_width))
			{
				glDeleteTextures(1, &_txt_id);
				init_txt_obj();
			}
		});
		reg_property_handle(&_img_pt, 1, [this](void*){
			if (ImGui::InputInt("height of texture", &_img_pt._txt_height))
			{
				glDeleteTextures(1, &_txt_id);
				init_txt_obj();
			}
		}); 
#endif
	}
	ft_video::~ft_video()
	{
		glDeleteBuffers(2, _pboIds);
	}
	void ft_video::draw()
	{
		
		ft_base::draw();
		
		float sizew = _in_p._sizew;
		float sizeh = _in_p._sizeh;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };

		ImVec2 uv0(0.f,0.f);
		ImVec2 uv1(0.f, 1.f); 
		ImVec2 uv2(1.f, 1.f); 
		ImVec2 uv3(1.f, 0.f);

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

		ImGui::ImageQuad((ImTextureID)_txt_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);

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

	void ft_video::link()
	{
		init_pbo();
	}

	void ft_video::update_pixels(GLubyte* dst, int sz)
	{
		static bool index = false;
		auto data_size = _img_pt._txt_width*_img_pt._txt_height * 3;
		assert(sz == data_size);
		bool nindex = !index;
		glBindTexture(GL_TEXTURE_2D, _txt_id);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pboIds[index]);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _img_pt._txt_width, _img_pt._txt_height, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pboIds[nindex]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, data_size, 0, GL_STREAM_DRAW);
		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		if (ptr)
		{
			memcpy(ptr, dst, data_size);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		}
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		index = nindex;
	}
}
