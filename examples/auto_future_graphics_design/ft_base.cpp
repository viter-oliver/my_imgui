#include "ft_base.h"
#include <GLFW/glfw3.h>
#include <typeinfo>
#include "af_bind.h"
#include "rescontainer_manager.h"
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "command_element_delta.h"
#endif
#include <regex>
extern bool is_editing();
namespace auto_future
{
	ft_base::ft_base()
		: base_ui_component()
	{
		//cout << "size of base_prop:" << sizeof(base_prop) << endl;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_in_p, 2, [this](void*){
			if (ImGui::Checkbox("Keep scale", &_keep_scale)&&_keep_scale)
			{
				if (_in_p._sizeh>0)
				{
					_w2h = _in_p._sizew / _in_p._sizeh;
				}
			}
			if (ImGui::SliderFloat("width", &_in_p._sizew, 1.0, base_ui_component::screenw) && _keep_scale&&_w2h>0.001)
			{
				_in_p._sizeh = _in_p._sizew / _w2h;
			}
		});
		reg_property_handle(&_in_p, 3, [this](void*){
			if (ImGui::SliderFloat("height", &_in_p._sizeh, 1.0, base_ui_component::screenh) && _keep_scale)
			{
				_in_p._sizew = _in_p._sizeh*_w2h;
			}

		});
		reg_property_handle(&_in_p, 5, [this](void*){
			ImGui::Combo("adjacent to parent:", &_in_p._aj_model, "fixed\0horisontal\0vertical\0\0");
		});

#endif
	}
     
     ft_base::~ft_base()
     {
#ifndef AFG_GAME
          removl_base_ui_component_ref( this );
#endif // !AFG_GAME

     }

     void ft_base::draw()
	{
          /**
		auto pbase = get_parent();
		auto& ajm=_in_p._aj_model;
		if (pbase&&_in_p._aj_model!=en_fixed)
		{
			auto bs_ps = base_pos();
			bool be_intersected;
			auto& adj_value = _in_p._interval;
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
		*/
	}
	void ft_base::draw_frames()
	{
		auto pbase = get_parent();
		auto& ajm = _in_p._aj_model;
		if (pbase&&_in_p._aj_model != en_fixed)
		{
			auto bs_ps = base_pos();
			bool be_intersected;
			auto& adj_value = _in_p._interval;
			if (ajm == en_horisontal)
			{
				float hitpos;
				bs_ps.x -= adj_value;
				be_intersected = pbase->get_border_hit_point(bs_ps.x, true, hitpos);
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
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
          if( _parent&&is_editing() && _selected&&ImGui::GetIO().KeyCtrl )
		{
               static cmd_value_block bk_posx_value = { 0, 0, 0, 0 }, bk_posy_value = {0,0,0,0};
               static bool be_dragging = false;
			auto ab_pos = absolute_coordinate_of_base_pos();
			auto offset = ImGui::GetCursorScreenPos();
			auto cur_pos = ab_pos + offset;
			ImGui::SetCursorScreenPos(cur_pos);
			ImGui::InvisibleButton(_in_p._name, ImVec2(_in_p._sizew, _in_p._sizeh));
               
               if( ImGui::IsItemActive() && ImGui::IsMouseDragging() )
			{
                    if( !be_dragging )
                    {
                         memcpy( &bk_posx_value[ 0 ], &_in_p._posx, sizeof(float) );
                         memcpy( &bk_posy_value[ 0 ], &_in_p._posy, sizeof( float ) );
                    }
                    be_dragging = true;
                    auto ms_delta = ImGui::GetIO().MouseDelta;
                    _in_p._posx += ms_delta.x;
                    _in_p._posy += ms_delta.y;
			}
               if( ImGui::IsMouseReleased( 0 ) )
               {
                    g_ui_edit_command_mg.create_command( edit_commd<base_ui_component>( this, &(_in_p._posx), &bk_posx_value[ 0 ], bk_posx_value.size() ) );
                    g_ui_edit_command_mg.create_command( edit_commd<base_ui_component>( this, &( _in_p._posy ), &bk_posy_value[ 0 ], bk_posy_value.size() ));
                    be_dragging = false;
               }
		}
#endif
		auto ifsz = _vframe_fun.size();
		if (ifsz > 0)
		{
			for (int ix = 0; ix < ifsz; ix++)
			{
				_vframe_fun[ix](ix);
				draw();
			}
		}
		else
		{
			draw();
		}
		for (auto it : _vchilds)
		{
			if (it->is_visible())
			{
				it->draw_frames();
			}
		}
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
		if (contains(posx,posy))
		{
			return this;
		}
		else
		{
			return nullptr;
		}
	}

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
	bool ft_base::set_prop_fd_value(int pg_id, int fd_id, void* pvalue)
	{
		assert(pvalue);
		auto pg_sz = _vprop_eles.size();
		if (pg_id>=pg_sz)
		{
			return false;
		}
		auto& pg_ele = _vprop_eles[pg_id];
		auto& vfd_ele = pg_ele->_pro_page;
		auto vfd_ele_sz = vfd_ele.size();
		if (fd_id>=vfd_ele_sz)
		{
			return false;
		}
		auto& fd_ele = *vfd_ele[fd_id];
		char* pdest = fd_ele._address;
		int count = fd_ele._count;
		if (count==0)
		{
			count = 1;
		}
          
          if (count>1&&fd_ele._type=="char")
          {
               strcpy( pdest, (char*)pvalue );
          }
          else
          {
               memcpy( pdest, pvalue, count * fd_ele._tpsz );
          }
#ifndef AFG_GAME
		prop_ele_position cur_prp_ele_pos = { this, pg_id, fd_id};
		calcu_bind_node(cur_prp_ele_pos);
#endif
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
          prtn->link();
		auto icnt = this->get_child_count();
		for (int ii = 0; ii < icnt; ii++)
		{
			auto pchild = this->get_child(ii);
			auto pchd_cpy = pchild->get_copy_of_object();
			prtn->add_child(pchd_cpy);
		}
		return prtn;
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname)
	{
          if( regex_search( tar_ui.get_name(), regex( uname ) ) )//tar_ui.get_name() == uname)
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
     void find_by_un_from_the_node( base_ui_component& node, const char* uname, vector<base_ui_component*>& resut_list )
     {
          if( regex_search( node.get_name(), regex( uname ) ) )
          {
               resut_list.emplace_back( &node );
          }
          for (auto& it:node._vchilds)
          {
               find_by_un_from_the_node( *it, uname, resut_list );
          }
     }
#endif
	float base_ui_component::screenw = 1920.f;
	float base_ui_component::screenh =720.f;
	prop_ele base_ui_component::null_prop_ele = { nullptr, 0 };
	field_ele base_ui_component::null_field_ele = {"", "",0,0 };
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
		prtn->link();
		auto icnt = byobject->get_child_count();
		for (int ii = 0; ii < icnt; ii++)
		{
			auto pchild = byobject->get_child(ii);
			auto pchd_cpy = get_copy_of_object(pchild);
			prtn->add_child(pchd_cpy);
		}
		return prtn;
	}
	void get_uic_path(base_ui_component* pobj, string& path_rtn)
	{
		assert(pobj);
		vector<string> name_list;
		base_ui_component* ppt = nullptr;
		base_ui_component* pcur = pobj;
		name_list.push_back(pcur->get_name());
		while (ppt = pcur->get_parent())
		{
			name_list.push_back(ppt->get_name());
			pcur = ppt;
		}
		auto name_list_sz = name_list.size();
		for (int ix = name_list_sz - 1; ix >= 0; ix--)
		{
			path_rtn += '/';
			path_rtn += name_list[ix];
		}
	}
}