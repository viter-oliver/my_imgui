varying vec3 Color;
varying vec2 Texcoord;
//out vec4 outColor;
//uniform sampler2D texPuppy;
void main()
{
	//outColor = texture(texPuppy, Texcoord);
	gl_FragColor = vec4(Color, 0.0);
}
