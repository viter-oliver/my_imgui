#include "ft_slider_thumb.h"

namespace zl_future
{
	ft_slider_thumb::ft_slider_thumb()
	{
#if !defined(DISABLE_DEMO)
		reg_property_handle(&_img_pt, 5, [this](void*){
			if (ImGui::Checkbox("Keep min scale", &_keep_min_scale) && _keep_min_scale)
			{
				if (_img_pt._size_minw > 0)
				{
					_w2h_min = _img_pt._size_minw / _img_pt._size_minh;
				}
			}
			if (ImGui::SliderFloat("min width", &_img_pt._size_minw, 1.0, base_ui_component::screenw) && _keep_min_scale&&_w2h_min > 0.001)
			{
				_img_pt._size_minh = _img_pt._size_minw / _w2h_min;
			}
		});
		reg_property_handle(&_img_pt, 6, [this](void*){
			if (ImGui::SliderFloat("min height", &_img_pt._size_minh, 1.0, base_ui_component::screenh) && _keep_min_scale)
			{
				_img_pt._size_minw = _img_pt._size_minh*_w2h_min;
			}
			ImGui::Spacing();
			ImGui::Spacing();
		});
		reg_property_handle(&_img_pt,9, [this](void*){
			if (ImGui::Checkbox("Keep max scale", &_keep_max_scale) && _keep_max_scale)
			{
				if (_img_pt._size_maxw > 0)
				{
					_w2h_max = _img_pt._size_maxw / _img_pt._size_maxh;
				}
			}
			if (ImGui::SliderFloat("max width", &_img_pt._size_maxw, 1.0, base_ui_component::screenw) && _keep_max_scale&&_w2h_max > 0.001)
			{
				_img_pt._size_maxh = _img_pt._size_maxw / _w2h_max;
			}
		});
		reg_property_handle(&_img_pt, 10, [this](void*){
			if (ImGui::SliderFloat("max height", &_img_pt._size_maxh, 1.0, base_ui_component::screenh) && _keep_max_scale)
			{
				_img_pt._size_maxw = _img_pt._size_maxh*_w2h_max;
			}
		});
		

#endif
	}
	void ft_slider_thumb::draw()
	{
	    auto _thumb_id_txt=_img_pt._thumb_id_txt;
		if (_thumb.get_texture_id()!=_thumb_id_txt)
		{
			_thumb.set_texture_id(_thumb_id_txt);
		}
		calcu_thumb_pos();
#if !defined(DISABLE_DEMO)
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

}