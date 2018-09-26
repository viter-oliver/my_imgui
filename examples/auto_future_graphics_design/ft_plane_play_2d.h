#pragma once
#include "ft_base.h"
#include "material.h"
#include "res_output.h"
#include "primitive_object.h"
#define FILE_NAME_LEN 50
namespace auto_future
{
	class AFG_EXPORT ft_plane_play_2d :
		public ft_base
	{
		struct intl_pt 
		{
			char _materil_name[FILE_NAME_LEN];
			char _texture_name[FILE_NAME_LEN];
			char _texture_fmt_name[FILE_NAME_LEN];
			int _frame_index = { 0 };
			ImVec2 _scale = {1.f,1.f};
			ImVec2 _trans ;
			float _rotation_z = { 0.f };
			intl_pt()
			{
				memset(_materil_name, 0, FILE_NAME_LEN);
				memset(_texture_name, 0, FILE_NAME_LEN);
				memset(_texture_fmt_name, 0, FILE_NAME_LEN);
			}
		};
		intl_pt _pt;
		GLfloat* _puv;
		GLuint _vbo_pos;
		GLuint _vbo_uv;
		GLuint _vao;
		int _uv_len;
		shared_ptr<material> _pmaterial;
		shared_ptr<af_texture> _texture;
		//shared_ptr<primitive_object> _plain_data;
	public:
		ft_plane_play_2d();
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int len = sizeof(intl_pt);
			vplist.emplace_back(&_pt, len);
			len += plen;
			return len;
		}
		void link();
		int get_cur_frame_index(){ return _pt._frame_index; }
		void set_frame_index(int frame_index){ _pt._frame_index = frame_index; }
		int get_frames_count(){
			return _uv_len / 8;
		}
		~ft_plane_play_2d(){ 
			if (_puv)
			{
				delete[] _puv;
			}
			glDeleteVertexArrays(1, &_vao);
			glDeleteBuffers(1, &_vbo_pos);
			glDeleteBuffers(1, &_vbo_uv);
		}
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page(int property_part);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};
	REGISTER_CONTROL(ft_plane_play_2d)
}
