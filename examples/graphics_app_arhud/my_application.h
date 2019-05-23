#pragma once
#include "application.h"
/*
"acc_control" : [ 3, 1, 0, 2 ],
"acc_fault" : [ 6, 0, 10, 2 ],
"acc_speed" : [ 4, 1, 5, 2 ],
"acc_speed_color" : [ 0, 1, 5, 2 ],
"active_navigation" : [ 6, 0, 13, 1 ],
"aeb_control" : [ 3, 1, 1, 2 ],
"aeb_show" : [ 6, 0, 1, 2 ],
"camera1" : [ 6, 0, 1, 3 ],
"camera2" : [ 6, 0, 0, 3 ],
"car_speed_prog" : [ 7, 1, 0, 1 ],
"car_speed_unit" : [ 4, 1, 0, 14, 1 ],
"control_media_icon" : [ 3, 1, 0, 4 ],
"cur_lane" : [ 6, 0, 0, 9, 1 ],
"fatigue_driving" : [ 6, 0, 9, 2 ],
"media_content" : [ 4, 1, 1, 0, 4 ],
"navi_direction" : [ 6, 0, 6, 3 ],
"peds_war1" : [ 6, 0, 16, 1 ],
"peds_war2" : [ 6, 0, 17, 1 ],
"pop_warning_dlg" : [ 6, 0, 6 ],
"prohibit_left_lane_change" : [ 6, 0, 18, 1 ],
"prohibit_right_lane_change" : [ 6, 0, 19, 1 ],
"rotate_speed_prog" : [ 7, 1, 1, 1 ],
"select_left_turn_low_fuel_hot_water" : [ 3, 1, 6, 2 ],
"select_right_turn_doors_open" : [ 3, 1, 7, 2 ],
"show_distance0" : [ 6, 0, 3, 1 ],
"show_distance1" : [ 6, 0, 4, 1 ],
"show_distance2" : [ 6, 0, 5, 1 ],
"show_distance3" : [ 6, 0, 6, 1 ],
"show_distance4" : [ 6, 0, 7, 1 ],
"show_distance5" : [ 6, 0, 8, 1 ],
"show_left_turn_low_fuel_hot_water" : [ 6, 0, 6, 2 ],
"show_limited_speed" : [ 6, 0, 2, 3 ],
"show_media_info" : [ 6, 0, 0, 4 ],
"show_no_seatbelt" : [ 6, 0, 0, 4 ],
"show_north_direction" : [ 6, 0, 4, 3 ],
"show_right_turn_doors_open" : [ 6, 0, 7, 2 ],
"show_third_call" : [ 6, 0, 0, 0, 4 ],
"speed_limited" : [ 4, 1, 0, 2, 3 ],
"value_car_speed" : [ 4, 1, 14, 1 ],
"value_endurance" : [ 4, 1, 11, 1 ],
"value_gear" : [ 4, 1, 15, 1 ],
"value_north_direction" : [ 4, 1, 4, 3 ],
"value_remain_mileage" : [ 4, 1, 13, 1 ],
"value_remain_time" : [ 4, 1, 10, 1 ],
"value_rotate_speed" : [ 4, 1, 12, 1 ],
"warning_content" : [ 4, 1, 0, 0, 6 ]
*/
class my_application :
	public auto_future::application
{

public:
	my_application(int argc, char **argv);
	void resLoaded();
	void onUpdate();

	virtual ~my_application();
#ifdef _SCOMMM
	bool create_run();
#endif // _SCOMMM
};

