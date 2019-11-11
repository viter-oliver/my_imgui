#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
namespace auto_future
{
	class screen_image_distortion
	{
		unsigned int _fboId;
		unsigned int _colorTextId;
		unsigned int _depthStencilTextId;
		float _width, _height;
	public:
		screen_image_distortion(float width, float height);
		~screen_image_distortion();
		void get_size(float& width, float& height)
		{
			width = _width;
			height =_height;
		}
		void bind_framebuffer();
		void disbind_framebuffer();
	};

}