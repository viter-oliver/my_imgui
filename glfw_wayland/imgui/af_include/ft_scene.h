#pragma once
#include "ft_base.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace auto_future
{
	class AFG_EXPORT ft_scene :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(intl_pt,_sn_pt,
			(af_vec4, _bk_clr))
		//frame buffer
		unsigned int _fboId;
		unsigned int _colorTextId;
		unsigned int _depthStencilTextId;
	public:
		ft_scene();
		~ft_scene();
		void draw();
		bool handle_mouse();
	};

	REGISTER_CONTROL(ft_scene)
}