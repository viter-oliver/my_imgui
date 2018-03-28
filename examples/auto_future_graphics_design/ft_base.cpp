#include "ft_base.h"
#include <typeinfo>
void ft_base::draw()
{
	if (!_visible)
	{
		return;
	}
	for (auto it :_vchilds)
	{
		if (it->is_visible())
		{
			it->draw();
		}
	}
}
#if !defined(IMGUI_WAYLAND)
void ft_base::draw_peroperty_page()
{

}
#endif
/*
fields: name,type,childs
*/
bool ft_base::init_from_json(Value& jvalue)
{
	_name=jvalue["name"].asString();
	Value& jscreen_pos = jvalue["screen_pos"];
	_pos.x = jscreen_pos["x"].asDouble();
	_pos.y = jscreen_pos["y"].asDouble();
	_pos.z = jscreen_pos["z"].asDouble();
	Value& childs = jvalue["childs"];
	if (childs.isNull())
	{
		return true;
	}
	int isize = childs.size();
	for (int ix = 0; ix < isize;++ix)
	{
		Value& child = childs[ix];
		base_ui_component* pcontrol_instance = factory::get().produce(child["type"].asString());
		add_child(pcontrol_instance);
		pcontrol_instance->init_from_json(child);
	}
	return true;
}

bool ft_base::init_json_unit(Value& junit)
{
	junit["name"] = _name;
	string cname = typeid(*this).name();
	cname = cname.substr(sizeof("class"));
	junit["type"] = cname;
	Value jscreen_pos(objectValue);
	jscreen_pos["x"] = _pos.x;
	jscreen_pos["y"] = _pos.y;
	jscreen_pos["z"] = _pos.z;
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
	if (chcnt>0)
	{
		junit["childs"] = jchilds;
	}
	return true;
}

bool ft_base::handle_mouse()
{
	if (!_visible)
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

ft_base::ft_base(ft_base& tar)
{

	_name = tar._name;
	_texture_id_index = tar._texture_id_index;
	for (auto it : tar._vchilds)
	{
		string cname = typeid(*it).name();
		cname = cname.substr(sizeof("class"));
		base_ui_component* pcontrol_instance = factory::get().produce(cname);
		_vchilds.push_back(pcontrol_instance);

	}
}

base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname)
{
	if (tar_ui._name==uname)
	{
		return&tar_ui;
	}
	for (auto it:tar_ui._vchilds)
	{
		base_ui_component* des_ui= find_a_uc_from_uc(*it, uname);
		if (des_ui)
		{
			return des_ui;
		}
	}
	return NULL;
}
float base_ui_component::screenw = 0.0f;
float base_ui_component::screenh = 0.0f;