layout(location = 0)attribute vec2 position;
layout(location = 1)attribute vec2 textcoord;

varying vec2 Textcoord;
//uniform mat4 proj;
//uniform mat4 view;
uniform mat4 model;
void main()
{
    gl_Position = /*proj * view */ model * vec4(position,0.0, 1.0);
    Textcoord = textcoord;
}
