#version 300 es
in vec3 aposition;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
out vec3 outValue;
void main()
{
    
   
     vec4 vpos= proj * view * model* vec4(aposition, 1.0);
     outValue=vpos.xyz;
  
}
