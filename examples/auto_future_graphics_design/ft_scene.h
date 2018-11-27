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
		struct intl_pt
		{
			float _sizew=800.f,_sizeh=600.f;
			float _bkr = 0.2f, _bkg = 0.2f, _bkb = 0.5f, _bka = 0.5f;
			intl_pt()
			{}
		};
		intl_pt _sn_pt;
		//frame buffer
		unsigned int _fboId;
		unsigned int _colorTextId;
		unsigned int _depthStencilTextId;
	public:
		ft_scene();
		~ft_scene();
		void draw();
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int len = sizeof(_sn_pt);
			vplist.emplace_back(&_sn_pt, len);
			len += plen;
			return len;

		}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page(int property_part = -1);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
		bool handle_mouse();
	};

	REGISTER_CONTROL(ft_scene)
}