#include "main_menu_controller.h"
#include "msg_host_n.h"
#include "enum_txt_name0.h"
using namespace auto_future_utilities;
extern msg_host_n g_msg_host;
#define _STR(x) #x
#define STR(x) _STR(x)
#define BASE_UC_REF(x) _##x=root->get_child(idx);_##x->set_visible(false);idx++

#define IMG_REF(x) _##x=static_cast<ft_image*> (root->get_child(idx));idx++

//img_selected
const ImVec2 first_level_menu_img_sel_pos[] =
{
	{ 1188, 114 },
	{ 1153, 176 },
	{ 1117, 237 },
	{ 1083, 296 },
	{ 1048, 357 },
	{ 1009, 426 },
	{ 971, 490 },
};
enum main_menu_pos_index
{
	en_drive_model,
	en_media,
	en_map,
	en_tpms,
	en_fuel_consumption,
	en_mileage,
	en_infomation,
};

const ImVec2 media_img_sel_pos[] =
{
	{ 1142, 192 },//info,drive,fuel
	{ 1102, 263 },
	{ 1059, 338 },
};
const ImVec2 popup_img_sel_pos[] =
{
	{ 82, 175 },
	{ 210, 175 },
};
main_menu_controller::main_menu_controller(base_ui_component* root)
	:_root(root), _first_level_menu_index(en_drive_model), _menu_level(0)
{
	_menu_bg = static_cast<ft_image*>(root->get_child(0));
	int idx = 2;
	IMG_REF(img_selected);
	IMG_REF(second_level_title);
	//IMG_REF(menu_bg);
	BASE_UC_REF(first_level_menu);
	BASE_UC_REF(media_group);
	BASE_UC_REF(tpms_group);
	BASE_UC_REF(fuel_cons_group);
	BASE_UC_REF(mileage_group);
	BASE_UC_REF(info_group);
	BASE_UC_REF(drive_mode_group);
	BASE_UC_REF(navi_group);
	_img_selected->base_pos() = first_level_menu_img_sel_pos[_first_level_menu_index];
	g_msg_host.attach_monitor("cruising mileage", [this](unsigned char* pbuff, int len){
		u8 data_valid = *pbuff++;
		*(pbuff+3) = 0;
		int cruising_mileage = *((int*)pbuff);
		auto mileage_endurance = static_cast<ft_slider*>(_mileage_group->get_child(3));
		auto ml_ed_value =static_cast<ft_textblock*>( mileage_endurance->get_child(0));
		char ch_ed_value[50] = { 0 };
		if (data_valid==1)
		{
			sprintf(ch_ed_value, "%d", cruising_mileage);
		}
		else
		{
			ch_ed_value[0] = '-';
			ch_ed_value[1] = '-';
		}
		ml_ed_value->set_content(ch_ed_value);

	});
#if 1
	g_msg_host.attach_monitor("key value", [this](unsigned char* pbuff, int len){
		enum en_key_state
		{
			en_key_value,
			en_key_press,
			en_key_release,
			en_key_long_press,
		};
		enum en_key_type
		{
			en_ktp_none,
			en_ktp_menu,
			en_ktp_ok,
			en_ktp_esc,
			en_ktp_left,
			en_ktp_right,
			en_ktp_up,
			en_ktp_down,
			en_ktp_ivi_disp,
		};
		enum en_key_source
		{
			en_ks_dashboard,
			en_ks_ir,
			en_ks_rotary_knob,
			en_ks_steering_wheel,
			en_ks_ivi,
		};
		u8 key_state = *pbuff++;//0 只有按键值，1、按下 2、弹起 3、长按。
		u8 key_type = *pbuff++;
		int key_value=*((int*)pbuff);
		u8 key_source = *(pbuff + 5);//0、面板按键 1、遥控按键 2、旋钮按键 3、方向盘按键 4、中控按键
		switch (key_type)
		{
		case en_ktp_menu:
			_root->set_visible(true);
			break;
		case en_ktp_ok:
			break;
		case en_ktp_esc:
			if (_menu_level>0)
			{
				_menu_level--;
			}
			if (_menu_level==0)
			{
				_root->set_visible(false);
			}
			break;
		case en_ktp_left:
			break;
		case en_ktp_right:
			break;
		case en_ktp_up:
			if (_first_level_menu_index>en_drive_model)
			{
				_first_level_menu_index--;
				_img_selected->base_pos() = first_level_menu_img_sel_pos[_first_level_menu_index];
			}
			break;
		case en_ktp_down:
			if (_first_level_menu_index < en_infomation)
			{
				_first_level_menu_index++;
				_img_selected->base_pos() = first_level_menu_img_sel_pos[_first_level_menu_index];
			}
			break;
		case en_ktp_ivi_disp:
			break;
		default:
			break;
		}
		if (_root->is_visible())
		{
			if (_menu_level==0)
			{
				_second_level_title->set_visible(false);
				_first_level_menu->set_visible(true);
				_img_selected->set_visible(true);
			}
			else
			{
				_second_level_title->set_visible(true);
				_first_level_menu->set_visible(false);
				_img_selected->set_visible(false);
				switch (_first_level_menu_index)
				{
				case en_drive_model:
					break;
				case en_media:
					break;
				case en_map:
					break;
				case en_tpms:
					break;
				case en_fuel_consumption:
					break;
				case en_mileage:
					break;
				case en_infomation:
					break;
				default:
					break;
				}
			}
		}
	});
#endif
}
