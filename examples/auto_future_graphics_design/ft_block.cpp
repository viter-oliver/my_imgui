#include "ft_block.h"


namespace auto_future
{
	ft_block::ft_block()
	{
		//reg_property_handle(&_pt, 2, [this](void* memb_adress)
		//{
		//	ImGui::ColorEdit4("block color:", (float*)memb_adress);
		//});
	}
	void ft_block::draw()
	{
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos1(pos0.x + _pt._sizew, pos0.y + _pt._sizeh);
		ImU32 col = ImGui::ColorConvertFloat4ToU32(_pt._bkclr);
		if (_pt._with_border)
		{
			ImU32 bd_clr = ImGui::ColorConvertFloat4ToU32(_pt._bdclr);
			ImGui::RenderFrame(pos0, pos1, bd_clr,false,_pt._round_utn);
			pos0.x += _pt._border_width_uhd; pos0.y += _pt._border_width_uhd;
			pos1.x -= _pt._border_width_uhd; pos1.y -= _pt._border_width_uhd;
			ImGui::RenderFrame(pos0, pos1, col, false, _pt._round_utn);
		}
		else
		{
			ImGui::RenderFrame(pos0, pos1, col, false, _pt._round_utn);
		}
		ft_base::draw();
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	base_ui_component* ft_block::get_hit_ui_object(float posx, float posy)
	{
		base_ui_component* hit_opt = ft_base::get_hit_ui_object(posx, posy);
		if (hit_opt)
		{
			return hit_opt;
		}
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos1(pos0.x + _pt._sizew, pos0.y + _pt._sizeh);
		ImRect cover_area(pos0, pos1);
		ImVec2 mouse_pos(posx, posy);
		if (cover_area.Contains(mouse_pos))
		{
			return this;
		}
		else
		{
			return nullptr;
		}
	}

#endif
}