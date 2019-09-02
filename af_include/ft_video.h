#pragma once
#include "ft_base.h"
#include "res_output.h"
#define FILE_NAME_LEN 50
namespace auto_future
{
	class AFG_EXPORT ft_video :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(intl_pt,_img_pt,
			(int,_txt_width, {600}),
			(int, _txt_height, { 480 }),
			(float, _aposx, { 0.f }),
			(float, _aposy, { 0.f }),			
			(float, _angle_nml, {0.f}))
		GLuint _txt_id;
		void init_txt_obj();
	public:
		ft_video();
		~ft_video(){}
		void link();
		void draw();
		void rotate(float angle){ _img_pt._angle_nml = angle; }
		GLuint get_txt_id(){ return _txt_id; }
	};

	REGISTER_CONTROL(ft_video)
}
