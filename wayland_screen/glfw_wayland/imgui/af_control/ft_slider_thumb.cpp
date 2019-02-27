#include "ft_slider_thumb.h"

namespace auto_future
{
	void ft_slider_thumb::draw()
	{
		calcu_thumb_pos();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		_thumb.set_selected(is_selected());
#endif
		_thumb.draw();
	}
	void ft_slider_thumb::calcu_thumb_pos()
	{
		float span_value = _img_pt._max_point - _img_pt._min_point;
		float scale_value = span_value == 0 ? 0 : _img_pt._progress_value / span_value;
		float scan_x_value = _img_pt._pos_max.x - _img_pt._pos_min.x;
		_thumb.base_pos().x = _img_pt._pos_min.x + scale_value*(scan_x_value);
		float scan_y_value = _img_pt._pos_max.y - _img_pt._pos_min.y;
		_thumb.base_pos().y = _img_pt._pos_min.y + scale_value*scan_y_value;
		float w = _img_pt._size_min.x + scale_value*(_img_pt._size_max.x - _img_pt._size_min.x);
		float h = _img_pt._size_min.y + scale_value*(_img_pt._size_max.y - _img_pt._size_min.y);
		_thumb.set_size(ImVec2(w, h));
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void ft_slider_thumb::draw_peroperty_page(int property_part)
	{
		ft_base::draw_peroperty_page();
		ImGui::Spacing();
		ImGui::SliderFloat("min pos x:", &_img_pt._pos_min.x, 0.f, base_ui_component::screenw);
		ImGui::SliderFloat("min pos y", &_img_pt._pos_min.y, 0.f, base_ui_component::screenh);
		ImGui::Spacing();
		ImGui::SliderFloat("min size w:", &_img_pt._size_min.x, 0.f, base_ui_component::screenw);
		ImGui::SliderFloat("min size h", &_img_pt._size_min.y, 0.f, base_ui_component::screenh);
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::SliderFloat("max pos x:", &_img_pt._pos_max.x, 0.f, base_ui_component::screenw);
		ImGui::SliderFloat("max pos y", &_img_pt._pos_max.y, 0.f, base_ui_component::screenh);
		ImGui::SliderFloat("max size w:", &_img_pt._size_max.x, 0.f, base_ui_component::screenw);
		ImGui::SliderFloat("max size h", &_img_pt._size_max.y, 0.f, base_ui_component::screenh);
		ImGui::Spacing();
		ImGui::SliderFloat("progress value:", &_img_pt._progress_value, 0.f, 100.f);
		
		ImGui::Spacing();
		ImVec2 tmb_size = thumb_size();
		ImVec2& tmb_pos = thumb_base_pos();
		ImGui::Text("thumb pos and size:%f,%f,%f,%f", tmb_pos.x, tmb_pos.y, tmb_size.x, tmb_size.y);

		_thumb.draw_peroperty_page(ft_image::en_texture_property);
	}

	bool ft_slider_thumb::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		Value& jmin_pos = jvalue["min_pos"];
		_img_pt._pos_min.x = jmin_pos["x"].asDouble();
		_img_pt._pos_min.y = jmin_pos["y"].asDouble();
		Value& jmax_pos = jvalue["max_pos"];
		_img_pt._pos_max.x = jmax_pos["x"].asDouble();
		_img_pt._pos_max.y = jmax_pos["y"].asDouble();
		Value& jmin_sz = jvalue["min_size"];
		_img_pt._size_min.x = jmin_sz["x"].asDouble();
		_img_pt._size_min.y = jmin_sz["y"].asDouble();
		Value& jmax_sz = jvalue["max_size"];
		_img_pt._size_max.x = jmax_sz["x"].asDouble();
		_img_pt._size_max.y = jmax_sz["y"].asDouble();
		_img_pt._progress_value = jvalue["pg_value"].asDouble();
		Value& jthmub = jvalue["thumb"];
		_thumb.init_from_json(jthmub);
		return true;
	}

	bool ft_slider_thumb::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		Value jmin_pos(objectValue);
		jmin_pos["x"] = _img_pt._pos_min.x;
		jmin_pos["y"] = _img_pt._pos_min.y;
		junit["min_pos"] = jmin_pos;
		Value jmax_pos(objectValue);
		jmax_pos["x"] = _img_pt._pos_max.x;
		jmax_pos["y"] = _img_pt._pos_max.y;
		junit["max_pos"] = jmax_pos;
		Value jmin_sz(objectValue);
		jmin_sz["x"] = _img_pt._size_min.x;
		jmin_sz["y"] = _img_pt._size_min.y;
		junit["min_size"] = jmin_sz;
		Value jmax_sz(objectValue);
		jmax_sz["x"] = _img_pt._size_max.x;
		jmax_sz["y"] = _img_pt._size_max.y;
		junit["max_size"] = jmax_sz;
		junit["pg_value"] = _img_pt._progress_value;
		Value jthumb(objectValue);
		_thumb.init_json_unit(jthumb);
		junit["thumb"] = jthumb;
		return true;
	}
#endif
}