#pragma once
#include "af_bind.h"
#include "easing.h"
using prop_value_block = string;
using vprop_block= vector<prop_value_block>;
using vvprop_block = vector<vprop_block>;
//const float cal_step = 0.025;
struct state_transition 
{
	unsigned int _durition{250};//ms
	easing_functions _easing_in_func{EaseLinear};
	easing_functions _easing_out_func{ EaseLinear };
};
using  vstrans= vector<state_transition>;
enum moving_state
{
	en_state_pause,
	en_state_in,
	en_state_out
};
struct af_state_manager
{
	vprop_pos _prop_list;
	vvprop_block _prop_value_list;
	vstrans _vtrans;
	unsigned char _state_idx{ 0 };
	moving_state _mstate{ en_state_pause };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	bool _sel{ false };
#endif
};
using ps_state_manager = shared_ptr<af_state_manager>;
using mp_state_manager = map<string, ps_state_manager>;
extern mp_state_manager g_mstate_manager;