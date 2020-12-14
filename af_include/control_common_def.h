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
namespace auto_future{ class base_ui_component; }

#include "property_utilities.h"
#include "af_type.h"
namespace auto_future
{
	struct property_range
	{
		void* _p_head_address;
		int _len;
		property_range(void* phead, int len) :_p_head_address(phead), _len(len){}
	};
	using namespace std;
	typedef vector<property_range> vproperty_list;
     typedef function<void( int )> frame_fun;
     typedef vector<frame_fun> vframe_fun;
	const unsigned char name_len = 40;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	using namespace Json;
	const float edit_unit_len = 5.0f;
	const float imge_edit_view_width = 300.f;
#endif
     typedef function<void( void )> mouse_fun;
     typedef function<void( float, float )>mouse_drag_fun;
	extern void ShowHelpMarker(const char* desc);
	extern base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname);
     extern void find_by_un_from_the_node( base_ui_component& node, const char* uname, vector<base_ui_component*>& resut_list );
	/**
	* @brief  base_ui_component is the base class of control class \n
	* description: base_ui_component is designed as a recursive composition
	*            design pattern, including all the basic features of graphics element,
	such as drawing, parenting, childing and identity,
	*/
	typedef std::function<void(void*)> property_handle;
	typedef map<string, property_handle> mp_tp_propty_handle;
	extern mp_tp_propty_handle _mcustom_type_property_handles_container;
	void reg_property_handle(string tpname, property_handle ph);
	void init_common_type_property_handles();

	class AFG_EXPORT base_ui_component
	{
		friend base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname);
          friend void find_by_un_from_the_node( base_ui_component& node, const char* uname, vector<base_ui_component*>& resut_list );
	protected:
          mouse_fun _mouse_clicked;
          mouse_fun _mouse_down;
          mouse_fun _mouse_release;
          mouse_drag_fun _mouse_drag;
          vframe_fun _vframe_fun;
		vp_prop_ele _vprop_eles;
		enum adjacent_model
		{
			en_fixed,
			en_horisontal,
			en_vertical,
		};
		/**
		* @brief define the property data block\n
		*/
		
		DEF_STRUCT_WITH_INIT(base_prop, _in_p,
			(float, _posx, {0.f}),
			(float, _posy, {0.f}),
			(float, _sizew, { 20.f }),
			(float, _sizeh, { 20.f }),
			(float, _interval, {0.f}),
			(int, _aj_model, { en_fixed }),
			(bool, _visible, {true}),
			(char, _name[name_len]))
			
		/** the member contain all of the components which is the object
		of the child class of base_ui_componnet */
		vector<base_ui_component*> _vchilds;
		/** the parent object, this member will NULL if current object is root */
		base_ui_component* _parent=nullptr;
		//bool _be_window = { false };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

	protected:
		/** used for selecting a object in project edit for property editing */
		bool _selected = { false };
		
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
		virtual void view_components_in_world_space(){}
#endif
	public:
		/** define the width of screen */
		static float screenw;
		/** define the height of screen */
		static float screenh;
		static prop_ele null_prop_ele;
		static field_ele null_field_ele;
		//static field_ele_with_value null_field_ele_with_value;
		prop_ele& get_prop_ele(int pgidx)
		{
			auto vpsz = _vprop_eles.size();
			if (pgidx >= vpsz)
			{
				return null_prop_ele;
			}
			return *_vprop_eles[pgidx];
		}		
		field_ele& get_filed_ele(int pgidx, int field_idx)
		{
			auto& pele = get_prop_ele(pgidx);
			auto vfsz = pele._pro_page.size();
			if (field_idx>=vfsz)
			{
				return null_field_ele;
			}
			auto& vfiled = pele._pro_page;
			return *vfiled[field_idx];
		}

		/**
		*@brief draw self on a surface
		*/
		virtual void draw() = 0;
          int instance_cnt()
          {
               return _vframe_fun.size();
          }
          int reg_frame_fun( int idx, frame_fun ffun )
          {
               auto ifsz = _vframe_fun.size();
               if( idx < ifsz )
               {
                    swap( _vframe_fun[ idx ], ffun );
                    return idx;
               }
               _vframe_fun.emplace_back( ffun );
               return ifsz;
          }
          void reg_frame_fun( vframe_fun&& vffuns )
          {
               _vframe_fun.clear();
               for( auto& ffun : vffuns )
               {
                    _vframe_fun.emplace_back( ffun );
               }
          }
          void clear_frame_fun()
          {
               _vframe_fun.clear();
          }
          virtual void draw_frames() = 0;
          void register_mouse_click( mouse_fun mclick )
          {
               swap( _mouse_clicked, mclick );
          }
          void register_mouse_down( mouse_fun mdown )
          {
               swap( _mouse_down, mdown );
          }
          void register_mouse_release( mouse_fun mrelease )
          {
               swap( _mouse_release, mrelease );
          }
          void register_mouse_draging( mouse_drag_fun mdrag )
          {
               swap( _mouse_drag, mdrag );
          }
          void trigger_click()
          {
               if( _mouse_clicked ) _mouse_clicked;
          }
          void trigger_mouse_down()
          {
               if( _mouse_down ) _mouse_down;
          }
          void trigger_mouse_release()
          {
               if( _mouse_release ) _mouse_release;
          }
          void trigger_mouse_drag( float xof, float yof )
          {
               if( _mouse_drag ) _mouse_drag( xof, yof );
          }
          virtual void mouse_clicked(){}
          virtual void mouse_down(){}
          virtual void mouse_relese(){}
          virtual void mouse_drag(float xoffset, float yoffset){}
          virtual base_ui_component* get_hit_ui_object( float posx, float posy ) = 0;

		virtual bool contains(float posx, float posy) = 0;
		virtual bool relative_contain(float pos, bool be_h) = 0;
		virtual bool set_prop_fd_value(int pg_id, int fd_id, void* pvalue) = 0;
		bool get_border_hit_point(float pos, bool be_h, float& hitpos)
		{
			const unsigned int max_repeat_cnt = 1000;
			bool from_et=false;
			float delta = 0;
			for (unsigned int ix = 0; ix < max_repeat_cnt; ix++)
			{
				pos += delta;
				bool be_hit = relative_contain(pos, be_h);
				if (ix==0)
				{
					from_et = be_hit;
					if (from_et)
					{
						delta = 1.f;
					}
					else
					{
						delta = -1.f;
					}
				}
				if (be_hit!=from_et)
				{

					hitpos = pos;
					if (!from_et)
					{
						hitpos -= delta;
					}
					return true;
				}
			}
			return false;
		}

		virtual bool relative_contain(af_vec2& point) = 0;

		bool get_border_hit_point(af_vec2& base_point, af_vec2& direction, af_vec2& hitpoint)
		{
			const unsigned int max_repeat_cnt = 1000;
			for (unsigned int ix = 0; ix < max_repeat_cnt; ix++)
			{
				base_point += direction;
				static bool from_et;
				bool be_contain = relative_contain(base_point);
				if (ix == 0)
				{
					from_et = be_contain;
				}
				if (from_et != be_contain)
				{
					hitpoint = base_point;
					return true;
				}
			}
			return false;
		}
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
		virtual void get_size(float& w,float& h)
		{
			w = _in_p._sizew;
			h = _in_p._sizeh;
		}
		virtual void set_size(float w, float h)
		{
			_in_p._sizew = w;
			_in_p._sizeh = h;
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
		void clear_rebundent_memory()
		{
			_vprop_eles.shrink_to_fit();
			_vchilds.shrink_to_fit();
		}
		virtual void remove_child(base_ui_component* pchild,bool release=true)
		{
			auto it = find(_vchilds.begin(), _vchilds.end(), pchild);
			if (it != _vchilds.end())
			{
				_vchilds.erase(it);
                    if(release )
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
