#include "screen_image_distortion.h"
#include "common_functions.h"
#include "res_output.h"
namespace auto_future
{
#if 1
	const char* vs_code = R"glsl(
	   attribute vec2 position;
        //attribute vec2 txtcoord;
        varying vec2 outTxtCd;
        void main()
        {
             outTxtCd=(position+vec2(1,1))*0.5;
             gl_Position=vec4(position,0.0,1.0);
        }
		)glsl";
     const char* fs_code = R"glsl(
        varying vec2 outTxtCd;
        uniform sampler2D text;
        //uniform sampler2D txtDistortion;
        void main()
        {
            //vec2 txtMap = texture(txtDistortion, outTxtCd);
            gl_FragColor = texture(text, outTxtCd);

        }
		)glsl";
#else
	// Shader sources
	const GLchar* vs_code = R"glsl(
    #version 150 core
    in vec2 position;
    in vec2 texcoord;
    out vec2 Texcoord;
    void main()
    {
        Texcoord = texcoord;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";
	const GLchar* fs_code = R"glsl(
    #version 150 core
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D text;
    //uniform sampler2D texPuppy;
    void main()
    {
        outColor = vec4(1,0,0,1);// texture(text, Texcoord);// mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);
    }
)glsl";
	GLfloat _vertices[] = {
		-1.f, -1.f,
		1.f, -1.f,
		-1.f, 1.f,
		1.f, 1.f, 
	};
	GLfloat _txtcoords[] = {
		0.f,1.f,
		1.f,1.f, 
		0.f,0.f,
		1.f,0.f,
	};
#endif



     GLfloat _plain_vertices[] = {
          -1.f, -1.f, 
          1.f, -1.f, 
          -1.f, 1.f,
          1.f, 1.0f, 
     };
	screen_image_distortion::screen_image_distortion(float width, float height)
		:_width(width), _height(height)
	{
#if 0
          glGenBuffers(2, _vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices)*sizeof(GLfloat), _vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_txtcoords)*sizeof(GLfloat), _txtcoords, GL_STATIC_DRAW);
          

		_pshader = make_shared<af_shader>(vs_code, fs_code);
		_pshader->use();
		auto pmid = _pshader->program_id();
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
		GLint posAttrib = glGetAttribLocation(pmid, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);
		GLint texAttrib = glGetAttribLocation(pmid, "texcoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
#else
          _pshader = make_shared<af_shader>( vs_code, fs_code );
          _pshader->use();
          auto pmid = _pshader->program_id();
          glGenBuffers( 1, &g_VboHandle );
          glBindBuffer( GL_ARRAY_BUFFER, g_VboHandle );
          glBufferData( GL_ARRAY_BUFFER, sizeof( _plain_vertices ), _plain_vertices, GL_STATIC_DRAW );
          GLint posAttrib = glGetAttribLocation( pmid, "position" );
          glEnableVertexAttribArray( posAttrib );

          glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE,0, (GLvoid*)0 );
#endif
		prepareFBO1(_colorTextId, _depthStencilTextId, _fboId, _width, _height);

	}

	screen_image_distortion::~screen_image_distortion()
	{
          glDeleteBuffers( 1, &g_VboHandle );
		glDeleteTextures(1, &_colorTextId);
		glDeleteTextures(1, &_depthStencilTextId);
		glDeleteFramebuffers(1, &_fboId);
	}

	void screen_image_distortion::bind_framebuffer()
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING,&_prev_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
		//glViewport(0, 0, _width, _height);
		//glClearColor(1, 1,1, 1);
		//glClear(GL_COLOR_BUFFER_BIT);
	}
	void screen_image_distortion::draw()
	{
		glViewport(0, 0, _width, _height);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		_pshader->use();
		glActiveTexture(GL_TEXTURE0);
		/*glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
*/		
		auto pmid = _pshader->program_id();
#if 0
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
		GLint posAttrib = glGetAttribLocation(pmid, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);
		GLint texAttrib = glGetAttribLocation(pmid, "texcoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
#else

#endif
          glBindBuffer( GL_ARRAY_BUFFER, g_VboHandle );
          glBufferData( GL_ARRAY_BUFFER, sizeof( _plain_vertices ), _plain_vertices, GL_STATIC_DRAW );
          GLint posAttrib = glGetAttribLocation( pmid, "position" );

          glEnableVertexAttribArray( posAttrib );

          glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0 );
		//auto txtid = g_mtexture_list["road.png"]->_atxt_id;
		glBindTexture(GL_TEXTURE_2D,  _colorTextId);
		//_pshader->uniform("text", 0);
	
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	void screen_image_distortion::disbind_framebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _prev_fbo);
	}

}