#version 120
precision mediump float;
attribute vec3 aposition;
attribute vec2 btextcoord;
attribute vec3 anormal;
varying vec2 Textcoord;
varying vec2 posx;
uniform float base_y;
uniform float lane_w;
uniform float vbdw;
uniform float lc2,lc1,lc0,rc2,rc1,rc0;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
void main()
{
	float nml=anormal.z;
    float z=aposition.y-base_y;
    posx=anormal.xy;
   vec3 bpos=aposition;
     float ln=anormal.x;
    if(bpos.x<0.00001)//left
    {
    	float dx=ln;
        if(dx<=0.1)
        {
			bpos.x=lc2*z*z+lc1*z+lc0;
        }
        else
        {
            bpos.x=lc2*z*z+lc1*z+lc0+lane_w;
        }
    }
   else
   {
		//float dx=posx.x-vbdw;
        if(ln>=2.9)
        {
        	bpos.x=rc2*z*z+rc1*z+rc0;
        }
        else
        {
			bpos.x=rc2*z*z+rc1*z+rc0-lane_w;
        }
   }
    
    vec4 vpos= proj * view * model* vec4(bpos, 1.0);
    gl_Position=vpos;
    Textcoord = btextcoord*nml;
}