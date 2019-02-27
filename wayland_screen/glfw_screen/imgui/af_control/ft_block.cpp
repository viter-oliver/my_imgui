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
		ImVec2 pos1(pos0.x + _in_p._sizew, pos0.y + _in_p._sizeh);
		ImVec4 bkcl(_pt._bkclr.x, _pt._bkclr.y, _pt._bkclr.z, _pt._bkclr.w);
		ImU32 col = ImGui::ColorConvertFloat4ToU32(bkcl);
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		if (is_selected())
		{
			col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
		}
#endif
		if (_pt._with_border)
		{
			ImVec4 bdcl(_pt._bdclr.x, _pt._bdclr.y, _pt._bdclr.z, _pt._bdclr.w);
			ImU32 bd_clr = ImGui::ColorConvertFloat4ToU32(bdcl);
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

}