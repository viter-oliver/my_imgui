#pragma once
#include "ft_base.h"
#include "ft_image.h"
namespace auto_future
{
	class AFG_EXPORT ft_slider_thumb :
		public ft_base
	{
		struct intl_pt
		{
			float _min_point = { 0.f }, _max_point = { 100.f }, _progress_value = {0.f};
			float _pos_minx, _pos_miny, _pos_maxx, _pos_maxy;
			float _size_minw, _size_minh, _size_maxw, _size_maxh;
			int _thumb_id_txt{0};
			intl_pt(){}
		};
		intl_pt _img_pt;
		ft_image _thumb;

	public:
		ft_slider_thumb() :ft_base(), _img_pt(){}
		~ft_slider_thumb(){}
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int tlen = _thumb.collect_property_range(vplist);
			int len = sizeof(intl_pt);
			vplist.emplace_back(&_img_pt, len);
			len += plen;
			len += tlen;
			return len;
		}
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
			_img_pt._progress_value=pg_value;
		}
		void calcu_thumb_pos();
		float get_progress_value()
		{
			return _img_pt._progress_value;
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
		void draw_peroperty_page(int property_part = -1);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};
	REGISTER_CONTROL(ft_slider_thumb)
}
