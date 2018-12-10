#pragma once
#include "ft_base.h"
#include "ft_image.h"
namespace auto_future
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
			(float, _pos_maxx, { 0.f }),
			(float, _pos_maxy, { 0.f }),
			(float, _size_minw, { 0.f }),
			(float, _size_minh, { 0.f }),
			(float, _size_maxw, { 0.f }),
			(float, _size_maxh, { 0.f }),
			(int, _thumb_id_txt, {0}))
		ft_image _thumb;

	public:
		ft_slider_thumb() :ft_base(){}
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
			return _thumb.get_size();
		}
		void draw();
		
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};
	REGISTER_CONTROL(ft_slider_thumb)
}
