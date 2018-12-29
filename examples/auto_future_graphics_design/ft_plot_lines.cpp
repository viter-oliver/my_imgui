#include "ft_plot_lines.h"
namespace auto_future
{
	ft_plot_lines::ft_plot_lines()
		:ft_base()
		,_values{}
	{ 
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_pt, 2, [this](void*){
			ImGui::SliderInt("count of value:", &_pt._v_count, 3, MAX_VALUE_COUNT, "%.0f");
			ImGui::SliderFloatN("values:", _values, _pt._v_count, -1, 1, "%.3f", 1.0f);
		});
#endif
	}
	void ft_plot_lines::draw()
	{
		ft_base::draw();
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		//ImVec2 winpos = ImGui::GetWindowPos();
		ImGui::SetCursorPosX(abpos.x);
		ImGui::SetCursorPosY(abpos.y);
		ImGui::PlotLines("Lines", _values, _pt._v_count, 0, "avg 0.0", -1.0f, 1.0f, ImVec2(_pt._sizew, _pt._sizeh));

	}
}
