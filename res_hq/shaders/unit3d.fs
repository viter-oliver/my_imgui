#version 300 es
precision mediump float;
varying vec2 Textcoord;
uniform sampler2D text;
void main()
{
   gl_FragColor =texture2D(text, Textcoord);
}

