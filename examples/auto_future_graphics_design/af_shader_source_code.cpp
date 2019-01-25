#include "af_shader_source_code.h"
const char* single_txt_vs = R"glsl(
layout(location = 0)attribute vec3 position;
layout(location = 1)attribute vec2 textcoord;
layout(location = 2)attribute vec3 normal;
varying vec2 Textcoord;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
void main()
{
    gl_Position = proj * view * model * vec4(position, 1.0);
    Textcoord = textcoord;
}
)glsl";
const char* single_txt_fs = R"glsl(
varying vec2 Textcoord;
uniform sampler2D text;
void main()
{
   gl_FragColor = texture(text, Textcoord);
}
)glsl";

const char* color_obj_vs = R"glsl(
layout(location = 0) attribute vec3 position;
layout(location = 1) attribute vec3 color;
layout(location = 2) attribute vec2 texcoord;
varying vec3 Color;
varying vec2 Texcoord;
uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 proj;
void main()
{
	Color = color;
	Texcoord = texcoord;
	gl_Position = proj * view * model * vec4(position, 1.0);
}
)glsl";
const char* color_obj_fs = R"glsl(
varying vec3 Color;
varying vec2 Texcoord;
void main()
{
	gl_FragColor = vec4(Color, 0.0);
}
)glsl";

const char* particles1_vs = R"glsl(
#version 330 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; 

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 particlecolor;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)

void main()
{
	float particleSize = xyzs.w; // because we encoded it this way.
	vec3 particleCenter_wordspace = xyzs.xyz;
	
	vec3 vertexPosition_worldspace = 
		particleCenter_wordspace
		+ CameraRight_worldspace * squareVertices.x * particleSize
		+ CameraUp_worldspace * squareVertices.y * particleSize;

	// Output position of the vertex
	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

	// UV of the vertex. No special space for this one.
	UV = squareVertices.xy + vec2(0.5, 0.5);
	particlecolor = color;
}
)glsl";
const char* particles1_fs = R"glsl(
#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 particlecolor;

// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
	// Output color = color of the texture at the specified UV
	//color = texture( myTextureSampler, UV ) * particlecolor;
	color = texture( myTextureSampler, UV );
}
)glsl";

const char* particles2_vs = R"glsl(
#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; 

// Output data ; will be interpolated for each fragment.
out vec2 UV;
uniform vec2 uvcol[120];
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)
void main()
{
	float particleSize = xyzs.w;
	int uv_index = int(particleSize); // because we encoded it this way.
	uv_index=uv_index*4;
	//gggid=gl_VertexID;
	uv_index=uv_index+gl_VertexID%4;
	particleSize=particleSize/20;
	vec3 particleCenter_wordspace = xyzs.xyz;
	
	vec3 vertexPosition_worldspace = 
		particleCenter_wordspace
		+ CameraRight_worldspace * squareVertices.x * particleSize
		+ CameraUp_worldspace * squareVertices.y * particleSize;

	// Output position of the vertex
	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

	// UV of the vertex. No special space for this one.
	UV = uvcol[uv_index];
}
)glsl";
const char* particles2_fs = R"glsl(
#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
out vec4 color;
uniform sampler2D myTextureSampler;

void main(){
	color = texture( myTextureSampler, UV );
}
)glsl";