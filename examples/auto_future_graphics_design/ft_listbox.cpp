#include "ft_listbox.h"

void ft_listbox::draw()
{

}
#if !defined(IMGUI_WAYLAND)
void ft_listbox::draw_peroperty_page()
{

}
bool ft_listbox::init_from_json(Value& jvalue)
{
	return true;
}
#endif

bool ft_listbox::handle_mouse()
{
	return false;
}

ft_listbox::ft_listbox()
	:ft_base()
	, _lt_pt()
{

}
