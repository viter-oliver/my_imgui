#include "ft_video.h"
#include "res_output.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "common_functions.h"
#include "SOIL.h"
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "video_capture.h"
#endif
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _txt_width, _txt_height,
			0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}
	void ft_video::init_pbo()
	{
		auto data_size = _txt_width*_txt_height * 4;
		glGenBuffers(2, _pboIds);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pboIds[0]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, data_size, 0, GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pboIds[1]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, data_size, 0, GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
	void ft_video::release_res()
	{
		if (_txt_id!=0)
		{
			glDeleteTextures(1, &_txt_id);
			_txt_id = 0;
		}
		if (_pboIds[0]!=0)
		{
			glDeleteBuffers(2, _pboIds);
			_pboIds[0] = 0;
			_pboIds[1] = 0;
		}
	}
	ft_video::ft_video()
		:ft_base()
	{
		//init_txt_obj();
		_pboIds[0] = _pboIds[1] = 0;
          set_size( 800, 800 );
		memset(_video_dev_name, 0, name_len);
          _img_pt._tin_clr = { 1.f, 1.f, 1.f };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

		reg_property_handle(&_img_pt, 2, [this](void*){
			ImGui::SliderFloat("rotate angle:", &_img_pt._angle_nml,0.f,1.f);
			if (_linked)
			{
				ImGui::Text("height of texture:%d", _txt_height);
				ImGui::Text("width of texture:%d", _txt_width);
				ImGui::Text("video device:%s", _video_dev_name);
				if (ImGui::Button("Delink"))
				{
					delink();
				}
			}
			else
			{
				ImGui::InputText("video device:", _video_dev_name, name_len);
				if (ImGui::Button("Link video device"))
				{
					if (video_link(this,_txt_width,_txt_height,_video_dev_name))
					{
						init_txt_obj();
						init_pbo();
						_linked = true;
					}
				}
			}
		}); 
#endif
	}
	ft_video::~ft_video()
	{
		release_res();
	}
	void ft_video::delink()
	{
		#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		_linked = false;
		#endif
		release_res();
	}
	void ft_video::draw()
	{
		float sizew = _in_p._sizew;
		float sizeh = _in_p._sizeh;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };
		float offsetx = abpos.x - base_pos().x;
		float offsety = abpos.y - base_pos().y;
		ImVec2 axisBasePos = { offsetx + _img_pt._aposx + winpos.x, offsety + _img_pt._aposy + winpos.y };

		if( _linked )
		{
			ImVec2 uv0(0.f,0.f);
			ImVec2 uv1(0.f, 1.f); 
			ImVec2 uv2(1.f, 1.f); 
			ImVec2 uv3(1.f, 0.f);

			if (_img_pt._angle_nml != 0.f)
			{
				pos1 = rotate_point_by_zaxis(pos1, _img_pt._angle_nml, axisBasePos);
				pos2 = rotate_point_by_zaxis(pos2, _img_pt._angle_nml, axisBasePos);
				pos3 = rotate_point_by_zaxis(pos3, _img_pt._angle_nml, axisBasePos);
				pos4 = rotate_point_by_zaxis(pos4, _img_pt._angle_nml, axisBasePos);
			}
			ImVec4 tin_clr( _img_pt._tin_clr.x, _img_pt._tin_clr.y, _img_pt._tin_clr.z, _img_pt._alpha_nml );
			ImGui::ImageQuad((ImTextureID)_txt_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3,tin_clr);
		}

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

	void ft_video::update_pixels(GLubyte* dst, int sz)
	{
		static bool index = false;
		auto data_size = _txt_width*_txt_height * 4;
		assert(sz == data_size);
		bool nindex = !index;
		glBindTexture(GL_TEXTURE_2D, _txt_id);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pboIds[index]);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _txt_width, _txt_height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pboIds[nindex]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, data_size, 0, GL_STREAM_DRAW);
		#if 1
		GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0,data_size,GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		if (ptr)
		{
			memcpy(ptr, dst, data_size);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		}
		#endif
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		index = nindex;
	}

	void ft_video::update_text_from_image_buff( GLubyte* pimage_buff, int buff_len )
     {
          int tw, th,ch;
          GLubyte* image_txt_data = SOIL_load_image_from_memory( pimage_buff, buff_len, &tw, &th, &ch, SOIL_LOAD_RGBA);
          if (!_linked)
          {
               _txt_width = tw;
               _txt_height = th;
               init_txt_obj();
               init_pbo();
               _linked = true;
          }
          else
          {
               if (_txt_width!=tw|| _txt_height!=th)
               {
                    _txt_width = tw;
                    _txt_height = th;
                    release_res();
                    init_txt_obj();
                    init_pbo();
               }
          }
          auto is_txt = tw*th*4;
          update_pixels( image_txt_data, is_txt );
          SOIL_free_image_data( image_txt_data );
     }
}
