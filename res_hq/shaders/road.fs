precision mediump float;
varying vec2 Textcoord;
varying vec2 posx;
uniform float bdw;
uniform sampler2D llane;
uniform sampler2D rlane;
uniform sampler2D road;

void main()
{
   float x=posx.x;
   if(x<=1.0)
   {
		gl_FragColor = texture2D(llane, Textcoord);
   }
   else if(x>=2.0)
   {
         gl_FragColor = texture2D(rlane, Textcoord);
    }
    else
	{
		 gl_FragColor = texture2D(road, Textcoord);
	}
}