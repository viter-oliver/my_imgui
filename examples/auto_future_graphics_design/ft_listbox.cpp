#include "ft_listbox.h"

void ft_listbox::draw()
{

}
#if !defined(IMGUI_WAYLAND)
void ft_listbox::draw_peroperty_page()
{

}
#endif
bool ft_listbox::init_from_json(Value& jvalue)
{
	return true;
}

bool ft_listbox::handle_mouse()
{
	return false;
}

ft_listbox::ft_listbox()
	:ft_base()
	, _vertical(true)
	, _width(0)
	, _height(0)
{

}

ft_listbox::ft_listbox(ft_listbox& bsource)
	: ft_base(bsource)
{
	_vertical = bsource._vertical;
	_width = bsource._width;
	_height = bsource._height;
}

base_ui_component* ft_listbox::get_a_copy()
{
	return new ft_listbox(*this);
}
