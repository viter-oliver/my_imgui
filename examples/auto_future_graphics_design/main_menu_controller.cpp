#include "main_menu_controller.h"
#define _STR(x) #x
#define STR(x) _STR(x)
#define BASE_UC_REF(x) if (!_##x&&object_name==STR(x)) {_##x=pobj;continue; }
#define IMG_REF(x) if (!_##x&&object_name==STR(x)) {_##x=static_cast<ft_image*> (pobj);continue;}
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
	auto chd_cnt = root->get_child_count();
	for (auto ix = 0; ix < chd_cnt;++ix)
	{
		auto pobj = root->get_child(ix);
		auto object_name = pobj->get_name();
		IMG_REF(img_selected);
		IMG_REF(second_level_title);
		IMG_REF(menu_bg);
		BASE_UC_REF(first_level_menu);
		BASE_UC_REF(media_group);
		BASE_UC_REF(tpms_group);
		BASE_UC_REF(fuel_cons_group);
		BASE_UC_REF(mileage_group);
		BASE_UC_REF(info_group);
		BASE_UC_REF(drive_mode_group);
		BASE_UC_REF(navi_group);
	}
}
