#pragma once
#include "afg.h"
#include "msg_host_n.h"
#define DEF_BASE_UC(x) base_ui_component* _##x
using namespace auto_future_utilities;
class test_model_controller
{
	base_ui_component* _root;
	DEF_BASE_UC(test_model_first_menu);
	/*DEF_BASE_UC(version_show);
	DEF_BASE_UC(screen_test_show);
	DEF_BASE_UC(brightness_test);
	DEF_BASE_UC(ddr_show);
	DEF_BASE_UC(emmc);
	DEF_BASE_UC(firmware);*/
	base_ui_component* _menu_group[6];
	DEF_BASE_UC(sel_frame);
	DEF_BASE_UC(debug);
	int _menu_idx, _menu_level,_screen_test_id,_brightness_test_level;
	bool _debug_model;
public:
	test_model_controller(base_ui_component* root);
	~test_model_controller();
	void key_operation(u8 key_state, u8 key_type, u8 key_source);
};

