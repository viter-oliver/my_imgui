#include "ft_base.h"
#include <typeinfo>
namespace auto_future
{
	void ft_base::draw()
	{
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
	void ft_base::draw_peroperty_page()
	{
		ImGui::InputText("object name", _in_p._name, name_len);
		ImGui::Text("base pos:");
		//ImGui::InputFloat("x", &_pos.x, 1.0f, base_ui_component::screenw);
		//ImGui::SameLine();
		ImGui::SliderFloat("x", &_in_p._pos.x, -base_ui_component::screenw, base_ui_component::screenw);
		ImGui::SliderFloat("y", &_in_p._pos.y, -base_ui_component::screenh, base_ui_component::screenh);
		ImGui::Checkbox("visibility", &_in_p._visible);
	}
	/*
	fields: name,type,childs
	*/
	bool ft_base::init_from_json(Value& jvalue)
	{
		strcpy(_in_p._name, jvalue["name"].asCString());
		Value& jscreen_pos = jvalue["screen_pos"];
		_in_p._pos.x = jscreen_pos["x"].asDouble();
		_in_p._pos.y = jscreen_pos["y"].asDouble();
		Value& childs = jvalue["childs"];
		if (childs.isNull())
		{
			return true;
		}
		int isize = childs.size();
		for (int ix = 0; ix < isize; ++ix)
		{
			Value& child = childs[ix];
			auto& cname = child["type"].asString();
			base_ui_component* pcontrol_instance = factory::get().produce(cname);
			add_child(pcontrol_instance);
			pcontrol_instance->init_from_json(child);
		}
		return true;
	}

	bool ft_base::init_json_unit(Value& junit)
	{
		junit["name"] = _in_p._name;
		string cname = typeid(*this).name();
		cname = cname.substr(sizeof("class autofuture::"));
		junit["type"] = cname;
		Value jscreen_pos(objectValue);
		jscreen_pos["x"] = _in_p._pos.x;
		jscreen_pos["y"] = _in_p._pos.y;
		junit["screen_pos"] = jscreen_pos;
		Value jchilds(arrayValue);
		size_t chcnt = child_count();
		for (size_t ix = 0; ix < chcnt; ix++)
		{
			base_ui_component* pch_uc = get_child(ix);
			Value jchuc(objectValue);
			pch_uc->init_json_unit(jchuc);
			jchilds.append(jchuc);
		}
		if (chcnt > 0)
		{
			junit["childs"] = jchilds;
		}
		return true;
	}
#endif

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
	float base_ui_component::screenw = 0.0f;
	float base_ui_component::screenh = 0.0f;

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