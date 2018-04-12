#pragma once
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
using namespace std;
struct base_vertex
{
	glm::vec3 position;
	glm::vec3 vnormal;
	//base_vertex(){ vnormal = glm::vec3(0.f); }
};
struct tri_face
{
	int vidx[3];
	tri_face(){ vidx[0] = vidx[1] = vidx[2] = 0; }
	tri_face(int i0, int i1, int i2){ vidx[0] = i0; vidx[1] = i1; vidx[2] = i2; }
};
struct tri_mesh 
{
	vector<base_vertex> vertices;
	vector<tri_face> faces;
};
extern void init_cube_trimesh_faces(tri_mesh& cube_trmesh);
extern void tri_mesh_normalize(tri_mesh& trmesh);
