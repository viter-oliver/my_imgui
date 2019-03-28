#include "ft_circle.h"
//#include "basic3d.h"


namespace auto_future
{
	ft_circle::ft_circle()
	{
		//reg_property_handle(&_pt, 2, [this](void* memb_adress)
		//{
		//	ImGui::ColorEdit4("block color:", (float*)memb_adress);
		//});
	}
	void ft_circle::draw()
	{
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec4 bkcl(_pt._bkclr.x, _pt._bkclr.y, _pt._bkclr.z, _pt._bkclr.w);
		ImU32 col = ImGui::ColorConvertFloat4ToU32(bkcl);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		int numb_segs = _pt._numb_segs;
		if (numb_segs<20)
		{
			numb_segs = 20;
		}
		if (_pt._solid)
		{
			draw_list->AddCircleFilled(pos0, _pt._radius, col, numb_segs);
		}
		else
		{
			draw_list->AddCircle(pos0, _pt._radius, col, numb_segs, _pt._thickness_utn);
		}
		ft_base::draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		if (is_selected())
		{
			ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
			ImVec2 editunit(edit_unit_len, edit_unit_len);
			ImVec2 pos1a = pos0 - editunit;
			ImVec2 pos1b = pos0 + editunit;
			ImGui::RenderFrame(pos1a, pos1b, col);
		}
#endif
	}
	bool ft_circle::contains(float posx, float posy)
	{
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		float xoff = posx - pos0.x;
		float yoff = posy - pos0.y;
		float ds_sqrt = xoff*xoff + yoff*yoff;
		float rs_sqrt = _pt._radius*_pt._radius;
		return ds_sqrt <= rs_sqrt;
	}
	bool ft_circle::relative_contain(af_vec2& point)
	{
		float ds_sqrt = point.x*point.x + point.y*point.y;
		float rs_sqrt = _pt._radius*_pt._radius;
		return ds_sqrt <= rs_sqrt;
	}
	bool ft_circle::relative_contain(float pos, bool be_h)
	{
		float rd = _pt._radius;
		return pos >= -rd&& pos <= rd;
	}
}