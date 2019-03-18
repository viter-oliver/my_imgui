#pragma once
#include "af_bind.h"
#include "easing.h"
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
	bool valid()
	{
		return _from != _to;
	}
};
struct state_transition 
{

	int _start_time;//ms{0}
	int _durition;//ms{250}
	int _easing_func;// {EaseLinear};

	//easing_functions _easing_out_func{ EaseLinear };
};
using sp_st_trans = shared_ptr< state_transition>;
using mp_trans = map<trans_key, sp_st_trans>;
//using  vstrans= vector<state_transition>;
enum moving_state
{
	en_state_pause,
	en_state_moving,
	en_state_out
};
struct af_state_manager
{
	vprop_pos _prop_list;
	vvprop_block _prop_value_list;
	state_transition _any_to_any;
	mp_trans _mtrans;
	unsigned char _state_idx{ 0 };
	moving_state _mstate{ en_state_pause };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	bool _sel{ false };
#endif
};
using ps_state_manager = shared_ptr<af_state_manager>;
using mp_state_manager = map<string, ps_state_manager>;
extern mp_state_manager g_mstate_manager;