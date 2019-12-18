#pragma once
#include "ft_base.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
namespace auto_future
{
	class AFG_EXPORT ft_scene :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(intl_pt,_sn_pt,
			(af_vec4, _bk_clr))
		//frame buffer
		unsigned int _fboId={0};
		unsigned int _colorTextId;
		unsigned int _depthStencilTextId;
		void release_resource();
	public:
		ft_scene();
		~ft_scene();
		void link();
          void draw_frames();
		bool handle_mouse();
	};

	REGISTER_CONTROL(ft_scene)
}