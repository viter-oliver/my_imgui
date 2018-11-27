#include "ft_plot_lines.h"
namespace auto_future
{

	void ft_plot_lines::draw()
	{
		ft_base::draw();
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		//ImVec2 winpos = ImGui::GetWindowPos();
		ImGui::SetCursorPosX(abpos.x);
		ImGui::SetCursorPosY(abpos.y);
		ImGui::PlotLines("Lines", _values, _pt._v_count, 0, "avg 0.0", -1.0f, 1.0f, ImVec2(_pt._sizew, _pt._sizeh));

	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	
	void ft_plot_lines::draw_peroperty_page(int)
	{
		ft_base::draw_peroperty_page();
		ImGui::SliderFloat("sw", &_pt._sizew, 0.f, base_ui_component::screenw);
		ImGui::SliderFloat("sh", &_pt._sizeh, 0.f, base_ui_component::screenh);
		ImGui::SliderInt("count of value:", &_pt._v_count, 3, MAX_VALUE_COUNT,"%.0f");
		ImGui::SliderFloatN("values:", _values, _pt._v_count, -1, 1, "%.3f", 1.0f);
	}
	bool ft_plot_lines::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		Value& jsize = jvalue["size"];
		_pt._sizew = jsize["w"].asDouble();
		_pt._sizeh = jsize["h"].asDouble();
		_pt._v_count= jvalue["value_count"].asInt();
		return true;
	}
	bool ft_plot_lines::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		Value jsize(objectValue);
		jsize["w"] = _pt._sizew;
		jsize["h"] = _pt._sizeh;
		junit["size"] = jsize;
		junit["value_count"] = _pt._v_count;
		return true;
	}
#endif
}
