#pragma once
#include "ft_base.h"
#include "ft_image.h"
namespace zl_future
{
	class AFG_EXPORT ft_slider_thumb :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(intl_pt, _img_pt,
			(float, _min_point, {0.f}),
			(float, _max_point, { 100.f }),
			(float, _progress_value_uhd, { 0.f }),
			(float, _pos_minx, { 0.f }),
			(float, _pos_miny, { 0.f }),
			(float, _size_minw, { 20.f }),
			(float, _size_minh, { 20.f }),
			(float, _pos_maxx, { 20.f }),
			(float, _pos_maxy, { 20.f }),
			(float, _size_maxw, { 40.f }),
			(float, _size_maxh, { 40.f }),
			(af_vi2, _thumb_id_txt))
		ft_image _thumb;
#if !defined(DISABLE_DEMO)
		bool _keep_min_scale{false};
		float _w2h_min{ 1.0 };
		bool _keep_max_scale{ false };
		float _w2h_max{ 1.0 };
#endif
	public:
		ft_slider_thumb();
		~ft_slider_thumb(){}
		
        void set_progress_value(float pg_value)
		{
			if (pg_value<_img_pt._min_point)
			{
				pg_value = _img_pt._min_point;
			}
			else if (pg_value > _img_pt._max_point)
			{
				pg_value = _img_pt._max_point;
			}
			_img_pt._progress_value_uhd=pg_value;
		}
		void calcu_thumb_pos();
		float get_progress_value()
		{
			return _img_pt._progress_value_uhd;
		}
		ImVec2 thumb_base_pos()
		{
			return _thumb.base_pos();
		}
		ImVec2 thumb_size()
		{
			float w, h;
			_thumb.get_size(w, h);
			return ImVec2(w,h);
		}
		void draw();
		

	};
	REGISTER_CONTROL(ft_slider_thumb)
}
