#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; 

// Output data ; will be interpolated for each fragment.
out vec2 UV;
//out vec4 particlecolor;
//in int gggid;
// Values that stay constant for the whole mesh.
uniform vec2 uvcol[120];
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)
void main()
{
	float particleSize = xyzs.w;
	int uv_index = int(particleSize); // because we encoded it this way.
	uv_index=uv_index*4;
	//gggid=gl_VertexID;
	uv_index=uv_index+gl_VertexID%4;
	particleSize=particleSize/20;
	vec3 particleCenter_wordspace = xyzs.xyz;
	
	vec3 vertexPosition_worldspace = 
		particleCenter_wordspace
		+ CameraRight_worldspace * squareVertices.x * particleSize
		+ CameraUp_worldspace * squareVertices.y * particleSize;

	// Output position of the vertex
	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

	// UV of the vertex. No special space for this one.
	UV = uvcol[uv_index];
	//UV = squareVertices.xy + vec2(0.5, 0.5);
	//particlecolor = color;
}

