#include "ft_cube.h"
#include "color_shader.h"
/*
      3              2


0             1


      7              6


4              5
*/

ft_cube::ft_cube()
	:ft_base()
{
#define INI_MESH(x,y,z) _mesh.vertices.push_back(base_vertex(x,y,z))
	INI_MESH(-0.25f, -0.25f, -0.25f);
	INI_MESH(0.25f, -0.25f, -0.25f);
	INI_MESH(0.25f, -0.25f, 0.25f);
	INI_MESH(-0.25f, -0.25f, 0.25f);
	INI_MESH(-0.25f, 0.25f, -0.25f);
	INI_MESH(0.25f, 0.25f, -0.25f);
	INI_MESH(0.25f, 0.25f, 0.25f);
	INI_MESH(-0.25f, 0.25f, 0.25f);
	init_cube_trimesh_faces(_mesh);
	_pshader = new color_shader(_mesh);
	_pshader->load_tri_mesh();

}

ft_cube::ft_cube(ft_cube& bsource)
	: ft_base(bsource)
{
	_mesh = bsource._mesh;
	_pshader =bsource. _pshader;
	_scale = bsource._scale;
	_rotation = bsource._rotation;
	_translation = bsource._translation;

}

ft_cube::~ft_cube()
{
	delete _pshader;
}

void ft_cube::draw()
{
	
	//_pshader->set_vertex_wrold_matrix(mod);
	_pshader->render_tri_mesh();
}
#if !defined(IMGUI_WAYLAND)
void ft_cube::draw_peroperty_page()
{
}
#endif
bool ft_cube::init_from_json(Value& jvalue)
{
	return true;
}

bool ft_cube::init_json_unit(Value& junit)
{
	return true;
}
