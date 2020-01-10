
varying vec2 Textcoord;
varying vec2 pos;
uniform float level;
uniform sampler2D text;
void main()
{
    float y=pos.y;
	if(y>level)
    {
		discard;
    }
    else
    {
       gl_FragColor = texture(text, Textcoord);
    }
}
