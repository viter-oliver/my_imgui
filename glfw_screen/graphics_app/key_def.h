#pragma once
enum en_key_state
{
	en_key_value,
	en_key_press,
	en_key_release,
	en_key_long_press,
};
enum en_key_type
{
	en_ktp_none,
	en_ktp_menu,
	en_ktp_ok,
	en_ktp_esc,
	en_ktp_left,
	en_ktp_right,
	en_ktp_up,
	en_ktp_down,
	en_ktp_ivi_disp,
};
enum en_key_source
{
	en_ks_dashboard,
	en_ks_ir,
	en_ks_rotary_knob,
	en_ks_steering_wheel,
	en_ks_ivi,
};