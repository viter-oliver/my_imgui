#version 120
precision mediump float;
attribute vec3 position;
attribute vec2 textcoord;
attribute vec3 normal;
varying vec2 Textcoord;
varying vec2 pos;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
void main()
{
    pos=normal.xy;
   vec3 bpos=position;
    
    vec4 vpos= proj * view * model* vec4(bpos, 1.0);
    gl_Position=vpos;
   Textcoord=textcoord;
}