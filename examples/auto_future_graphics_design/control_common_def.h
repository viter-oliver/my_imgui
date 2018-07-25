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
#include "platform_def.h"
//#include <map>
namespace auto_future
{
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
	const unsigned char name_len = 20;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	const float edit_unit_len = 5.0f;
	const float imge_edit_view_width = 300.f;
#endif
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
		/**
		* @brief define the property data block\n
		*/
		struct internal_property
		{
			char _name[name_len];
			ImVec2 _pos;
			bool _visible;
			internal_property() :_visible(true){ memset(_name, 0, name_len); }
		};
		/// the member will be serialized 
		internal_property _in_p;
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
	public:
		/**
		*@brief draw property on the property page for editing
		*/
		virtual void draw_peroperty_page() = 0;
		bool is_selected()
		{
			return _selected;
		}
		void set_selected(bool beselected)
		{
			_selected = beselected;
		}
		//virtual ImVec2 range_radius_vector(ImVec2 direction)
		//{
		//	return ImVec2();
		//}

		/**
		*@brief instancing class from a json value unit
		*@param jvalue a json value
		*  jvalue which contain all the value of the data member of current object
		*@return result
		*  --true success
		*  --false failure
		*/
		virtual bool init_from_json(Value& jvalue){ return true; }
		/**
		*@brief init a json unit by some data members
		*@param junit a json value
		*  junit which will be stored all the value of the data member of current object
		*@return result
		*  -true success
		*  -false failure
		*/
		virtual bool init_json_unit(Value& junit){ return true; }
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
		virtual int collect_property_range(vproperty_list& vplist)
		{
			//vplist.push_back(property_range(&_in_p, sizeof(internal_property)));
			int len = sizeof(internal_property);
			vplist.emplace_back(&_in_p, sizeof(internal_property));
			return len;
		}
		virtual ImVec2 get_size()
		{
			return ImVec2();
		}
		/**
		*@brief get a copy of a ui component object
		*/
		virtual base_ui_component* get_copy_of_object()
		{
			return NULL;
		}

		//virtual base_ui_component* get_new_instance() = 0;
		void set_name(string& name)
		{
			strcpy(_in_p._name, name.c_str());

		}
		string get_name(){ return string(_in_p._name); }
		virtual bool handle_mouse(){ return false; }
		base_ui_component()
			:_in_p()
			, _parent(NULL)
		{

		}

		virtual ~base_ui_component()
		{
			for (auto& it : _vchilds)
			{
				delete it;
			}
		}
		virtual void add_child(base_ui_component* pchild){ pchild->_parent = this; _vchilds.push_back(pchild); }
		virtual void remove_child(base_ui_component* pchild)
		{
			auto it = find(_vchilds.begin(), _vchilds.end(), pchild);
			if (it != _vchilds.end())
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
		/**
		*@brief get relative coordinates of current object
		*/
		ImVec2& base_pos(){ return _in_p._pos; }
		/**
		*@brief get absolute coordinates of current object
		*/
		ImVec2 absolute_coordinate_of_base_pos()
		{
			ImVec2 base_pos = _in_p._pos;
			base_ui_component* parent = get_parent();
			while (parent/*&&!parent->_be_window*/)
			{
				ImVec2& pbase_pos = parent->base_pos();
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
}