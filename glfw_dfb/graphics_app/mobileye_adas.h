/*
* uart.h
*
*  Created on: 2016-4-14
*      Author: viter
*/

#ifndef mobileye_adas_h_
#define mobileye_adas_h_
enum en_lane_state
{
	en_lane_normal,
	en_lane_ldw_left,
	en_lane_ldw_right,
	en_lane_off,
	en_lane_num,
};
extern void init_lane_params();
extern void calcu_left_right_lanes();
extern void register_adas_cmd_handl();
extern void adas_cmd_update();
#endif /* UART_H_ */
