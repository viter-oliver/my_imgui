#include "test_model_controller.h"
#include "key_def.h"

#define GET_BUI_REF(x) _##x=static_cast<base_ui_component*> (root->get_child(idx));idx++

test_model_controller::test_model_controller(base_ui_component* root)
	:_root(root), _menu_idx(0), _menu_level(0),
	_screen_test_id(0), _brightness_test_level(20),_debug_model(false)
{
	//root->set_visible(true);
	int idx = 0;
	GET_BUI_REF(test_model_first_menu);
	_menu_group[0] = _test_model_first_menu;
	_menu_group[1] = _root->get_child(1);
	_menu_group[2] = _root->get_child(2);
	_menu_group[3] = _root->get_child(3);
	_menu_group[4] = _root->get_child(4);
	_menu_group[5] = _root->get_child(5);

	/*GET_BUI_REF(version_show);
	GET_BUI_REF(screen_test_show);
	GET_BUI_REF(brightness_test);
	GET_BUI_REF(ddr_show);
	GET_BUI_REF(emmc);
	GET_BUI_REF(firmware);*/

	_sel_frame = _test_model_first_menu->get_child(1);
	_debug=_test_model_first_menu->get_child(2);

}


test_model_controller::~test_model_controller()
{
}
enum en_test_model_menu_idx
{
	en_debug,
	en_version,
	en_screen,
	en_brightness,
	en_ddr,
	en_emmc,
	en_firmware,
};
int sel_frame_pos_y[]=
{
	100,
	175,
	250,
	325,
	400,
	475,
};
enum en_screen_test_model
{
	en_black_test,
	en_white_test,
	en_red_test,
	en_green_test,
	en_blue_test,
	en_calibration1_test,
	en_calibration2_test,
};
float screen_test_colors[en_calibration1_test][3] = {
	{ 0.f, 0.f, 0.f, },
	{ 1.f, 1.f, 1.f, },
	{ 1.f, 0.f, 0.f, },
	{ 0.f, 1.f, 0.f, },
	{ 0.f, 0.f, 1.f, },
};
enum en_brightness_test_level
{
	en_bn_level1=1,
	en_bn_level20=20,
};
void test_model_controller::key_operation(u8 key_state, u8 key_type, u8 key_source)
{
	int prev_menu_index = _menu_idx;
	int prev_screen_test_id = _screen_test_id;
	int prev_brightness_level = _brightness_test_level;
	switch (key_type)
	{
	case en_ktp_menu:
		_root->set_visible(true);
		break;
	case en_ktp_ok:
		if (_menu_idx==0)
		{
			auto on_off = _debug->get_child(0);
			auto checked = on_off->get_child(0);
			_debug_model = checked->is_visible();
			_debug_model = !_debug_model;
			checked->set_visible(_debug_model);
		}
		else if (_menu_level<1){
			_menu_level++;
			_test_model_first_menu->set_visible(false);
			_menu_group[_menu_idx]->set_visible(true);
		}
		break;
	case en_ktp_esc:

		if (_menu_level == 0)
		{
			_root->set_visible(false);
		}
		else
		{
			_menu_level--;
			if (_menu_level==0)
			{
				_test_model_first_menu->set_visible(true);
				_menu_group[_menu_idx]->set_visible(false);
			}
		}
		break;

	case en_ktp_up:
		if (_menu_idx>en_debug)
		{
			_menu_idx--;
		}
		break;
	case en_ktp_down:
		if (_menu_idx <en_firmware)
		{
			_menu_idx++;
		}
		break;
	case en_ktp_left:
		if (_menu_level>0)
		{
			if (_menu_idx==en_screen&&_screen_test_id>en_black_test)
			{
				_screen_test_id--;
			}
			else if (_menu_idx==en_brightness&&_brightness_test_level>en_bn_level1)
			{
				_brightness_test_level--;
			}
		}
		break;
	case en_ktp_right:
		if (_menu_level > 0)
		{
			if (_menu_idx == en_screen&&_screen_test_id <en_calibration2_test)
			{
				_screen_test_id++;
			}
			else if (_menu_idx==en_brightness&&_brightness_test_level<en_bn_level20)
			{
				_brightness_test_level++;
			}
		}
		break;
	case en_ktp_ivi_disp:
		break;
	default:
		break;
	}
	if (prev_menu_index != _menu_idx)
	{
		_sel_frame->set_base_posy(sel_frame_pos_y[_menu_idx]);
	}
	if (prev_screen_test_id!=_screen_test_id)
	{
		if (prev_screen_test_id>en_blue_test)
		{
			auto prev_test = _menu_group[en_screen]->get_child(prev_screen_test_id - en_calibration1_test);
			prev_test->set_visible(false);
		}
		if (_screen_test_id>en_blue_test)
		{
			auto prev_test = _menu_group[en_screen]->get_child(_screen_test_id - en_calibration1_test);
			prev_test->set_visible(true);
		}
		else
		{
			auto screen_test = static_cast<ft_block*>(_menu_group[en_screen]);
			float r = screen_test_colors[_screen_test_id][0];
			float g = screen_test_colors[_screen_test_id][1];
			float b = screen_test_colors[_screen_test_id][2];
			screen_test->set_background_color(r,g,b,1.f);
		}
	}
	else if (prev_brightness_level!=_brightness_test_level)
	{
		auto brg_test = _menu_group[en_brightness];
		auto brg_test_level_txt = static_cast<ft_textblock*>(brg_test->get_child(0));
		char str_show[50] = { 0 };
		sprintf(str_show, "%d", _brightness_test_level);
		brg_test_level_txt->set_content(str_show);
	}
}