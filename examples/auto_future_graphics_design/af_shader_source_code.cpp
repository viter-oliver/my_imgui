#include "af_shader_source_code.h"

const char* single_txt;
const char* single_txt_vs = R"glsl(#version 300 es

in vec3 position;
in vec3 color;
in vec2 textCoord;

out vec3 VertColor;
out vec2 TextCoord;
uniform mat4 trans;
void main()
{
	gl_Position = trans*vec4(position, 1.0);
	VertColor = color;
	TextCoord = textCoord;
}
)glsl";
const char* single_txt_fs = R"glsl(#version 300 es

in vec3 VertColor;
in vec2 TextCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float mixValue;

out vec4 color;


void main()
{
	vec4 color1 = texture(tex1, TextCoord);
	//vec4 color2 = texture(tex2, vec2(TextCoord.s, 1.0 - TextCoord.t));
	
	//vec4 color2= texture(tex2, TextCoord);
	//color = mix(color1, color2, mixValue);
	color=color1;
}
)glsl";

const char* color_obj = "color_obj";
const char* color_obj_vs = R"glsl(#version 300 es
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
const char* color_obj_fs = R"glsl(#version 300 es
varying vec3 Color;
varying vec2 Texcoord;
void main()
{
	gl_FragColor = vec4(Color, 0.0);
}
)glsl";

const char* particles1 = "particles1";
const char* particles1_vs = R"glsl(#version 330 core
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
const char* particles1_fs = R"glsl(#version 330 core

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
const char* particles2 = "particles2";
const char* particles2_vs = R"glsl(#version 330 core

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
const char* particles2_fs = R"glsl(#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
out vec4 color;
uniform sampler2D myTextureSampler;

void main(){
	color = texture( myTextureSampler, UV );
}
)glsl";

const char* modeling = "modeling";
const char* modeling_vs=R"glsl(#version 300 es
precision mediump float;
layout(location=0) in vec3 normal;
layout(location=1) in vec3 position;
layout(location=2) in vec2 textCoord;
out vec3 FragPos;
out vec2 TextCoord;
out vec3 FragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
	FragPos = vec3(model * vec4(position, 1.0));//special a position for fragment
    TextCoord = textCoord;
	mat3 normalMatrix = mat3(transpose(inverse(model)));
	FragNormal = normalMatrix * normal; //calculate normal
}
)glsl";
const char* modeling_fs=R"glsl(#version 300 es
precision mediump float;
in vec3 FragPos;
in vec2 TextCoord;
in vec3 FragNormal;
out vec4 o_clr;
uniform vec3 light_position;
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;
uniform float light_constant;
uniform float light_linear;
uniform float light_quadratic;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse0;
//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular0;
//uniform sampler2D texture_specular1;
//uniform sampler2D texture_specular2;
void main()
{
	vec3	ambient = light_ambient * vec3(texture(texture_diffuse0, TextCoord));

	vec3	lightDir = normalize(light_position - FragPos);
	vec3	normal = normalize(FragNormal);
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * light_diffuse * vec3(texture(texture_diffuse0, TextCoord));
	float	specularStrength = 0.5f;
	vec3	reflectDir = normalize(reflect(-lightDir, normal));
	vec3	viewDir = normalize(viewPos - FragPos);
	float	specFactor = pow(max(dot(reflectDir, viewDir), 0.0), 64.0f);
	vec3	specular = specFactor * light_specular * vec3(texture(texture_specular0, TextCoord));
	float distance = length(light_position - FragPos);
	float attenuation = 1.0f / (light_constant 
			+ light_linear * distance
			+ light_quadratic * distance * distance);

	vec3	result = (ambient + diffuse + specular) * attenuation;
	//vec3 result=vec3(0,0,0);
	o_clr	= vec4(result , 1.0f);
}
)glsl";

const char* modeling_vs_col = R"glsl(#version 300 es
precision mediump float;
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
out vec3 FragPos;
out vec3 FragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
	FragPos = vec3(model * vec4(position, 1.0));//special a position for fragment
	mat3 normalMatrix = mat3(transpose(inverse(model)));
	FragNormal = normalMatrix * normal; //calculate normal
}
)glsl";
const char* modeling_fs_col = R"glsl(#version 300 es
precision mediump float;
in vec3 FragPos;
in vec3 FragNormal;
out vec4 o_clr;
uniform vec3 light_position;

uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

uniform vec3 viewPos;
uniform vec3 col_ambient;
uniform vec3 col_diffuse;
uniform vec3 col_specular;
void main()
{
	vec3	ambient = light_ambient * col_ambient;

	vec3	lightDir = normalize(light_position - FragPos);
	vec3	normal = normalize(FragNormal);
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * light_diffuse * col_diffuse;
	float	specularStrength = 0.5f;
	vec3	reflectDir = normalize(reflect(-lightDir, normal));
	vec3	viewDir = normalize(viewPos - FragPos);
	float	specFactor = pow(max(dot(reflectDir, viewDir), 0.0), 64.0f);
	vec3	specular = specFactor * light_specular * col_specular;
	
	vec3	result = ambient + diffuse + specular;
	o_clr	= vec4(result , 1.0f);
}
)glsl";