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
