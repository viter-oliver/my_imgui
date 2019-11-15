
attribute vec3 position;
attribute vec3 normal;
attribute vec2 textCoord;
varying vec3 FragPos;
varying vec2 TextCoord;
varying vec3 FragNormal;

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
