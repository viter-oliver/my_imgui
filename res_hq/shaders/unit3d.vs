#version 300 es
attribute vec3 aposition;
attribute vec2 btextcoord;
attribute vec3 anormal;
varying vec2 Textcoord;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
out vec3 outValue;
void main()
{
	float nml=anormal.z;
    vec4 vpos= proj * view * model* vec4(aposition, 1.0);
    gl_Position=vpos;
    outValue=vpos.xyz;
    Textcoord = btextcoord*nml;
}
