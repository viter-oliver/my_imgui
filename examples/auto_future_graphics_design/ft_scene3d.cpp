#include "ft_scene3d.h"
#include "common_functions.h"

ft_scene3d::ft_scene3d(ft_scene3d& bsource)
{
	prepareFBO1(_colorTextId, _depthStencilTextId, _fboId, screenw, screenh);

}


ft_scene3d::~ft_scene3d()
{
}

void ft_scene3d::draw()
{

}
#if !defined(IMGUI_WAYLAND)
void ft_scene3d::draw_peroperty_page()
{

}
#endif

bool ft_scene3d::init_from_json(Value& jvalue)
{
	return true;
}

bool ft_scene3d::handle_mouse()
{
	return true;
}

void ft_scene3d::set_light_type(en_light_type ltype)
{

}

