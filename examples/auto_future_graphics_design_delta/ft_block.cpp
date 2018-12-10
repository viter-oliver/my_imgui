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
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

	bool ft_block::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		Value& bsize = jvalue["size"];
		_pt._sizew = bsize["x"].asDouble();
		_pt._sizeh = bsize["y"].asDouble();
		_pt._round_utn = jvalue["round"].asDouble();
		Value& block_color = jvalue["block_color"];
		_pt._bkclr.x = block_color["x"].asDouble();
		_pt._bkclr.y = block_color["y"].asDouble();
		_pt._bkclr.z = block_color["z"].asDouble();
		_pt._bkclr.w = block_color["w"].asDouble();
		_pt._with_border = jvalue["with_border"].asBool();
		_pt._border_width_uhd = jvalue["border_width"].asDouble();
		Value& border_color = jvalue["border_color"];
		if (border_color.isObject())
		{
			_pt._bdclr.x = border_color["x"].asDouble();
			_pt._bdclr.y = border_color["y"].asDouble();
			_pt._bdclr.z = border_color["z"].asDouble();
			_pt._bdclr.w = border_color["w"].asDouble();

		}
		return true;
	}

	bool ft_block::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		Value bsize(objectValue);
		bsize["x"] = _pt._sizew;
		bsize["y"] = _pt._sizeh;
		junit["size"] = bsize;
		junit["round"] = _pt._round_utn;
		Value block_color(objectValue);
		block_color["x"] = _pt._bkclr.x;
		block_color["y"] = _pt._bkclr.y;
		block_color["z"] = _pt._bkclr.z;
		block_color["w"] = _pt._bkclr.w;
		junit["block_color"] = block_color;
		junit["with_border"] = _pt._with_border;
		junit["border_width"] = _pt._border_width_uhd;
		Value border_color(objectValue);
		border_color["x"] = _pt._bdclr.x;
		border_color["y"] = _pt._bdclr.y;
		border_color["z"] = _pt._bdclr.z;
		border_color["w"] = _pt._bdclr.w;
		junit["border_color"] = border_color;
		return true;
	}
#endif
}