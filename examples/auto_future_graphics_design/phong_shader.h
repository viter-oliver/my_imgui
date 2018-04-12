#pragma once
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "basic3d.h"
class phong_shader
{
	GLuint _vao, _vbo,_ebo;
	GLuint _vertex_shader,_fragment_shader,_shader_program;
	//attribute
	GLuint _kzPosition, _kzNormal;
	//uniform
	//mat4
	GLuint _ProjectionCameraWorldMatrix, _kzWorldMatrix, _kzNormalMatrix;
	//vec3
	GLuint _kzCameraPosition, _PointLightPosition2, _PointLightAttenuation2, _DirectionalLightDirection1;
	//vec4 color
	GLuint _PointLightColor2, _DirectionalLightColor1, _Emissive, _Ambient, _Diffuse, _SpecularColor;
	//float
	GLuint _SpecularExponent, _BlendIntensity;
public:
	phong_shader();
	~phong_shader();
	void render_tri_mesh(tri_mesh& tmesh);
	void set_vertex_wrold_matrix(glm::mat4& vtmat);
};

