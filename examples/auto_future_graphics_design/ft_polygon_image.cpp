#include "ft_polygon_image.h"
namespace auto_future
{
	void ft_polygon_image::draw()
	{

	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void ft_polygon_image::draw_peroperty_page(int property_part)
	{

	}
	/*
	fields:
	texture_id_index,
	vertexes:
	{x,y,z,u,v,col}
	*/
	bool ft_polygon_image::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		Value& jvertexes = jvalue["vertexes"];
		int isize = jvertexes.size();
		for (int ix = 0; ix < isize; ix++)
		{
			Value& jvertex = jvertexes[ix];
			ft_vertex vertex;
			vertex.pos.x = jvertex["x"].asDouble();
			vertex.pos.y = jvertex["y"].asDouble();
			vertex.pos.z = jvertex["z"].asDouble();
			vertex.uv.x = jvertex["u"].asDouble();
			vertex.uv.x = jvertex["v"].asDouble();
			vertex.col = jvertex["col"].asUInt();
			_vertexes.push_back(vertex);
		}
		return true;
	}
#endif

	bool ft_polygon_image::handle_mouse()
	{
		return false;
	}
}