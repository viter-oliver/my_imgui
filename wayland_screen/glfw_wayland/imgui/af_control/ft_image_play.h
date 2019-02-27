#pragma once
#include "ft_base.h"
#include "res_output.h"
#define FILE_NAME_LEN 50
namespace auto_future
{
	class AFG_EXPORT ft_image_play :
		public ft_base
	{
		struct intl_pt
		{
			char _texture_name[FILE_NAME_LEN];
			char _texture_fmt_name[FILE_NAME_LEN];

			ImVec2 _size;
			ImVec2 _axis_pos;
			int _frame_index = { 0 };
			float _angle = { 0.f };
			intl_pt() {
				memset(_texture_name, 0, FILE_NAME_LEN);
				memset(_texture_fmt_name, 0, FILE_NAME_LEN);
			}
		};
		intl_pt _img_pt;
		shared_ptr<af_texture> _texture;
		vres_txt_cd _vtexture_cd;
	public:
		ft_image_play() 
			:ft_base(), _img_pt()
		{}
		~ft_image_play()
		{}
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int len = sizeof(intl_pt);
			vplist.emplace_back(&_img_pt, len);
			len += plen;
			return len;
		}
		void link();
		int get_cur_frame_index(){ return _img_pt._frame_index; }
		void set_frame_index(int frame_index){ _img_pt._frame_index = frame_index; }
		int get_frames_count(){
			return _vtexture_cd.size();
		}
		ImVec2 get_size()
		{
			return _img_pt._size;
		}
		void set_size(ImVec2& im_size)
		{
			_img_pt._size = im_size;
		}
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		enum 
		{
			en_parent_property=1,
			en_geometry_property=2,
			en_texture_property=8,
		};
		void draw_peroperty_page(int property_part = -1);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif

		void rotate(float angle){ _img_pt._angle = angle; }
	};

	REGISTER_CONTROL(ft_image_play)
}
