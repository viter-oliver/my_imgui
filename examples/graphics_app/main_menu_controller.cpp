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
	:_root(root)
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
	g_msg_host.attach_monitor("cruising mileage", [this](unsigned char* pbuff, int len){
		u8 data_valid = *pbuff;
		*pbuff = 0;
		int cruising_mileage = *((int*)pbuff);
		auto mileage_endurance = static_cast<ft_slider*>(_mileage_group->get_child(3));

		auto ml_ed_value =static_cast<ft_textblock*>( mileage_endurance->get_child(0));
		char ch_ed_value[50] = { 0 };
		sprintf(ch_ed_value, "%d", cruising_mileage);
		ml_ed_value->set_content(ch_ed_value);

	});
}
