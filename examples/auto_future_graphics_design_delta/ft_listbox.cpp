#include "ft_listbox.h"
namespace auto_future
{
	ft_listbox::ft_listbox()
		:ft_base()
	{
		/*_be_window = true;*/
		reg_property_handle(&_lt_pt, 6, [this](void*){
			float scrmx = scroll_max();
			ImGui::SliderFloat("scroll value", &_lt_pt._scroll_value, 0.f, scrmx);
			set_scroll_value(_lt_pt._scroll_value);
		});
	}
	float ft_listbox::scroll_max()
	{
		size_t icnt = child_count();
		ImVec2 frange;
		for (size_t ix = 0; ix < icnt; ix++)
		{
			auto pchid = get_child(ix);
			ImVec2 isz = pchid->get_size();
			frange += isz;
		}
		ImVec2 psize(_lt_pt._sizew, _lt_pt._sizeh);
		frange -= psize;
		return _lt_pt._vertical ? frange.y : frange.x;
	}
	void ft_listbox::set_scroll_value(float scvalue)
	{
		size_t icnt = child_count();
		ImVec2 init_pos;
		if (_lt_pt._vertical)
		{
			for (size_t ix = 0; ix < icnt; ix++)
			{
				auto pchid = get_child(ix);
				ImVec2 isz = pchid->get_size();
				set_base_posy(init_pos.y - scvalue);
				init_pos.y += isz.y;//next item
			}
		}
		else
		{
			for (size_t ix = 0; ix < icnt; ix++)
			{
				auto pchid = get_child(ix);
				ImVec2 isz = pchid->get_size();
				set_base_posx ( init_pos.x - scvalue);
				init_pos.x += isz.x;//next item
			}

		}
	}
	void ft_listbox::set_logic_scroll_value(float scvalue)
	{
		if (scvalue > _lt_pt._rangey)
		{
			scvalue = _lt_pt._rangey;
		}
		else
			if (scvalue < _lt_pt._rangex)
			{
				scvalue = _lt_pt._rangex;
			}
		float rg_len = _lt_pt._rangey - _lt_pt._rangex;
		float scmx = scroll_max();
		float rscroll_value = scmx*scvalue / rg_len;
		set_scroll_value(rscroll_value);
	}
	void ft_listbox::draw()
	{
		/*ImGui::SetCursorPos(base_pos());
		ImGui::BeginChild("listbox", _lt_pt._size, true);*/
		ImVec2 apos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		apos += winpos;
		ImVec2 szpos = apos + ImVec2(_lt_pt._sizew, _lt_pt._sizeh);
		ImGui::PushClipRect(apos, szpos, true);
		ft_base::draw();
		ImGui::PopClipRect();
		//ImGui::EndChild();
	}

	void ft_listbox::add_child(base_ui_component* pchild)
	{
		size_t icnt = child_count();
		ImVec2 nsz = pchild->get_size();
		if (icnt > 0)
		{
			base_ui_component* plast = get_child(icnt - 1);
			ImVec2 sz = plast->get_size();
			ImVec2 bpos = plast->base_pos();
			if (_lt_pt._vertical)
			{
				set_base_pos(0.f, bpos.y + sz.y);
			}
			else
			{
				set_base_pos(bpos.x + sz.x, bpos.y);
			}
		}
		else
		{
			set_base_pos(0, 0);
		}

		ft_base::add_child(pchild);
	}
	void ft_listbox::remove_child(base_ui_component* pchild)
	{
		ImVec2 chd_sz = pchild->get_size();
		auto it = find(_vchilds.begin(), _vchilds.end(), pchild);
		if (it != _vchilds.end())
		{
			auto itt = it + 1;
			while (itt != _vchilds.end())
			{
				ImVec2& bpos = (*itt)->base_pos();
				if (_lt_pt._vertical)
				{
					bpos.y -= chd_sz.y;
				}
				else
				{
					bpos.x -= chd_sz.x;
				}
				itt++;
			}
			_vchilds.erase(it);
			delete pchild;
		}
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

	bool ft_listbox::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		_lt_pt._vertical = jvalue["vertical"].asBool();
		Value& jsize = jvalue["size"];
		_lt_pt._sizew = jsize["w"].asDouble();
		_lt_pt._sizeh = jsize["h"].asDouble();
		Value& jrange = jvalue["range"];
		_lt_pt._rangex = jrange["min"].asDouble();
		_lt_pt._rangey = jrange["max"].asDouble();
		return true;
	}
	bool ft_listbox::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		junit["vertical"] = _lt_pt._vertical;
		Value jsize(objectValue);
		jsize["w"] = _lt_pt._sizew;
		jsize["h"] = _lt_pt._sizeh;
		junit["size"] = jsize;
		Value jrange(objectValue);
		jrange["min"] = _lt_pt._rangex;
		jrange["max"] = _lt_pt._rangey;
		junit["range"] = jrange;
		return true;
	}
#endif

	bool ft_listbox::handle_mouse()
	{
		return false;
	}
}
