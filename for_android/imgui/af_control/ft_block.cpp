#include "ft_block.h"


namespace auto_future
{
	ft_block::ft_block()
	{
		//reg_property_handle(&_pt, 2, [this](void* memb_adress)
		//{
		//	ImGui::ColorEdit4("block color:", (float*)memb_adress);
		//});
		_pt._bkclr = { 1., 1., 1., 1. };
		_pt._bdclr = { 1., 1., 1., 1. };

	}
	void ft_block::draw()
	{
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos1(pos0.x + _in_p._sizew, pos0.y + _in_p._sizeh);
		ImVec4 bkcl(_pt._bkclr.x, _pt._bkclr.y, _pt._bkclr.z, _pt._bkclr.w);
		ImU32 col = ImGui::ColorConvertFloat4ToU32(bkcl);
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
#if !defined(DISABLE_DEMO)
		if (is_selected())
		{
			ImVec4 sel_col;
			ImVec4 wt_col(1., 1., 1., 1.);
			if (_pt._with_border)
			{
				ImVec4 bdcl(_pt._bdclr.x, _pt._bdclr.y, _pt._bdclr.z, _pt._bdclr.w);
				sel_col = wt_col - bdcl;
			}
			else
			{
				sel_col = wt_col - bkcl;
			}
			sel_col.w = 1.0;
			col = ImGui::ColorConvertFloat4ToU32(sel_col);
			ImVec2 editUnit(edit_unit_len, edit_unit_len);
			ImVec2 pos1a = pos0 - editUnit;
			ImVec2 pos1b = pos0 + editUnit;
			ImGui::RenderFrame(pos1a, pos1b, col);
			ImVec2 pos2(pos0.x + _in_p._sizew, pos0.y);
			ImVec2 pos2a = pos2 - editUnit;
			ImVec2 pos2b = pos2 + editUnit;
			ImGui::RenderFrame(pos2a, pos2b, col);
			ImVec2 pos3a = pos1 - editUnit;
			ImVec2 pos3b = pos1 + editUnit;
			ImGui::RenderFrame(pos3a, pos3b, col);
			ImVec2 pos4(pos0.x, pos0.y + _in_p._sizeh);
			ImVec2 pos4a = pos4 - editUnit;
			ImVec2 pos4b = pos4 + editUnit;
			ImGui::RenderFrame(pos4a, pos4b, col);
		}
#endif
		//ft_base::draw();
	}

}