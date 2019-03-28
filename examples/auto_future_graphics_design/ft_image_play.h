#pragma once
#include "ft_base.h"
#include "res_output.h"
#define FILE_NAME_LEN 50
namespace auto_future
{
	class AFG_EXPORT ft_image_play :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(intl_pt,_img_pt,
			(char, _texture_name[FILE_NAME_LEN]), 
			(float, _aposx, { 0.f }),
			(float, _aposy, { 0.f }),			
			(char, _texture_fmt_name[FILE_NAME_LEN]),
			(int, _frame_index, {0}),
			(float, _angle_nml, {0.f}))
		shared_ptr<af_texture> _ps_texture;
		vres_txt_cd _vtexture_cd;
	public:
		ft_image_play();
		~ft_image_play(){}
		void link();
		int get_cur_frame_index(){ return _img_pt._frame_index; }
		void set_frame_index(int frame_index){ _img_pt._frame_index = frame_index; }
		int get_frames_count(){
			return _vtexture_cd.size();
		}
		void draw();
		void rotate(float angle){ _img_pt._angle_nml = angle; }
	};

	REGISTER_CONTROL(ft_image_play)
}
