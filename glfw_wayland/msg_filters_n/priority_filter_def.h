#ifndef PRIORITY_FILTER_DEF
#define PRIORITY_FILTER_DEF
enum priority_filter_status_type
{
	en_op_off,// state off
	en_op_on,//state on
	en_op_preempted,//be preempted by some higher priority data object
};

enum priority_filter_peps_type
{
	en_nonindication,
	en_sys_exception_start,
	en_switch_p_or_n_start,
	en_clutch_start_engine,
	en_foot_brake_start_engine,
	en_switch_p_shut_supply,
	en_switch_n_start_engine,
};
#endif // !priority_filter_def
