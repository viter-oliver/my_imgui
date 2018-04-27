#pragma once
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <vector>
#include <string>
#include "json/json.h"
#include <algorithm>
//#include <map>
struct ft_vertex
{

	ImVec3  pos;
	ImVec2  uv;
	ImU32   col;
};

struct property_range
{
	void* _p_head_address;
	int _len;
	property_range(void* phead, int len) :_p_head_address(phead), _len(len){}
};
using namespace std;
typedef vector<property_range> vproperty_list;

using namespace Json;
class base_ui_component;
#if !defined(IMGUI_WAYLAND)
const unsigned char name_len = 20;
const float edit_unit_len = 5.0f;
const float imge_edit_view_width = 300.f;
#endif
extern base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname);
class base_ui_component
{
	friend base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname);
protected:
	struct internal_property
	{
		char _name[name_len];
		ImVec3 _pos;
		bool _visible;
		internal_property() :_visible(true){ memset(_name, 0, name_len); }
    };
	internal_property _in_p;
	vector<base_ui_component*> _vchilds;
	base_ui_component* _parent;
#if !defined(IMGUI_WAYLAND)
protected:
	bool _selected;
public:
	virtual void draw_peroperty_page() = 0;
	bool is_selected()
	{
		return _selected;
	}
	void set_selected(bool beselected)
	{
		_selected = beselected;
	}
	virtual bool init_from_json(Value&){ return true; }
	virtual bool init_json_unit(Value&){ return true; }

#endif
public:
	static float screenw;
	static float screenh;
	virtual void draw() = 0;
	virtual void collect_property_range(vproperty_list& vplist)
	{
		//vplist.push_back(property_range(&_in_p, sizeof(internal_property)));
		vplist.emplace_back(&_in_p, sizeof(internal_property));
	}
	//virtual base_ui_component* get_new_instance() = 0;
	void set_name(string& name)
	{
		strcpy(_in_p._name,name.c_str());

	}
	string get_name(){ return string(_in_p._name); }
	virtual bool handle_mouse(){ return false; }
	base_ui_component()
		:_in_p()
		, _parent(NULL)
#if !defined(IMGUI_WAYLAND)
		, _selected(false)
#endif
	{

	}
	
	~base_ui_component()
	{
		for (auto& it:_vchilds)
		{
			delete it;
		}
	}
	void add_child(base_ui_component* pchild){ pchild->_parent = this; _vchilds.push_back(pchild); }
	void remove_child(base_ui_component* pchild)
	{
		auto it = find(_vchilds.begin(), _vchilds.end(), pchild);
		if (it!=_vchilds.end())
		{
			_vchilds.erase(it);
			delete pchild;
		}
	}
	base_ui_component* get_child(int index)
	{
		return _vchilds[index];
	}
	
	size_t get_child_count(){ return _vchilds.size(); }
	base_ui_component* get_parent(){ return _parent; }
	size_t child_count(){ return _vchilds.size(); }
	ImVec3 base_pos(){ return _in_p._pos; }
	ImVec3 absolute_coordinate_of_base_pos()
	{
		ImVec3 base_pos = _in_p._pos;
		base_ui_component* parent = get_parent();
		while (parent)
		{
			ImVec3 pbase_pos = parent->base_pos();
			base_pos += pbase_pos;
			parent = parent->get_parent();
		}
		return base_pos;
	}
	void offset(ImVec2& imof)
	{
		_in_p._pos.x += imof.x;
		_in_p._pos.y += imof.y;
	}
	bool is_visible(){ return _in_p._visible; }
	void set_visible(bool visible){ _in_p._visible = visible; }
};
