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
	//gl_Position =  vec4(position, 1.0);
}
