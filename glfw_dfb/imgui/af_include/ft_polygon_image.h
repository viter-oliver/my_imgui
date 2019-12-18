#pragma once
#include "ft_base.h"
#include "res_output.h"
namespace auto_future
{
#define MAX_POS_PAIR_CNT 20
	class AFG_EXPORT ft_polygon_image :
		public ft_base
	{
		DEF_STRUCT_WITH_INIT(pg_img_pt,_img_pt,
			(char, _track_file[name_len]),
			(float, _progrss_nml, {0.}),
			(int, _img_txt, {0})
			)
	public:
		void link();
		ft_polygon_image();
		void draw();
	private:
		void read_track_file(const char* track_file);
		vector<ImVec2> _track0;
		vector<ImVec2> _track1;
		vector<float> _track_segs0;
		vector<float> _track_segs1;
		float _seg_len0, _seg_len1;
		ps_af_file _ps_track_file;
	};
	REGISTER_CONTROL(ft_polygon_image)
}