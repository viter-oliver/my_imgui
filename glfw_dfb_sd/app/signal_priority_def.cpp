#include "af_bind.h"
#include "signal_priority_def.h"
using namespace auto_future;


//en_sg_side_fcw2_aeb_fctb,//had_10(0x25f),ESC_2_1(0x132),FMradar_1(0x26A)
vsub_signal_unit side_sgu_fcw2_aeb_fctb=
{
	{false,[&](bool visible){set_property_aliase_value("show_fcw2_side", &visible);}},
	{false,[&](bool visible){set_property_aliase_value("show_aeb_side", &visible);}},
	{false,[&](bool visible){set_property_aliase_value("show_fctb", &visible);}},
};
//en_sg_side_bsd2,//LCDA_1(0x268)
vsub_signal_unit side_sgu_bsd2=
{
	{false,[&](bool visible){set_property_aliase_value("show_bsd2", &visible);}},
};
//en_sg_side_fcw1,//had_10(0x25f),
vsub_signal_unit side_sgu_fcw1=
{
	{false,[&](bool visible){set_property_aliase_value("show_fcw1_side", &visible);}},
};
//en_sg_side_fcta2,//more than 3s---FMradar_1(0x26A),HAD_6(0x116)|HAD_10(0x25f)
vsub_signal_unit side_sgu_fcta2=
{
	{false,[&](bool visible){set_property_aliase_value("show_fmradar", &visible);}},
};
//en_sg_side_fcta1,//FMradar_1(0x26A)
vsub_signal_unit side_sgu_fcta1=
{
	{false,[&](bool visible){set_property_aliase_value("show_fcta1", &visible);}},
};
//en_sg_side_bsd1,//LCDA_1(0x268)
vsub_signal_unit side_sgu_bsd11=
{
	{false,[&](bool visible){set_property_aliase_value("show_bsd", &visible);}},
};
//en_sg_side_acc_change_lane_auto,//HAD_1(0x118),HAD_7(0x25a)|HAD_5(0x265)
vsub_signal_unit side_sgu_changelane_auto=
{
	{false,[&](bool visible){set_property_aliase_value("show_acc_side", &visible);}},
	{false,[&](bool visible){set_property_aliase_value("show_lane_change_frames", &visible);}},
	{false,[&](bool visible){set_property_aliase_value("show_lane_change_frames", &visible);}},
};

vsg_pt_uint side_area_sgs=
{
	make_shared<signal_unit_deque>(side_sgu_fcw2_aeb_fctb),
	make_shared<signal_unit_deque>(side_sgu_bsd2),
	make_shared<signal_unit_deque>(side_sgu_fcw1),
	make_shared<signal_unit_deque>(side_sgu_fcta2),
	make_shared<signal_unit_deque>(side_sgu_fcta1),
	make_shared<signal_unit_deque>(side_sgu_bsd11),
	make_shared<signal_unit_deque>(side_sgu_changelane_auto),
};
