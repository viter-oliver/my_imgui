#pragma once
#include "control_common_def.h"
#include <chrono>
using namespace chrono;
struct prop_ele_position
{
	base_ui_component* _pobj;
	uint16_t _page_index;
	uint16_t _field_index;

	bool operator <(const prop_ele_position& tpp) const
	{
		if (_pobj != tpp._pobj)
		{
			return _pobj < tpp._pobj;
		}
		else
		{
			if (_page_index != tpp._page_index)
			{
				return _page_index < tpp._page_index;
			}
			else
			{
				return _field_index < tpp._field_index;
			}
		}
	}
	bool operator ==(const prop_ele_position& tpp) const
	{
		bool be_equal = _pobj == tpp._pobj&&_page_index == tpp._page_index&&_field_index == tpp._field_index;
		return be_equal;
	}
};
using vprop_pos = vector<prop_ele_position>;
struct prop_ele_bind_unit
{
	vprop_pos _param_list;
	//vector<prop_ele_position> _ref_list;
	string _expression;
	/*prop_ele_bind_unit()
	{
	_expression.resize(1024 * 16);
	}*/
};
struct base_prp_type
{
     string _type;
     void* _pbase;
     int _size;
     vprop_pos _param_list;
     base_prp_type( string bty );
     ~base_prp_type()
     {
          free( _pbase );
     }
     void override_param_list();

};
struct lazy_value
{
     string _value;
     steady_clock::time_point  _start;
     int _during;//milliseconds
};
using ps_bs_prp = shared_ptr<base_prp_type>;
using bs_prp_dic = map<string, ps_bs_prp>;
using bind_dic = map<prop_ele_position, shared_ptr<prop_ele_bind_unit>>;
using bind_ref_dic = map<prop_ele_position, shared_ptr<vprop_pos>>;
using prop_ele_pos_index = vector<unsigned short>;
using prop_ele_value_dic = map<string, lazy_value>;

extern bind_dic g_bind_dic;
extern bind_ref_dic g_bind_ref_dic;
extern void calcu_bind_node(prop_ele_position& pep);
extern void calcu_prop_ele_pos_index(const prop_ele_position& pep, prop_ele_pos_index& pep_id); 
using ps_prp_ele_pos = shared_ptr<prop_ele_position>;
using aliase_map = map<string, ps_prp_ele_pos>;
extern aliase_map g_aliase_dic;
extern bool AFG_EXPORT set_property_aliase_value(string prp_aliase_name, void* pvalue);
extern bool AFG_EXPORT set_property_txt_aliase_value( string prp_aliase_name, int sub_id,int group_id=0);
extern prop_ele_value_dic g_lazy_value_buff;
extern bool AFG_EXPORT set_property_aliase_lazy_value( string prp_aliase_name, int during, void* pvalue );
extern void AFG_EXPORT execute_lazy_value();
extern AFG_EXPORT base_ui_component*  get_aliase_ui_control(string prp_aliase_name);

extern bs_prp_dic g_base_prp_dic;
extern bool AFG_EXPORT prp_is_catched_by_base_prp_type( prop_ele_position& prp_pos, base_prp_type& base_prp );
extern bool AFG_EXPORT cover_common_value( string name_of_common_value );
template<class T> bool set_property_aliase_value_T(string prp_aliase_name, T pvalue )
{
     return set_property_aliase_value( prp_aliase_name, &pvalue );
}
template<> inline bool set_property_aliase_value_T( string prp_aliase_name, string pvalue )
{
     return set_property_aliase_value( prp_aliase_name, (void*)pvalue.c_str() );
}
template<class T> bool get_prop_fd_value( prop_ele_position& pep, T& pvalue )
{
     auto& pgidx = pep._page_index;
     auto& fdidx = pep._field_index;
     auto& field = pep._pobj->get_filed_ele( pgidx, fdidx );
     if (field._tpsz!=sizeof(T))
     {
          printf("The variable you input which type is invalid for(%s_%d_%d)!\n",pep._pobj->get_name().c_str(),pgidx,fdidx);
          return false;
     }
     memcpy( &pvalue, field._address, field._tpsz );
     return true;
}

enum get_ui_control_result
{
     ui_rt_invalid_reciver,
     ui_rt_invalid_alias,
     ui_rt_unmatched_type,
     ui_rt_get_ui_control,
};
template<class T> get_ui_control_result get_ui_control_by_alias( string prp_aliase_name, T** ppUi_control )
{
     if( *ppUi_control != 0 );
     {
          return ui_rt_invalid_reciver;
     }
     auto icontrol = get_aliase_ui_control( prp_aliase_name );
     if (icontrol==nullptr)
     {
          return ui_rt_invalid_alias;
     }
     const char* phost_name = typeid( T ).name();
     const char* ptar_name = typeid( *icontrol ).name();
     if (strcmp(phost_name,ptar_name)!=0)
     {
          return ui_rt_unmatched_type;
     }
     *ppUi_control = static_cast<T*>( icontrol );
     return ui_rt_get_ui_control;
}
