#include "ft_polygon_image.h"


void ft_polygon_image::draw()
{

}
#if !defined(IMGUI_WAYLAND)
void ft_polygon_image::draw_peroperty_page()
{

}
#endif
/*
fields:
texture_id_index,
vertexes:
  {x,y,z,u,v,col}
*/
bool ft_polygon_image::init_from_json(Value& jvalue)
{
	_name = jvalue["name"].asString();
	Value& jvertexes = jvalue["vertexes"];
	int isize = jvertexes.size();
	for (int ix = 0; ix < isize; ix++)
	{
		Value& jvertex = jvertexes[ix];
		ft_vertex vertex;
		vertex.pos.x = jvertex["x"].asDouble();
		vertex.pos.y = jvertex["y"].asDouble();
		vertex.pos.z = jvertex["z"].asDouble();
		vertex.uv.x= jvertex["u"].asDouble();
		vertex.uv.x = jvertex["v"].asDouble();
		vertex.col = jvertex["col"].asUInt();
		_vertexes.push_back(vertex);
	}
	return true;
}

bool ft_polygon_image::handle_mouse()
{
	return false;
}
