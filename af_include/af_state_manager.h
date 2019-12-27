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
extern mp_state_manager g_mstate_manager;
AFG_EXPORT bool save_trans_value( string trans_name, int sid );
AFG_EXPORT bool reg_trans_handle(string trans_name, trans_finish_handle trans_handle);
AFG_EXPORT bool play_trans(string trans_name, int from, int to);
AFG_EXPORT bool play_tran(string stm_name, int from, int to);
AFG_EXPORT bool play_tran_playlist(string stm_name, int playlist_id);

AFG_EXPORT void keep_state_trans_on();
/*
class state_trans_player
{
	bool _be_playing{ false };
	trans_key _cur_trans_play_key;
	system_clock::time_point _cur_trans_play_start;
	ps_state_manager _psel{ nullptr };
public:
	void play_state_trans(ps_state_manager pstate, int from, int to);
	void keep_state_trans_on();
	ps_state_manager current_ps_stm()
	{
		if (_be_playing)
		{
			return _psel;
		}
		else
		{
			return nullptr;
		}
	}
	bool be_playing(){ return _be_playing; }
};
extern state_trans_player g_state_trans_player;
*/