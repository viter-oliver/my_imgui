/*
* uart.h
*
*  Created on: 2016-4-14
*      Author: viter
*/

#ifndef navi_info_h_
#define navi_info_h_
enum 
{
	en_navi_no_direction,
	en_navi_direct_left,
	en_navi_left_moving,
	en_navi_left_turn,
	en_navi_direct_right,
	en_navi_right_moving,
	en_navi_right_turn,
};
extern void register_navi_cmd_handl();
#endif /* UART_H_ */
