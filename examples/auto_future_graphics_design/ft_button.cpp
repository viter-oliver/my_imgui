#include "ft_button.h"

void ft_button::draw()
{

}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void ft_button::draw_peroperty_page()
{

}
/*
fields:
screen_poses[4],
      {x,y,z}
texture_id_index,
texture_index0,
texture_index1,
label
*/
bool ft_button::init_from_json(Value& jvalue)
{
	ft_base::init_json_unit(jvalue);

	_pt._texture_indices[0] = jvalue["texture_index0"].asInt();
	_pt._texture_indices[1] = jvalue["texture_index1"].asInt();
	strcpy(_pt._label ,jvalue["label"].asCString());
	return true;
}
#endif
bool ft_button::handle_mouse()
{
	return true;
}

ft_button::ft_button()
	:ft_base()
{

}

