#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
extern "C" {
#include "image_DXT.h"
}
#include "SOIL.h"
#include <vector>
#include <fstream>
#ifndef GL_COMPRESSED_RGBA8_ETC2_EAC
#define GL_COMPRESSED_RGBA8_ETC2_EAC      0x9278
#endif
const int WWIDTH = 720, WHEIGHT = 720;

const GLchar *vertex_shader =
        "uniform mat4 ProjMtx;\n"
        "attribute vec2 Position;\n"
        "attribute vec2 UV;\n"
        "varying vec2 Frag_UV;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

const GLchar* fragment_shader =
    "uniform sampler2D Texture;\n"
    "varying vec2 Frag_UV;\n"
    "void main()\n"
    "{\n"
    "	gl_FragColor =  texture2D(Texture, Frag_UV);\n"
    "}\n";
float ortho_projection[4][4] =
{
    { 1.f,                  0.f,                    0.0f, 0.0f },
    { 0.f,                  1.f,                    0.0f, 0.0f },
    { 0.f,                  0.f,                    1.0f, 0.0f },
    { 0.f,                  0.f,                    0.0f, 1.0f },
};
void mtrans(float xoff,float yoff,float zoff){
	ortho_projection[3][0]=xoff;
	ortho_projection[3][1]=yoff;
	ortho_projection[3][2]=zoff;

}
void mscale(float xs,float ys,float zs){
	ortho_projection[0][0]=xs;
	ortho_projection[1][1]=ys;
	ortho_projection[2][2]=zs;
}

GLfloat _plain_vertices[] = {
	-0.5f, -0.5f,0.f,1.f,
	0.5f, -0.5f, 1.f,1.f,
	-0.5f, 0.5f,0.f,0.f,
	0.5f, 0.5f, 1.f,0.f,
};
static GLuint          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static GLuint          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static GLuint          g_AttribLocationPosition = 0, g_AttribLocationUV = 0;
static GLuint          g_VboHandle = 0;
static GLuint        g_Texture0 = 0,g_Texture1=0;
void init_basic_ogl_objects(){

    g_ShaderHandle = glCreateProgram();
    g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
    glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(g_VertHandle);
    glCompileShader(g_FragHandle);
    glAttachShader(g_ShaderHandle, g_VertHandle);
    glAttachShader(g_ShaderHandle, g_FragHandle);
    glLinkProgram(g_ShaderHandle);

    g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
    g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");

    g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
    g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");

    glGenBuffers(1, &g_VboHandle);

    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_plain_vertices), _plain_vertices, GL_STATIC_DRAW);
	
    glEnableVertexAttribArray(g_AttribLocationPosition);
    glEnableVertexAttribArray(g_AttribLocationUV);

    glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, (GLvoid*)0);
    glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, (GLvoid*)(2 * sizeof(GL_FLOAT)));
}
using namespace std;
enum param_pos
{
	en_exe_pos,
	en_img_pos,
	en_opt_pos,
	en_width_pos,
	en_height_pos,
	en_param_cnt,
};
int main(int argc, char *argv[])
{
	if(argc<en_param_cnt)
	{
		fprintf(stderr,"Usage: test_texture  imagefile [OPTIONS]\n");
		fprintf(stderr,"-d\t draw the image simply.\n");		
		fprintf(stderr,"-dxt\tdraw the image by dxt5 compression format.\n");
		fprintf(stderr,"-astc \tdraw the image by astc compression format.\n");
		fprintf(stderr,"-etc2 \tdraw the image by etc2 compression format.\n");
		return 0;
	}
	glfwInit();
	
	glfwWindowHint(GLFW_CLIENT_API,GLFW_OPENGL_ES_API);
#ifdef GLFW_INCLUDE_ES2
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#endif
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(WWIDTH, WHEIGHT, "OpenGL", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	init_basic_ogl_objects();
	if(!strcmp(argv[en_opt_pos],"-d")||!strcmp(argv[en_opt_pos],"-dxt")){
		int width, height, channels;
		GLubyte* imgdata =SOIL_load_image(argv[1], &width, &height, &channels, SOIL_LOAD_RGBA);
		glGenTextures(1, &g_Texture0);
		glBindTexture(GL_TEXTURE_2D, g_Texture0);				

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		// Step2 设定wrap参数				
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);				
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);				
		// Step3 设定filter参数				
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);				
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,	GL_LINEAR);
		// Step4 加载纹理				
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);	
		if(!strcmp(argv[en_opt_pos],"-dxt")){//"-d"

			glGenTextures(1, &g_Texture1);
			glBindTexture(GL_TEXTURE_2D, g_Texture1);				

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			// Step2 设定wrap参数				
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);				
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);				
			// Step3 设定filter参数				
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);				
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,	GL_LINEAR);
			int DDS_size;
			unsigned char *DDS_data = NULL;
			unsigned int cpcode=0;
			if( (channels & 1) == 1 )
			{
				/*	RGB, use DXT1	*/
				DDS_data = convert_image_to_DXT1( imgdata, width, height, channels, &DDS_size );
				cpcode=GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			} else
			{
				/*	RGBA, use DXT5	*/
				DDS_data = convert_image_to_DXT5( imgdata, width, height, channels, &DDS_size );
				cpcode=GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				printf("width=%d,height=%d,rawdata size=%d,dds size=%d\n",width,height,width*height*4,DDS_size);
			}
			if( DDS_data )
			{
			      glCompressedTexImage2D(GL_TEXTURE_2D, 0, cpcode, width, height, 0, DDS_size, DDS_data);

				SOIL_free_image_data( DDS_data );
				/*	printf( "Internal DXT compressor\n" );	*/
			} 
			
		}
		// Step5 释放纹理图片资源				
		SOIL_free_image_data(imgdata);
	}
	else{
		unsigned int iwidth=0;
		unsigned int iheight=0;
		
		ifstream ifs(argv[1], ios::in |ios::binary);
		if(!ifs){
			fprintf(stderr,"invalid compressed image file:%s\n",argv[1]);
			return 0;
		}
		auto file_bigin=ifs.tellg();
		ifs.seekg(0, ios::end);
		unsigned int sz_code = ifs.tellg() - file_bigin;
		ifs.seekg(0, ios::beg);
		vector<unsigned char> vbytes;
		vbytes.reserve(sz_code);
		vbytes.resize(sz_code);
		//pbuf->sgetn((char*)&vbytes[0], sz_code);
		ifs.read((char*)&vbytes[0],sz_code);
		ifs.close();
		glGenTextures(1, &g_Texture0);
		glBindTexture(GL_TEXTURE_2D, g_Texture0);				

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Step2 设定wrap参数				
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);				
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);				
		// Step3 设定filter参数				
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);				
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,	GL_LINEAR);
		if(!strcmp(argv[en_opt_pos],"-astc")){
			unsigned int astc_code_unde10[]=
			{
			GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
			GL_COMPRESSED_RGBA_ASTC_5x4_KHR,
			GL_COMPRESSED_RGBA_ASTC_5x5_KHR,
			GL_COMPRESSED_RGBA_ASTC_6x5_KHR,
			GL_COMPRESSED_RGBA_ASTC_6x6_KHR,
			GL_COMPRESSED_RGBA_ASTC_8x5_KHR,
			GL_COMPRESSED_RGBA_ASTC_8x6_KHR,0,
			GL_COMPRESSED_RGBA_ASTC_8x8_KHR,
			};
			unsigned int astc_code_above10[]={
			GL_COMPRESSED_RGBA_ASTC_10x5_KHR,
			GL_COMPRESSED_RGBA_ASTC_10x6_KHR,0,
			GL_COMPRESSED_RGBA_ASTC_10x8_KHR,0,
			GL_COMPRESSED_RGBA_ASTC_10x10_KHR,0,
			GL_COMPRESSED_RGBA_ASTC_12x10_KHR,0,
			GL_COMPRESSED_RGBA_ASTC_12x12_KHR,
			};
/*			unsigned int sastc_code_under10[]={
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,0,
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,
			};
			unsigned int sastc_code_above10[]={
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,0,
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,0,
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,0,
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,0
			GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,
			};
*/
			unsigned char block_dimx=vbytes[4];
			unsigned char block_dimy=vbytes[5];
			unsigned int compress_code=0;
			unsigned short block_index=block_dimx+block_dimy;
			printf("block_dimx=%d,block_dimy=%dblock_index=%d\n",block_dimx,block_dimy,block_index);
			for(int ix=0;ix<0x10;ix++)
				printf("vbytes[%d]=0x%x\n",ix,vbytes[ix]);
			if(block_dimx<10)
			{
			      block_index-=8;
				compress_code=astc_code_unde10[block_index];
			}
			else{
				block_index-=15;
				compress_code=astc_code_above10[block_index];
			}
			if(!compress_code){
				fprintf(stderr,"invalid dimx:%d or dimy:%d\n",block_dimx,block_dimy);
				return 0;
			}

			iwidth=*(unsigned int*)(&vbytes[7]);
			*((unsigned char*)(&iwidth)+3)=0;
			iheight=*(unsigned int*)(&vbytes[10]);
			printf("iheight=%d\n",iheight);
			
			*((unsigned char*)(&iheight)+3)=0;
			printf("iwidth=%d,iheight=%d compress_code=0x%x sz_code=%d\n",iwidth,iheight,compress_code,sz_code);
			unsigned int offset=(unsigned int)(sz_code-16);
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, compress_code, iwidth, iwidth, 0, offset, &vbytes[16]);
		}
		else 
		if(!strcmp(argv[en_opt_pos],"-etc2")){
			for(int ix=0xc;ix<0x10;ix++){
				printf("vbytes[0x%x]=0x%2x\n",ix,vbytes[ix]);

			}
			iwidth=vbytes[0xc]*0x100+vbytes[0xd];
			iheight=vbytes[0xe]*0x100+vbytes[0xf];
			
			printf("etc2 iwidth=%d,iheight=%d\n",iwidth,iheight);
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA8_ETC2_EAC, iwidth, iwidth, 0, sz_code-16, &vbytes[16]);

		}
		else
		if(!strcmp(argv[en_opt_pos],"-etc1")){
			iwidth=vbytes[0xc]*0x100+vbytes[0xd];
			iheight=vbytes[0xe]*0x100+vbytes[0xf];
			glDisable(GL_BLEND);
			printf("etc1 iwidth=%d,iheight=%d\n",iwidth,iheight);
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, iwidth, iwidth, 0, sz_code-16, &vbytes[16]);
		}
		else
		if(!strcmp(argv[en_opt_pos],"-raw")){
			iwidth=atoi(argv[en_width_pos]);
			iheight=atoi(argv[en_height_pos]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iwidth,iheight,0, GL_RGBA, GL_UNSIGNED_BYTE, &vbytes[0]);	

		}
			
		glBindTexture(GL_TEXTURE_2D, 0);		
		
	}
	//glPointParameterfARB(
	printf("g_Texture1=%d\n",g_Texture1);
	glViewport(0,0,WWIDTH,WHEIGHT);
	while(!glfwWindowShouldClose(window))
	{
		//glfwPollEvents();
		glClearColor(0.18f, 0.18f, 0.18f, 1.0f);		
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(g_ShaderHandle);
		glActiveTexture(GL_TEXTURE0);		
		glBindTexture(GL_TEXTURE_2D, g_Texture0);
		if(g_Texture1==0){
			glUniformMatrix4fv(g_AttribLocationProjMtx,1,GL_FALSE,&ortho_projection[0][0]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
		else{
			mtrans(-0.5f,0.f,0.f);
			glUniformMatrix4fv(g_AttribLocationProjMtx,1,GL_FALSE,&ortho_projection[0][0]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			mtrans(0.5f,0.f,0.f);
			glActiveTexture(GL_TEXTURE0);		
			glBindTexture(GL_TEXTURE_2D, g_Texture1);
			glUniformMatrix4fv(g_AttribLocationProjMtx,1,GL_FALSE,&ortho_projection[0][0]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			//GL_POINT;
		}	
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
    return 0;
}
