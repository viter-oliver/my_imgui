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
			(float, _aposx, { 0.f }),
			(float, _aposy, { 0.f }),			
			(float, _angle_nml, {0.f}))
		GLuint _txt_id = {0};
		GLuint _pboIds[2];
		int _txt_width, _txt_height;
		char _video_dev_name[name_len];
		bool _linked=false;
		void init_txt_obj();
		void init_pbo();
		void release_res();
	public:
		ft_video();
		~ft_video();
		void delink();
		void draw();
		void update_pixels(GLubyte* dst, int sz);
		//GLuint get_txt_id(){ return _txt_id; }

	};

	REGISTER_CONTROL(ft_video)
}
