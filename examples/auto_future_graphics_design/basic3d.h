#pragma once
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif
using namespace std;
struct base_vertex
{
	ImVec3 position;
	ImVec3 vnormal;
	base_vertex(float x, float y, float z) :position(x, y, z){}
	//base_vertex(){ vnormal = glm::vec3(0.f); }
};
struct tri_face
{
	unsigned short vidx[3];
	tri_face(){ vidx[0] = vidx[1] = vidx[2] = 0; }
	tri_face(int i0, int i1, int i2){ vidx[0] = i0; vidx[1] = i1; vidx[2] = i2; }
};
struct tri_mesh 
{
	vector<base_vertex> vertices;
	vector<tri_face> faces;
	GLuint _vbo, _vao, _ebo;
	tri_mesh();
	~tri_mesh();
};
struct basic_shader
{
	virtual void load_tri_mesh(tri_mesh& tmesh)=0;
	virtual void render_tri_mesh(tri_mesh& tmesh)=0;
	virtual void set_vertex_wrold_matrix(glm::mat4& vtmat)=0;
};
extern void init_cube_trimesh_faces(tri_mesh& cube_trmesh);
extern void tri_mesh_normalize(tri_mesh& trmesh);
