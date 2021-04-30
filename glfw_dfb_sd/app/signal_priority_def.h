#include "priority_control.h"
enum en_signal_priority_side_area
{
	en_sg_side_fcw2_aeb_fctb,//had_10(0x25f),ESC_2_1(0x132),FMradar_1(0x26A)
	en_sg_side_bsd2,//LCDA_1(0x268)
	en_sg_side_fcw1,//had_10(0x25f),
	en_sg_side_fcta2,//more than 3s---FMradar_1(0x26A),HAD_6(0x116)|HAD_10(0x25f)
	en_sg_side_fcta1,//FMradar_1(0x26A)
	en_sg_side_bsd1,//LCDA_1(0x268)
	en_sg_side_acc_change_lane_auto,//HAD_1(0x118),HAD_7(0x25a)|HAD_5(0x265)
	en_sg_signals_side_cnt,
};

