#pragma once
#include "afg.h"
enum signal_light_def
{
	en_light_off,
	en_light_on,
	en_light_blinking,
	en_light_reserver,
};
#define DEF_SIGNAL_LIGHT(x) ft_image* _##x;unsigned char _##x##_state={0}
class signal_light_controller
{
	base_ui_component* _root;
	DEF_SIGNAL_LIGHT(left_turn);
	DEF_SIGNAL_LIGHT(right_turn);
	DEF_SIGNAL_LIGHT(rear_fog);
	DEF_SIGNAL_LIGHT(width_beam);
	DEF_SIGNAL_LIGHT(front_fog);
	DEF_SIGNAL_LIGHT(high_beam);
	DEF_SIGNAL_LIGHT(low_beam);
	DEF_SIGNAL_LIGHT(ALS);
	DEF_SIGNAL_LIGHT(high_coolant_temp);
	DEF_SIGNAL_LIGHT(low_fuel);
	DEF_SIGNAL_LIGHT(front_seatbelt1);
	DEF_SIGNAL_LIGHT(front_seatbelt2);
	DEF_SIGNAL_LIGHT(air_bg);
	DEF_SIGNAL_LIGHT(rear_seatbelt1);
	DEF_SIGNAL_LIGHT(rear_seatbelt2);
	DEF_SIGNAL_LIGHT(rear_seatbelt3); 
	DEF_SIGNAL_LIGHT(esp_on_off);
	DEF_SIGNAL_LIGHT(ccs);
	DEF_SIGNAL_LIGHT(eba);
	DEF_SIGNAL_LIGHT(hdc);
	DEF_SIGNAL_LIGHT(ldw);
	DEF_SIGNAL_LIGHT(fcw);
	DEF_SIGNAL_LIGHT(start_stop);
	DEF_SIGNAL_LIGHT(epb);
	DEF_SIGNAL_LIGHT(apb_fault);
	DEF_SIGNAL_LIGHT(auto_hold);
	DEF_SIGNAL_LIGHT(brake_system);
	DEF_SIGNAL_LIGHT(transmission);
	DEF_SIGNAL_LIGHT(emisssion);
	DEF_SIGNAL_LIGHT(low_egnine_oil);
	DEF_SIGNAL_LIGHT(charging);
	DEF_SIGNAL_LIGHT(eps);
	DEF_SIGNAL_LIGHT(pm2_5);
	DEF_SIGNAL_LIGHT(sport_mode);
	DEF_SIGNAL_LIGHT(abs);
public:
	signal_light_controller(base_ui_component* root);
	~signal_light_controller();
	void on_update();
};

