#include "ft_slider_thumb.h"

namespace auto_future
{
	void ft_slider_thumb::draw()
	{
		if (_thumb.get_texture_id()!=_img_pt._thumb_id_txt)
		{
			_thumb.set_texture_id(_img_pt._thumb_id_txt);
		}
		calcu_thumb_pos();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		_thumb.set_selected(is_selected());
#endif
		_thumb.draw();
	}
	void ft_slider_thumb::calcu_thumb_pos()
	{
		float span_value = _img_pt._max_point - _img_pt._min_point;
		float scale_value = span_value == 0 ? 0 : _img_pt._progress_value_uhd / span_value;
		float scan_x_value = _img_pt._pos_maxx - _img_pt._pos_minx;
		_thumb.set_base_posx( _img_pt._pos_minx + scale_value*scan_x_value);
		float scan_y_value = _img_pt._pos_maxy - _img_pt._pos_miny;
		_thumb.set_base_posy ( _img_pt._pos_miny + scale_value*scan_y_value);
		float w = _img_pt._size_minw + scale_value*(_img_pt._size_maxw - _img_pt._size_minw);
		float h = _img_pt._size_minh + scale_value*(_img_pt._size_maxh - _img_pt._size_minh);
		_thumb.set_size(ImVec2(w, h));
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

#endif
}