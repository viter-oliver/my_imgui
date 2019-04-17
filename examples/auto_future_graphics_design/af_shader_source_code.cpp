#include "af_shader_source_code.h"

const char* single_txt;
const char* single_txt_vs = R"glsl(
layout(location = 0)attribute vec3 position;
layout(location = 1)attribute vec2 textcoord;
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

const char* color_obj = "color_obj";
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

const char* particles1 = "particles1";
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
const char* particles2 = "particles2";
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

const char* modeling = "modeling";
const char* modeling_vs=R"glsl(
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textCoord;
layout(location = 2) in vec3 normal;
out vec3 FragPos;
out vec2 TextCoord;
out vec3 FragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
	FragPos = vec3(model * vec4(position, 1.0)); // 在世界坐标系中指定片元位置
	TextCoord = textCoord;
	mat3 normalMatrix = mat3(transpose(inverse(model)));
	FragNormal = normalMatrix * normal; // 计算法向量经过模型变换后值
}
)glsl";
const char* modeling_fs=R"glsl(
#version 330 core

in vec3 FragPos;
in vec2 TextCoord;
in vec3 FragNormal;

// 光源属性结构体
struct LightAttr
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;	// 衰减常数
	float linear;   // 衰减一次系数
	float quadratic; // 衰减二次系数
};

uniform LightAttr light;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

out vec4 color;

void main()
{
	// 环境光成分
	vec3	ambient = light.ambient * vec3(texture(texture_diffuse0, TextCoord));

	// 漫反射光成分 此时需要光线方向为指向光源
	vec3	lightDir = normalize(light.position - FragPos);
	vec3	normal = normalize(FragNormal);
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * light.diffuse * vec3(texture(texture_diffuse0, TextCoord));

	// 镜面反射成分 此时需要光线方向为由光源指出
	float	specularStrength = 0.5f;
	vec3	reflectDir = normalize(reflect(-lightDir, normal));
	vec3	viewDir = normalize(viewPos - FragPos);
	float	specFactor = pow(max(dot(reflectDir, viewDir), 0.0), 64.0f);
	vec3	specular = specFactor * light.specular * vec3(texture(texture_specular0, TextCoord));

	// 计算衰减因子
	float distance = length(light.position - FragPos); // 在世界坐标系中计算距离
	float attenuation = 1.0f / (light.constant 
			+ light.linear * distance
			+ light.quadratic * distance * distance);

	vec3	result = (ambient + diffuse + specular) * attenuation;
	color	= vec4(result , 1.0f);
}
)glsl";