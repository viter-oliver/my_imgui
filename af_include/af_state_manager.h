#pragma once
#include "af_bind.h"
#include "easing.h"
#include<chrono>
using namespace chrono;
using prop_value_block = string;
using vprop_block= vector<prop_value_block>;
using vvprop_block = vector<vprop_block>;
//const float cal_step = 0.025;
struct trans_key 
{
	int _from, _to;
	bool operator <(const trans_key&ttrans) const
	{
		if (_from!=ttrans._from)
		{
			return _from < ttrans._from;
		}
		else
		{
			return _to < ttrans._to;
		}
	}
     bool operator ==( const trans_key& ttrans ) const
     {
          return _from == ttrans._from&& _to == ttrans._to;
     }
     /*trans_key& operator =( trans_key& ttrans )
     {
          _from = ttrans._from;
          _to = ttrans._to;
          return *this;
     }*/
	bool valid()
	{
		return _from != _to;
	}
};
struct state_transition 
{

	int _start_time;//ms{0}
	int _duration;//ms{250}
	int _easing_func;// {EaseLinear};

	//easing_functions _easing_out_func{ EaseLinear };
};
using sp_st_trans = shared_ptr< state_transition>;
using vtrans_key = vector<trans_key>;
using vvtrans_key = vector<vtrans_key>;
using mp_trans = map<trans_key, sp_st_trans>;
//using  vstrans= vector<state_transition>;
enum moving_state
{
	en_state_pause,
	en_state_moving,
	en_state_out
};
enum trans_play_state
{
     en_play_stop,
     en_play_tran,
     en_play_tran_playlist,
	 en_play_state_cnt
};
typedef	function<void(int from, int to)> trans_finish_handle;
struct af_state_manager
{
	vprop_pos _prop_list;//属性列表——该状态机包含的所有属性
	vvprop_block _prop_value_list;//属性值列表——
	state_transition _any_to_any;
	trans_finish_handle _trans_finish;
     sp_st_trans _pcur_tran { nullptr };
	mp_trans _mtrans;//状态切换
	unsigned char _state_idx{ 0 };
	moving_state _mstate{ en_state_pause };
     trans_play_state _play_state {en_play_stop};
     steady_clock::time_point _trans_start;
	 int _cur_from{ 0 }, _cur_to{ 0 };
	 int _cur_play_trans_id{ 0 }, _cur_playlist_id{0};
	 vvtrans_key _playlist_list;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	bool _sel{ false };
#endif
};
using ps_state_manager = shared_ptr<af_state_manager>;
using mp_state_manager = map<string, ps_state_manager>;
extern AFG_EXPORT mp_state_manager g_mstate_manager;
AFG_EXPORT bool trans_is_playing( string trans_name );
AFG_EXPORT bool save_trans_value( string trans_name, int sid );
AFG_EXPORT bool restore_trans_value( string trans_name, int sid );
AFG_EXPORT bool reg_trans_handle(string trans_name, trans_finish_handle trans_handle);
AFG_EXPORT bool play_tran(string stm_name, int from, int to);
AFG_EXPORT bool play_tran_playlist(string stm_name, int playlist_id);
AFG_EXPORT void keep_state_trans_on();
AFG_EXPORT bool save_property_to_trans_state( string trans_name,
                                              prop_ele_position& prp_pos,
                                              int base_id );
template<class T> struct paire_value
{
     int id;
     T delta_value;
};
template<class T> bool set_trans_pair_state_list_delta( string trans_name,
                                                   prop_ele_position& prp_pos,
                                                   int base_id,//start state index
                                                   T base_value,
                                                   vector<paire_value<T>>& vpvalue )
{
     const auto& itrans = g_mstate_manager.find( trans_name );
     if( itrans == g_mstate_manager.end() )
     {
          printf( "invalid trans name:%s\n", trans_name.c_str() );
          return false;
     }
     int isz = sizeof( T );
     auto& pobj = prp_pos._pobj;
     auto& pgidx = prp_pos._page_index;
     auto& fidx = prp_pos._field_index;
     field_ele& fel = pobj->get_filed_ele( pgidx, fidx );
     if( fel._tpsz != isz )
     {
          printf( "value type size:%d!=original property size:%d\n", isz, fel._tpsz );
          return false;
     }
     auto& trans = *itrans->second;
     auto& prp_list = trans._prop_list;
     int pos_id = 0;
     for( ; pos_id < prp_list.size(); pos_id++ )
     {
          if( prp_list[ pos_id ] == prp_pos )
          {
               break;
          }
     }
     if( pos_id == prp_list.size() )
     {
          printf( "invalid property element position\n" );
          return false;
     }
     auto vpsz = vpvalue.size();
     auto& prp_value_list = trans._prop_value_list;
     auto sz_prp_value_list = prp_value_list.size();
    
     auto& pp_vl_list_target = prp_value_list[ base_id ];
     auto& value_target = pp_vl_list_target[ pos_id ];
     //T tar_value;
     memcpy( &value_target[ 0 ], &base_value, sizeof( T ) );
     for( auto& pu : vpvalue )
     {
          auto& id = pu.id;
          if( id >= sz_prp_value_list )
          {
               printf( "invalid id:%d for delta value set\n", id );
               continue;
          }
          auto& delta_value = pu.delta_value;
          T des_p_value = base_value + delta_value;
          auto& ppv_list_target = prp_value_list[ id ];
          auto& des_value = ppv_list_target[ pos_id ];
          memcpy( &des_value[ 0 ], &des_p_value, sizeof( T ) );
     }

     return true;
}

template<class T> bool set_trans_sequential_state_list_delta(string trans_name,
                                   prop_ele_position& prp_pos,
                                   int target_id,//start state index
                                   T tar_value,
                                   vector<T>& vpvalue)
{
     const auto& itrans = g_mstate_manager.find( trans_name );
     if (itrans==g_mstate_manager.end())
     {
          printf( "invalid trans name:%s\n", trans_name.c_str() );
          return false;
     }
     int isz = sizeof( T );
     auto& pobj = prp_pos._pobj;
     auto& pgidx = prp_pos._page_index;
     auto& fidx = prp_pos._field_index;
     field_ele& fel = pobj->get_filed_ele( pgidx, fidx );
     if (fel._tpsz!=isz)
     {
          printf( "value type size:%d!=original property size:%d\n", isz, fel._tpsz );
          return false;
     }
     auto& trans =*itrans->second;
     auto& prp_list = trans._prop_list;
     int pos_id = 0;
     for( ; pos_id < prp_list.size();pos_id++ )
     {
          if( prp_list[ pos_id ] == prp_pos )
          {
               break;
          }
     }
     if (pos_id==prp_list.size())
     {
          printf( "invalid property element position\n" );
          return false;
     }
     auto vpsz = vpvalue.size();
     auto& prp_value_list = trans._prop_value_list;
     auto sz_prp_value_list = prp_value_list.size();
     auto delta_sz = sz_prp_value_list - target_id - 1;
     if( vpsz != delta_sz )
     {
          printf("invalid vpvalue size:%d for target id:%d\n",vpsz,target_id);
          return false;
     }
     auto& pp_vl_list_target = prp_value_list[ target_id ];
     auto& value_target = pp_vl_list_target[ pos_id ];
     //T tar_value;
     memcpy( &value_target[ 0 ],&tar_value,  sizeof( T ) );
     for( int ix = 0; ix < vpsz;ix++ )
     {
          T pvalue = tar_value + vpvalue[ix];
          auto& des_value= prp_value_list[ target_id + 1 + ix ][ pos_id ];
          memcpy( &des_value[ 0 ], &pvalue, sizeof( T ) );
     }
     return true;
}
