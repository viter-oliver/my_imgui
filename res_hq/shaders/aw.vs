#version 120
attribute vec3 aposition;
attribute vec2 btextcoord;
attribute vec3 anormal;
varying vec2 Textcoord;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
void main()
{
    vec4 vpos= proj * view * model* vec4(aposition, 1.0);
    vec3 anm=anormal;
     float dtt=length(anm);
    gl_Position=dtt*vpos;

    Textcoord = btextcoord;
}
