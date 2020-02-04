//precision mediump float;
varying vec2 Textcoord;
uniform sampler2D text;
uniform float alpha;
void main()
{
	vec4 fcolor=texture2D(text, Textcoord);
     fcolor.a=alpha*fcolor.a;
   gl_FragColor =fcolor;// texture2D(text, Textcoord);
}

