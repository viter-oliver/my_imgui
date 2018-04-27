#include "basic3d.h"

/*
        3              2


    0             1


	    7              6


	4              5
*/
tri_mesh::tri_mesh()
{
	glGenVertexArrays(1, &_vao);
	//vbo
	glGenBuffers(1, &_vbo);
	//ebo;
	glGenBuffers(1, &_ebo);
}
tri_mesh::~tri_mesh()
{
	glDeleteBuffers(1, &_ebo);
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}
void init_cube_trimesh_faces(tri_mesh& cube_trmesh)
{
	if (cube_trmesh.vertices.size()!=8)
	{
		printf("fail to pick up cube faces because the number of vertices is %d!\n", cube_trmesh.vertices.size());
		return;
	}
#define FACE_INIT(x,y,z) cube_trmesh.faces.push_back(tri_face(x, y, z))
	FACE_INIT(0, 4, 5);
	FACE_INIT(5, 1, 0);

	FACE_INIT(3, 2, 6);
	FACE_INIT(6, 7, 3);

	FACE_INIT(7, 4, 0);
	FACE_INIT(0, 3, 7);

	FACE_INIT(6, 2, 1);
	FACE_INIT(1, 5, 6);

	FACE_INIT(0, 1, 2);
	FACE_INIT(2, 3, 0);

	FACE_INIT(4, 7, 6);
	FACE_INIT(6, 5, 4);

}
void tri_mesh_normalize(tri_mesh& trmesh)
{
	for (auto& ia:trmesh.faces)
	{
		ImVec3 ev1=trmesh.vertices[ia.vidx[0]].position - trmesh.vertices[ia.vidx[1]].position;
		glm::vec3 e1 = { ev1.x, ev1.y, ev1.z };
		ImVec3 ev2=trmesh.vertices[ia.vidx[2]].position - trmesh.vertices[ia.vidx[1]].position;
		glm::vec3 e2 = { ev2.x, ev2.y, ev2.z };
		glm::vec3 no = glm::cross(e1, e2);
		ImVec3 eno = { no.x, no.y, no.z };
		trmesh.vertices[ia.vidx[0]].vnormal += eno;
		trmesh.vertices[ia.vidx[1]].vnormal += eno;
		trmesh.vertices[ia.vidx[2]].vnormal += eno;
	}
	for (auto& ib:trmesh.vertices)
	{
		glm::vec3 enm= { ib.vnormal.x, ib.vnormal.y, ib.vnormal.z };
		glm::vec3 enmn = glm::normalize(enm);
		ib.vnormal = {enmn.x,enmn.y,enmn.z};
	}
}