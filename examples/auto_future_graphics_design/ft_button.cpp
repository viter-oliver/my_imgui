#include "ft_button.h"

void ft_button::draw()
{

}
#if !defined(IMGUI_WAYLAND)
void ft_button::draw_peroperty_page()
{

}
#endif
bool ft_button::handle_mouse()
{
	return true;
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

	_texture_indices[0] = jvalue["texture_index0"].asInt();
	_texture_indices[1] = jvalue["texture_index1"].asInt();
	_label = jvalue["label"].asString();
	return true;
}

ft_button::ft_button()
	:ft_base()
{

}

ft_button::ft_button(ft_button& bsource)
	:ft_base(bsource)
{
	_texture_indices[0] = bsource._texture_indices[0];
	_texture_indices[1] = bsource._texture_indices[1];
}

base_ui_component* ft_button::get_a_copy()
{
	return new ft_button(*this);
}
