#include "main_menu_controller.h"
#include "enum_txt_name0.h"
#include "key_def.h"

extern msg_host_n g_msg_host;
#define _STR(x) #x
#define STR(x) _STR(x)
#define BASE_UC_REF(x) _##x=root->get_child(idx);_##x->set_visible(false);idx++

#define IMG_REF(x) _##x=static_cast<ft_image*> (root->get_child(idx));idx++
enum en_first_level_menu_id
{
	en_menu_navi,
	en_menu_setup,
	en_menu_media,
	en_menu_trip,
};

main_menu_controller::main_menu_controller(base_ui_component* root)
	:_root(root), _first_level_menu_index(en_menu_navi), _menu_level(0)
{
	int idx = 0;
	IMG_REF(menu_bg);
	IMG_REF(arrow_btn_left);
	IMG_REF(arrow_btn_right);
	IMG_REF(img_selected);
	//IMG_REF(menu_bg);
	BASE_UC_REF(info_icon_list);
	BASE_UC_REF(navi_group);
	BASE_UC_REF(setup_group);
	BASE_UC_REF(media_group);
	BASE_UC_REF(trip_group);

	g_msg_host.attach_monitor("the mileage", [this](unsigned char* pbuff, int len){
		u8 data_valid = *pbuff++;
		*(pbuff + 3) = 0;
		int cruising_mileage = *((int*)pbuff);
		auto tripb = static_cast<ft_textblock*>(_trip_group->get_child(1));
		auto ml_ed_value = static_cast<ft_textblock*>(tripb->get_child(0));
		char ch_ed_value[50] = { 0 };
		if (data_valid == 1)
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
	g_msg_host.attach_monitor("cruising mileage", [this](unsigned char* pbuff, int len){//Ðøº½Àï³Ì
		u8 data_valid = *pbuff++;
		*(pbuff + 3) = 0;
		int cruising_mileage = *((int*)pbuff);
		auto trip_endurance = static_cast<ft_slider*>(_trip_group->get_child(1));
		auto ml_ed_value = static_cast<ft_textblock*>(trip_endurance->get_child(1));
		char ch_ed_value[50] = { 0 };
		if (data_valid == 1)
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

}

void main_menu_controller::key_operation(u8 key_state, u8 key_type, u8 key_source)
{
	int prev_first_level_menu_index = _first_level_menu_index;
	switch (key_type)
	{
	case en_ktp_menu:
		_root->set_visible(true);
		break;
	case en_ktp_ok:
		break;
	case en_ktp_esc:
		if (_menu_level > 0)
		{
			_menu_level--;
		}
		if (_menu_level == 0)
		{
			_root->set_visible(false);
		}
		break;
	case en_ktp_left:
		if (_first_level_menu_index > en_menu_navi)
		{
			_first_level_menu_index--;
		}
		break;
	case en_ktp_right:
		if (_first_level_menu_index < en_menu_trip)
		{
			_first_level_menu_index++;
		}
		break;
	case en_ktp_up:

		break;
	case en_ktp_down:

		break;
	case en_ktp_ivi_disp:
		break;
	default:
		break;
	}
	if (prev_first_level_menu_index != _first_level_menu_index)
	{
		base_ui_component* pmenu_icon = _info_icon_list->get_child(prev_first_level_menu_index);
		pmenu_icon->set_visible(false);
		base_ui_component* pmenu_group = _root->get_child(prev_first_level_menu_index + 5);
		pmenu_group->set_visible(false);
		pmenu_icon = _info_icon_list->get_child(_first_level_menu_index);
		pmenu_icon->set_visible(true);
		pmenu_group = _root->get_child(_first_level_menu_index + 5);
		pmenu_group->set_visible(true);
	}
}