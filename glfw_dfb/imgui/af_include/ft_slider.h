#pragma once
#include <vector>
#include <math.h>
#include "ft_base.h"
#include "res_output.h"

namespace auto_future
{

	typedef vector<point_pair> point_pair_vec;
	class AFG_EXPORT ft_slider :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(intl_pt, _slider_pt,
			(float, _bg_txtw, { 20.f }),
			(float, _bg_txth, { 20.f }),
			(char, _cbuffer_random_text[128]),
			(float, _hd_txtw, {20.f}),
			(float, _hd_txth, { 20.f }),
			(float, _tb_txtw, { 20.f }),
			(float, _tb_txth, { 20.f }),
			(float, _progress_nml, { 0.f }),
			(int, _direction_item, {0}),
			(float, _bg_aposx, {0.f}),
			(float, _bg_aposy, {0.f}),
			(float, _bg_angle_nml, {0.f}),
			(int, _texture_bg_index_txt, {0}),
			(float, _hd_posx, { 20.f }),
			(float, _hd_posy, { 20.f }),
			(int, _texture_head_index_txt,{0}),
			(float, _tb_height, { 20.f }),
			(float, _tb_offset, { 0.f }),
			(bool, _thumb_visible, {false}),
			(int, _texture_thumb_index_txt, {0}))
	public:
		ft_slider();
		~ft_slider(){}
		void link();
		void set_progress(float value){ _slider_pt._progress_nml = value; }
		float get_progress(){ return _slider_pt._progress_nml; }

		void draw();
	private:
		bool read_point_position_file(const char *str);
		//point_pair_vec _custom_envelope;
		shared_ptr<af_file> _ps_track_file;
		vector<ImVec2> _custom_track0;
		vector<ImVec2> _custom_track1;
		vector<ImVec2> _custom_mid_track;
		vector<float> _custom_track_segment;
		float _custom_trace_length{ 0.f };
	};

	REGISTER_CONTROL(ft_slider)
}
