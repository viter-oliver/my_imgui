#include "basic3d.h"

/*
        3              2


    0             1


	    7              6


	4              5
*/
void init_cube_trimesh_faces(tri_mesh& cube_trmesh)
{
	if (cube_trmesh.vertices.size()!=8)
	{
		printf("fail to pick up cube faces because the number of vertices is %d!\n", cube_trmesh.vertices.size());
		return;
	}
	cube_trmesh.faces.push_back(tri_face(0, 1, 2));
	cube_trmesh.faces.push_back(tri_face(2, 3, 0));

	cube_trmesh.faces.push_back(tri_face(1, 5, 6));
	cube_trmesh.faces.push_back(tri_face(6, 2, 1));

	cube_trmesh.faces.push_back(tri_face(5, 6, 7));
	cube_trmesh.faces.push_back(tri_face(7, 4, 5));

	cube_trmesh.faces.push_back(tri_face(7, 3, 0));
	cube_trmesh.faces.push_back(tri_face(0, 4, 7));

	cube_trmesh.faces.push_back(tri_face(0, 4, 5));
	cube_trmesh.faces.push_back(tri_face(5, 1, 0));

	cube_trmesh.faces.push_back(tri_face(6, 2, 3));
	cube_trmesh.faces.push_back(tri_face(3, 7, 6));


}
void tri_mesh_normalize(tri_mesh& trmesh)
{
	for (auto ia:trmesh.faces)
	{
		glm::vec3 e1 = trmesh.vertices[ia.vidx[0]].position - trmesh.vertices[ia.vidx[1]].position;
		glm::vec3 e2 = trmesh.vertices[ia.vidx[2]].position - trmesh.vertices[ia.vidx[1]].position;
		glm::vec3 no = glm::cross(e1, e2);
		trmesh.vertices[ia.vidx[0]].vnormal += no;
		trmesh.vertices[ia.vidx[1]].vnormal += no;
		trmesh.vertices[ia.vidx[2]].vnormal += no;
	}
	for (auto& ib:trmesh.vertices)
	{
		ib.vnormal = glm::normalize(ib.vnormal);
	}
}