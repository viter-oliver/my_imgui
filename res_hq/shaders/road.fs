precision mediump float;
varying vec2 Textcoord;
varying vec2 posx;
uniform float bdw;
uniform int lhide,rhide;
uniform sampler2D llane;
uniform sampler2D rlane;
//uniform sampler2D road;

void main()
{
   float x=posx.x;
   if(x<=1.0)
   {
        if(lhide!=0)
       	 discard;
        else
			gl_FragColor = texture2D(llane, Textcoord);
   }
   else if(x>=2.0)
   {
         if(rhide!=0)
         	discard;
         else  
             gl_FragColor = texture2D(rlane, Textcoord);
    }
    else
	{
		discard;// gl_FragColor = vec4(0,0,0,0);//texture2D(road, Textcoord);
	}
}