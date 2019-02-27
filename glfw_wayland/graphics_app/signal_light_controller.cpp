#include "signal_light_controller.h"
#include "msg_host_n.h"
using namespace auto_future_utilities;
extern msg_host_n g_msg_host;


#define GET_IMG_REF(x) _##x=static_cast<ft_image*> (root->get_child(idx));_##x->set_visible(false); idx++


signal_light_controller::signal_light_controller(base_ui_component* root)
	:_root(root)
{
	root->set_visible(true);
	int idx = 0;
	GET_IMG_REF(left_turn);
	GET_IMG_REF(right_turn);
	GET_IMG_REF(rear_fog);
	GET_IMG_REF(width_beam);
	GET_IMG_REF(front_fog);
	GET_IMG_REF(high_beam);
	GET_IMG_REF(low_beam);
	GET_IMG_REF(ALS);
	GET_IMG_REF(high_coolant_temp);
	GET_IMG_REF(low_fuel);
	GET_IMG_REF(front_seatbelt1);
	GET_IMG_REF(front_seatbelt2);
	GET_IMG_REF(air_bg);
	GET_IMG_REF(rear_seatbelt1);
	GET_IMG_REF(rear_seatbelt2);
	GET_IMG_REF(rear_seatbelt3);
	GET_IMG_REF(esp_on_off);
	GET_IMG_REF(ccs);
	GET_IMG_REF(eba);
	GET_IMG_REF(hdc);
	GET_IMG_REF(ldw);
	GET_IMG_REF(fcw);
	GET_IMG_REF(start_stop);
	GET_IMG_REF(epb);
	GET_IMG_REF(apb_fault);
	GET_IMG_REF(auto_hold);
	GET_IMG_REF(brake_system);
	GET_IMG_REF(transmission);
	GET_IMG_REF(emisssion);
	GET_IMG_REF(low_egnine_oil);
	GET_IMG_REF(charging);
	GET_IMG_REF(eps);
	GET_IMG_REF(pm2_5);
	GET_IMG_REF(sport_mode);
	GET_IMG_REF(abs);

#define REGISTER_CMD_HANDLE(tag,ctl) \
	g_msg_host.attach_monitor(#tag, [this](unsigned char* pbuff, int len){\
	_##ctl##_state = *pbuff;\
	_##ctl->set_visible(_##ctl##_state != en_light_off);\
	})

	REGISTER_CMD_HANDLE(left turn, left_turn);
	REGISTER_CMD_HANDLE(right turn, right_turn);
	REGISTER_CMD_HANDLE(rear foglight, rear_fog);
	REGISTER_CMD_HANDLE(width lamp, width_beam);
	REGISTER_CMD_HANDLE(front foglight, front_fog);
	REGISTER_CMD_HANDLE(high beam, high_beam);
	REGISTER_CMD_HANDLE(low beam, low_beam);
/*	REGISTER_CMD_HANDLE(low beam, ALS);
	REGISTER_CMD_HANDLE(high_coolant_temp);
	REGISTER_CMD_HANDLE(low_fuel);
	REGISTER_CMD_HANDLE(front_seatbelt1);
	REGISTER_CMD_HANDLE(front_seatbelt2);
	REGISTER_CMD_HANDLE(air_bg);
	REGISTER_CMD_HANDLE(rear_seatbelt1);
	REGISTER_CMD_HANDLE(rear_seatbelt2);
	REGISTER_CMD_HANDLE(rear_seatbelt3);
	REGISTER_CMD_HANDLE(esp off,esp_on_off);
	REGISTER_CMD_HANDLE(ccs);
	REGISTER_CMD_HANDLE(eba);
	REGISTER_CMD_HANDLE(hdc);
	REGISTER_CMD_HANDLE(ldw);
	REGISTER_CMD_HANDLE(fcw);
	REGISTER_CMD_HANDLE(start_stop);
	REGISTER_CMD_HANDLE(epb,epb);
	REGISTER_CMD_HANDLE(apb_fault);
	REGISTER_CMD_HANDLE(auto_hold);
	REGISTER_CMD_HANDLE(brake_system);
	REGISTER_CMD_HANDLE(transmission);
	REGISTER_CMD_HANDLE(emisssion);
	REGISTER_CMD_HANDLE(low_egnine_oil);
	REGISTER_CMD_HANDLE(charging);
	REGISTER_CMD_HANDLE(eps);
	REGISTER_CMD_HANDLE(pm2_5);
	REGISTER_CMD_HANDLE(sport_mode);
	REGISTER_CMD_HANDLE(abs);*/
	/*g_msg_host.attach_monitor("left turn", [this](unsigned char* pbuff, int len){
		_left_turn_state = *pbuff;
		_left_turn->set_visible(_left_turn_state != en_light_off);
		});*/
	
}


signal_light_controller::~signal_light_controller()
{
}

void signal_light_controller::on_update()
{
#define LIGHT_BLINKING_CTRL(x) if (_##x##_state==en_light_blinking){\
	bool be_visible = _##x->is_visible();\
	_##x->set_visible(!be_visible);\
}
	LIGHT_BLINKING_CTRL(left_turn);
	LIGHT_BLINKING_CTRL(right_turn);
	LIGHT_BLINKING_CTRL(rear_fog);
	LIGHT_BLINKING_CTRL(width_beam);
	LIGHT_BLINKING_CTRL(front_fog);
	LIGHT_BLINKING_CTRL(high_beam);
	LIGHT_BLINKING_CTRL(low_beam);
}
