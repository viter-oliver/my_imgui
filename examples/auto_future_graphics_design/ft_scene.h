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
			(float, _sizew, {800.f}),
			(float, _sizeh, {600.f}), 
			(af_vec4, _bk_clr))
		//frame buffer
		unsigned int _fboId;
		unsigned int _colorTextId;
		unsigned int _depthStencilTextId;
	public:
		ft_scene();
		~ft_scene();
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		base_ui_component* get_hit_ui_object(float posx, float posy);
#endif
		bool handle_mouse();
	};

	REGISTER_CONTROL(ft_scene)
}