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
	_name = jvalue["name"].asString();
	Value& jscreen_poses = jvalue["screen_pos"];
	for (int  ix = 0; ix < 4; ix++)
	{
		Value& spos = jscreen_poses[ix];
		_pos[ix].x = spos["x"].asDouble();
		_pos[ix].y = spos["y"].asDouble();
		_pos[ix].z = spos["z"].asDouble();
	}
	_texture_id_index = jvalue["texture_id_index"].asInt();
	_texture_indices[0] = jvalue["texture_index0"].asInt();
	_texture_indices[1] = jvalue["texture_index1"].asInt();
	_label = jvalue["label"].asString();
	return true;
}
