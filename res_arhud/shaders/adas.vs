attribute vec2 position;
attribute vec2 textcoord;
varying vec2 Textcoord;
uniform mat4 ProjMtx;

void main()
{
    gl_Position = ProjMtx * vec4(position, 0.0, 1.0);
    Textcoord = textcoord;
}