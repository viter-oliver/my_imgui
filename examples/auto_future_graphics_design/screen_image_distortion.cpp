#include "screen_image_distortion.h"
#include "common_functions.h"
namespace auto_future
{
	GLfloat _vertices[] = {
		-1.f, -1.f,0.f,0.f,
		1.f, -1.f, 1.f,0.f,
		-1.f, 1.f, 0.f,1.f,
		1.f, 1.f, 1.f,1.f,
	};
	screen_image_distortion::screen_image_distortion(float width, float height)
		:_width(width), _height(height)
	{
		prepareFBO1(_colorTextId, _depthStencilTextId, _fboId, _width, _height);

	}

	screen_image_distortion::~screen_image_distortion()
	{
		glDeleteTextures(1, &_colorTextId);
		glDeleteTextures(1, &_depthStencilTextId);
		glDeleteFramebuffers(1, &_fboId);
	}

	void screen_image_distortion::bind_framebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
		glViewport(0, 0, _width, _height);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void screen_image_distortion::disbind_framebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}