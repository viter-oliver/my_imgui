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
		ft_base::draw();
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos1(pos0.x + _pt._sizew, pos0.y + _pt._sizew);
		ImU32 col = ImGui::ColorConvertFloat4ToU32(_pt._bkclr);
		ImGui::RenderFrame(pos0, pos1, col);

	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

	bool ft_block::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		Value& bsize = jvalue["size"];
		_pt._sizew = bsize["x"].asDouble();
		_pt._sizeh = bsize["y"].asDouble();
		Value& block_color = jvalue["block_color"];
		_pt._bkclr.x = block_color["x"].asDouble();
		_pt._bkclr.y = block_color["y"].asDouble();
		_pt._bkclr.z = block_color["z"].asDouble();
		_pt._bkclr.w = block_color["w"].asDouble();
		return true;
	}

	bool ft_block::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		Value bsize(objectValue);
		bsize["x"] = _pt._sizew;
		bsize["y"] = _pt._sizeh;
		junit["size"] = bsize;
		Value block_color(objectValue);
		block_color["x"] = _pt._bkclr.x;
		block_color["y"] = _pt._bkclr.y;
		block_color["z"] = _pt._bkclr.z;
		block_color["w"] = _pt._bkclr.w;
		junit["block_color"] = block_color;
		return true;
	}
#endif
}