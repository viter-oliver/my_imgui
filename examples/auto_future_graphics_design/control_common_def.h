/**
  @file   control_common_def.h
  @brief  the head file provide basic definition of control class, all control class inherited from the class base_ui_component
  @author  Viter 
  @version V1.0.0
  @date    6/13/2018 
  @modifier                          
  @reason

  ========================================================================

  description:
  
  ========================================================================
**/
#pragma once
#include "platform_def.h "
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <vector>
#include <string>
#include "json.h"
#include <algorithm>
#include <map>
#include <functional>
//#include "command_element.h"
//#include <map>


#include "property_utilities.h"

namespace auto_future
{
	struct af_vec2
	{
		float x, y;
		af_vec2() = default;
	};
	struct af_vec3
	{
		float x, y, z;
		af_vec3() = default;
	};
	struct af_vec4
	{
		float x, y, z, w;
		af_vec4() = default;
		/*ImVec4& operator=(af_vec4& av4)
		{
			return std::move(ImVec4(av4.x, av4.y, av4.z, av4.w));
		}*/
	};

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

	class base_ui_component;
	const unsigned char name_len = 40;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	using namespace Json;
	const float edit_unit_len = 5.0f;
	const float imge_edit_view_width = 300.f;
#endif
	extern void ShowHelpMarker(const char* desc);
	extern base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname);
	/**
	* @brief  base_ui_component is the base class of control class \n
	* description: base_ui_component is designed as a recursive composition
	*            design pattern, including all the basic features of graphics element,
	such as drawing, parenting, childing and identity,
	*/
	class AFG_EXPORT base_ui_component
	{
		friend base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname);
	protected:

		vp_prop_ele _vprop_eles;
		/**
		* @brief define the property data block\n
		*/
		
		DEF_STRUCT_WITH_INIT(base_prop, _in_p,
			(float, _posx, {0.f}),
			(float, _posy, {0.f}),
			(bool, _visible, {true}),
			(char, _name[name_len]))
			
		/** the member contain all of the components which is the object
		of the child class of base_ui_componnet */
		vector<base_ui_component*> _vchilds;
		/** the parent object, this member will NULL if current object is root */
		base_ui_component* _parent;
		//bool _be_window = { false };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

	protected:
		/** used for selecting a object in project edit for property editing */
		bool _selected = { false };
		typedef std::function<void(void*)> property_handle;
		map<string, property_handle> _mcustom_type_property_handles_container;
		map<void*, property_handle> _mcustom_var_property_handles_container;
		struct st_member_key 
		{
			void *_address;
			int _id;
			st_member_key(void* address, int id)
				:_address(address)
				, _id(id)
			{}
			bool operator <(const st_member_key& skey) const
			{
				return ((char*)_address + _id) < ((char*)skey._address + skey._id);
			}
		};
		map<st_member_key, property_handle> _mcustom_member_property_handles_container;
		map<st_member_key, value_range> _mcustom_member_value_ranges_container;
		void reg_property_handle(string tpname, property_handle ph)
		{
			_mcustom_type_property_handles_container[tpname] = ph;
		}
		void reg_property_handle(void* var_address, property_handle ph)
		{
			_mcustom_var_property_handles_container[var_address] = ph;
		}
		void reg_property_handle(void* var_address, int member_id, property_handle ph)
		{
			_mcustom_member_property_handles_container[st_member_key(var_address, member_id)] = ph;
		}
		void reg_value_range(void* var_address, int member_id, int imin, int imax)
		{
			_mcustom_member_value_ranges_container[st_member_key(var_address, member_id)] = value_range(imin, imax);
		}
		void reg_value_range(void* var_address, int member_id, float fmin, float fmax)
		{
			_mcustom_member_value_ranges_container[st_member_key(var_address, member_id)] = value_range(fmin, fmax);
		}
		void reg_value_range(void* var_address, int member_id, double dmin, double dmax)
		{
			_mcustom_member_value_ranges_container[st_member_key(var_address, member_id)] = value_range(dmin, dmax);
		}

	public:
		/**
		*@brief draw property on the property page for editing
		*/
		//virtual void draw_peroperty_page(int property_part = -1) = 0;
		void draw_peropertys();
		//struct command_elemment;
		bool is_selected()
		{
			return _selected;
		}
		void set_selected(bool beselected)
		{
			_selected = beselected;
		}
		vp_prop_ele& get_prop_ele(){
			return _vprop_eles;
		}
		//virtual ImVec2 range_radius_vector(ImVec2 direction)
		//{
		//	return ImVec2();
		//}
		virtual base_ui_component* get_hit_ui_object(float posx, float posy) = 0;

		/**
		*@brief init some data member from a json value unit
		*@param jvalue a json value
		*  jvalue which contain all the value of the data member of current object
		*@return result
		*  --true success
		*  --false failure
		*/
			void init_property_from_json(Value& jvalue);
	    /**
		*@brief save some data members to a json unit
		*@param junit a json value
		*  junit which will be stored all the value of the data member of current object
		*@return result
		*  -true success
		*  -false failure
		*/
		void save_property_to_json(Value& junit);

#endif
	public:
		/** define the width of screen */
		static float screenw;
		/** define the height of screen */
		static float screenh;
		/**
		*@brief draw self on a surface
		*/
		virtual void draw() = 0;
		/**
		*@brief get the memory range of the property data block
		*@param vplist[out] a container which the range of property data block will store into
		*@return length of property data block
		*  -
		*/
		int collect_property_range(vproperty_list& vplist)
		{
			int len = 0;
			for (auto& prop_ele : _vprop_eles)
			{
				len += prop_ele->_pro_sz;
				vplist.emplace_back(prop_ele->_pro_address, prop_ele->_pro_sz);
			}
			return len;
		}
		/**
		*@brief get hit ui object
		*@param posx posy- the coordinate of mouse 
		*@return object hit by mouse
		*  -
		*/
		virtual ImVec2 get_size()
		{
			return ImVec2();
		}
		virtual void set_size(float w, float h)
		{

		}
		virtual void link(){}
		/**
		*@brief get a copy of a ui component object
		*/
		virtual base_ui_component* get_copy_of_object()
		{
			return NULL;
		}

		//virtual base_ui_component* get_new_instance() = 0;
		void set_name(string name)
		{
			if (name.length()>name_len)
			{
				name.erase(name.begin() + name_len, name.end());
			}
			strcpy(_in_p._name, name.c_str());

		}
		string get_name(){ return string(_in_p._name); }
		virtual bool handle_mouse(){ return false; }
		base_ui_component()
			: _parent(NULL)
		{

		}

		virtual ~base_ui_component()
		{
			for (auto& it : _vchilds)
			{
				delete it;
			}
		}
		virtual void add_child(base_ui_component* pchild)
		{
			pchild->_parent = this; 
			_vchilds.push_back(pchild); 
		}
		virtual void remove_child(base_ui_component* pchild)
		{
			auto it = find(_vchilds.begin(), _vchilds.end(), pchild);
			if (it != _vchilds.end())
			{
				_vchilds.erase(it);
				delete pchild;
			}
		}
		bool move_pre(base_ui_component* pchild)
		{
			auto it = find(_vchilds.begin(), _vchilds.end(), pchild);
			if (it == _vchilds.end())
			{
				return false;
			}
			if (it==_vchilds.begin())
			{
				return false;
			}
			auto itp = it - 1;
			swap(*it, *itp);
			return true;
		}
		bool move_next(base_ui_component* pchild)
		{
			auto it = find(_vchilds.begin(), _vchilds.end(), pchild);
			if (it == _vchilds.end())
			{
				return false;
			}
			auto ilast = _vchilds.end() - 1;
			if (it==ilast)
			{
				return false;
			}
			auto inext = it + 1;
			swap(*it, *inext);
			return true;
		}
		base_ui_component* get_child(int index)
		{
			return _vchilds[index];
		}
		string try_create_a_child_name(string attempt_name,base_ui_component* pcur=NULL)
		{
			for (auto& ichd:_vchilds)
			{
				if (ichd!=pcur&& ichd->get_name()==attempt_name)
				{
					attempt_name += '_';
				}
			}
			return attempt_name;
		}

		size_t get_child_count(){ return _vchilds.size(); }
		base_ui_component* get_parent(){ return _parent; }
		size_t child_count(){ return _vchilds.size(); }
		/**
		*@brief get relative coordinates of current object
		*/
		ImVec2 base_pos(){ return ImVec2(_in_p._posx, _in_p._posy); }
		void set_base_pos(float posx, float posy)
		{
			_in_p._posx = posx; _in_p._posy = posy;
		}
		void set_base_posx(float posx)
		{
			_in_p._posx = posx;
		}
		void set_base_posy(float posy)
		{
			_in_p._posy = posy;
		}

		/**
		*@brief get absolute coordinates of current object
		*/
		ImVec2 absolute_coordinate_of_base_pos()
		{
			ImVec2 base_pos(_in_p._posx, _in_p._posy);
			base_ui_component* parent = get_parent();
			while (parent/*&&!parent->_be_window*/)
			{
				ImVec2 pbase_pos = parent->base_pos();
				base_pos += pbase_pos;
				parent = parent->get_parent();
			}
			return base_pos;
		}
		void offset(ImVec2& imof)
		{
			set_base_pos(base_pos().x + imof.x, base_pos().y + imof.y);
		}
		bool is_visible(){ return _in_p._visible; }
		void set_visible(bool visible){ _in_p._visible = visible; }
	};
}
