#include "ft_slide.h"

void ft_slide::draw()
{

}
#if !defined(IMGUI_WAYLAND)
void ft_slide::draw_peroperty_page()
{

}
#endif
/*
fields:
texture_id_index,
thumb[4],
  {x,y,z}
background[4],
  {x,y,z}
texture_index_thumb,
texture_index_background
*/
bool ft_slide::init_from_json(Value& jvalue)
{
	_name = jvalue["name"].asString();
	_texture_id_index = jvalue["texture_id_index"].asInt();
	Value& jthumb = jvalue["thumb"];
	for (int ix = 0; ix < 4;ix++)
	{
		Value& jspos = jthumb[ix];
		_thumb[ix].x = jspos["x"].asDouble();
		_thumb[ix].y = jspos["y"].asDouble();
		_thumb[ix].z = jspos["z"].asDouble();
	}
	Value& jbackground = jvalue["background"];
	for (int ix = 0; ix < 4; ix++)
	{
		Value& jspos = jbackground[ix];
		_background[ix].x = jspos["x"].asDouble();
		_background[ix].y = jspos["y"].asDouble();
		_background[ix].z = jspos["z"].asInt();
	}
	_texture_index_thumb = jvalue["texture_index_thumb"].asInt();
	_texture_index_background = jvalue["texture_index_background"].asInt();
	return true;
}

bool ft_slide::handle_mouse()
{
	return false;
}
