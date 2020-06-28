#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "af_shader.h"
namespace auto_future
{
	class screen_image_distortion
	{
          GLuint g_VboHandle;
		GLuint _fboId;
		GLint _prev_fbo;
		GLuint _colorTextId;
		GLuint _depthStencilTextId;
		float _width, _height;
		GLuint _txtDistortion;
		ps_shader _pshader;
	public:
		screen_image_distortion(float width, float height);
		~screen_image_distortion();
		void get_size(float& width, float& height)
		{
			width = _width;
			height =_height;
		}
		void bind_framebuffer();
		void draw();
		void disbind_framebuffer();
	};

}