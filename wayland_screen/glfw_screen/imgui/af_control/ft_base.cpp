#include "ft_base.h"
#include <typeinfo>
namespace auto_future
{
	ft_base::ft_base()
		: base_ui_component()
	{
		//cout << "size of base_prop:" << sizeof(base_prop) << endl;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_in_p, 5, [this](void*){
			ImGui::Combo("adjacent to parent:", &_in_p._aj_model, "fixed\0horisontal\0vertical\0\0");
		});
#endif
	}
	void ft_base::draw()
	{

		auto pbase = get_parent();
		auto ajm=_in_p._aj_model;
		if (pbase&&_in_p._aj_model!=en_fixed)
		{
			auto bs_ps = base_pos();
			bool be_intersected;
			auto adj_value = _in_p._adjacent_to_p;
			if (ajm == en_horisontal)
			{
				float hitpos;
				bs_ps.x -= adj_value;
				be_intersected = pbase->get_border_hit_point(bs_ps.x,true, hitpos);
				if (be_intersected)
				{
					hitpos += adj_value;
					set_base_posx(hitpos);
				}
			}
			else
			{
				float hitpos;
				bs_ps.y -= adj_value;
				be_intersected = pbase->get_border_hit_point(bs_ps.y, false, hitpos);
				if (be_intersected)
				{
					hitpos += adj_value;
					set_base_posy(hitpos);
				}
			}

		}

		if (!is_visible())
		{
			return;
		}
		for (auto it : _vchilds)
		{
			if (it->is_visible())
			{
				it->draw();
			}
		}
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)	
	base_ui_component* ft_base::get_hit_ui_object(float posx, float posy)
	{
		for (auto it = _vchilds.rbegin(); it != _vchilds.rend(); it++)
		{
			if ((*it)->is_visible())
			{
				base_ui_component* hit_object = (*it)->get_hit_ui_object(posx, posy);
				if (hit_object)
				{
					return hit_object;
				}
			}

			//hit_object = (*it).get_hit_ui_object(posx, posy);
		}
		if (contains(posx,posy))
		{
			return this;
		}
		else
		{
			return nullptr;
		}
	}
#endif
	bool ft_base::contains(float posx, float posy)
	{
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos1(pos0.x + _in_p._sizew, pos0.y + _in_p._sizeh);
		ImRect cover_area(pos0, pos1);
		ImVec2 mouse_pos(posx, posy);
		bool be_contain= cover_area.Contains(mouse_pos);
		return be_contain;
	}
	bool ft_base::relative_contain(af_vec2& point)
	{
		ImVec2 pos0{ 0.f, 0.f };
		ImVec2 pos1{ _in_p._sizew, _in_p._sizeh };
		ImRect cover_area(pos0, pos1);
		ImVec2 tar{ point.x, point.y };
		bool be_contained = cover_area.Contains(tar);
		return be_contained;
	}
	bool ft_base::relative_contain(float pos, bool be_h)
	{
		bool be_contain = pos >= 0;
		if (be_h)
		{
			be_contain = be_contain && pos <= _in_p._sizew;
		}
		else
		{
			be_contain = be_contain && pos <= _in_p._sizeh;
		}
		return be_contain;
	}
	bool ft_base::handle_mouse()
	{
		if (!is_visible())
		{
			return false;
		}
		for (auto it : _vchilds)
		{
			if (!it->handle_mouse())
				return false;
		}
		return true;
	}
	base_ui_component* ft_base::get_copy_of_object()
	{
		string cname = typeid(*this).name();
		cname = cname.substr(sizeof("class autofuture::"));
		base_ui_component* prtn = factory::get().produce(cname);
		vproperty_list vrtn, vobj;
		prtn->collect_property_range(vrtn);
		this->collect_property_range(vobj);
		property_copy(vrtn, vobj);
		auto icnt = this->get_child_count();
		for (int ii = 0; ii < icnt; ii++)
		{
			auto pchild = this->get_child(ii);
			auto pchd_cpy = pchild->get_copy_of_object();
			prtn->add_child(pchd_cpy);
		}
		return prtn;
	}
	base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname)
	{
		if (tar_ui.get_name() == uname)
		{
			return&tar_ui;
		}
		for (auto it : tar_ui._vchilds)
		{
			base_ui_component* des_ui = find_a_uc_from_uc(*it, uname);
			if (des_ui)
			{
				return des_ui;
			}
		}
		return NULL;
	}
	float base_ui_component::screenw = 1920.f;
	float base_ui_component::screenh =720.f;

	void property_copy(vproperty_list& vdest, vproperty_list& vsource)
	{
		for (size_t i = 0; i < vdest.size(); i++)
		{
			memcpy(vdest[i]._p_head_address, vsource[i]._p_head_address, vdest[i]._len);
		}
	}

	base_ui_component* get_copy_of_object(base_ui_component* byobject)
	{
		string cname = typeid(*byobject).name();
		cname = cname.substr(sizeof("class autofuture::"));
		base_ui_component* prtn = factory::get().produce(cname);
		vproperty_list vrtn, vobj;
		prtn->collect_property_range(vrtn);
		byobject->collect_property_range(vobj);
		property_copy(vrtn, vobj);
		auto icnt = byobject->get_child_count();
		for (int ii = 0; ii < icnt; ii++)
		{
			auto pchild = byobject->get_child(ii);
			auto pchd_cpy = get_copy_of_object(pchild);
			prtn->add_child(pchd_cpy);
		}
		return prtn;
	}
}