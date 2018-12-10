#include "ft_base.h"
#include <GLFW/glfw3.h>
#include <typeinfo>
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "command_element.h"
#endif
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
		/*for (auto it = _vchilds.rbegin(); it != _vchilds.rend(); it++)
		{

			if ((*it)->is_visible())
			{
				(*it)->draw();
			}
		}*/
	}
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
		return nullptr;
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	//void ft_base::draw_peroperty_page(int property_part)
	//{
	//	if (ImGui::InputText("object name", _in_p._name, name_len))
	//	{
	//		auto pparent = get_parent();
	//		if (pparent)
	//		{
	//			string nname = pparent->try_create_a_child_name(_in_p._name, this);
	//			set_name(nname);
	//		}
	//	}
	//	ImGui::Text("base pos:");
	//	//ImGui::InputFloat("x", &_pos.x, 1.0f, base_ui_component::screenw);
	//	//ImGui::SameLine();
	//	if (ImGui::SliderFloat("x", &_in_p._posx, -screenw, screenw))
	//	{
	//		printf("current posx:%f\n", _in_p._posx);
	//	}
	//	/*if (ImGui::IsMouseReleased(1))
	//	{
	//		printf("mouse 1 is released!\n");
	//	}*/
	//	ImGui::SliderFloat("y", &_in_p._posy, -screenw, screenh);
	//	if (ImGui::Checkbox("visibility", &_in_p._visible))
	//	{
	//		g_ui_edit_command_mg.create_command\
	//			(edit_commd<base_ui_component>(this, command_elemment(string("ft_base"), en_pt_visible, command_value(_in_p_bk._visible))));
	//		_in_p_bk._visible = _in_p._visible;
	//	}
	//	ImGuiContext& g = *GImGui;
	//	if (ImGui::IsMouseReleased(0)||g.IO.InputContentChanged)
	//	{
	//		bool pt_modified = false;
	//		if (_in_p._posx != _in_p_bk._posx)
	//		{
	//			g_ui_edit_command_mg.create_command\
	//				(edit_commd<base_ui_component>(this, command_elemment(string("ft_base"), en_pt_pos_x, command_value(_in_p_bk._posx))));
	//			_in_p_bk._posx = _in_p._posx;
	//			pt_modified = true;
	//		}
	//		if (_in_p._posy != _in_p_bk._posy)
	//		{
	//			g_ui_edit_command_mg.create_command\
	//				(edit_commd<base_ui_component>(this, command_elemment(string("ft_base"), en_pt_pos_y, command_value(_in_p_bk._posy))));
	//			_in_p_bk._posy = _in_p._posy;
	//			pt_modified = true;
	//		}
	//		if (strcmp(_in_p._name, _in_p_bk._name) != 0)
	//		{
	//			g_ui_edit_command_mg.create_command\
	//				(edit_commd<base_ui_component>(this, command_elemment(string("ft_base"), en_pt_name, command_value(_in_p_bk._name))));
	//			strcpy(_in_p_bk._name, _in_p._name);
	//			_in_p_bk._name[strlen(_in_p._name)] = '\0';
	//			pt_modified = true;

	//			g.IO.InputContentChanged = false;
	//		}
	//		if (pt_modified)
	//		{
	//			g.IO.InputContentChanged = false;
	//		}
	//	} 
	//}
	void ft_base::back_up_property()
	{
		_in_p_bk=_in_p;
		for (auto it : _vchilds)
		{
			it->back_up_property();
		}
	}
	void ft_base::execute_command(command_elemment& ele_cmd)
	{
		if (ele_cmd._cmd_type == "ft_base")
		{
			switch (ele_cmd._cmd_id)
			{
			case en_pt_name:
				set_name(ele_cmd._cmd_value._value._svalue);
				strcpy(_in_p_bk._name, _in_p._name);
				_in_p_bk._name[strlen(_in_p._name)] = '\0';
				break;
			case en_pt_pos_x:
				_in_p._posx = ele_cmd._cmd_value._value._fvalue;
				_in_p_bk._posx = _in_p._posx;
				break;
			case en_pt_pos_y:
				_in_p._posy = ele_cmd._cmd_value._value._fvalue;
				_in_p_bk._posy = _in_p._posy;
				break;
			case en_pt_visible:
				_in_p._visible = ele_cmd._cmd_value._value._bvalue;
				_in_p_bk._visible = _in_p._visible;
				break;
			default:
				break;
			}
		}
	}	
	command_elemment ft_base::clone_cmd_ele(command_elemment&ele_cmd)
	{
		if (ele_cmd._cmd_type=="ft_base")
		{
			switch (ele_cmd._cmd_id)
			{
			case en_pt_name:
				
				return command_elemment(string("ft_base"), en_pt_name, command_value(_in_p._name));
			case en_pt_pos_x:
				return command_elemment(string("ft_base"), en_pt_pos_x, command_value(_in_p._posx));
			case en_pt_pos_y:
				return command_elemment(string("ft_base"), en_pt_pos_y, command_value(_in_p._posy));
				
			case en_pt_visible:
				return command_elemment(string("ft_base"), en_pt_visible, command_value(_in_p._visible));
			default:
				break;
			}
		}
		return command_elemment();

	}
	/*
	fields: name,type,childs
	*/
	bool ft_base::init_from_json(Value& jvalue)
	{
		strcpy(_in_p._name, jvalue["name"].asCString());
		Value& jscreen_pos = jvalue["screen_pos"];
		_in_p._posx = jscreen_pos["x"].asDouble();
		_in_p._posy = jscreen_pos["y"].asDouble();
		_in_p._visible = jvalue["visible"].asBool();
		_in_p_bk = _in_p;
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
		jscreen_pos["x"] = _in_p._posx;
		jscreen_pos["y"] = _in_p._posy;
		junit["screen_pos"] = jscreen_pos;
		junit["visible"] = _in_p._visible;
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