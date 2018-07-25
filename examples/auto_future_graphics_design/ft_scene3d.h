#pragma once
#include "ft_base.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
namespace auto_future
{
	class AFG_EXPORT ft_scene3d :
		public ft_base
	{
		struct intl_pt
		{
			ImVec2 _size;
			ImVec4 _back_color;
			intl_pt()
				:_size(800.f, 600.f)
				, _back_color(0.2f, 0.2f, 0.5f, 0.5f)
			{}
		};
		intl_pt _sn_pt;
		//frame buffer
		unsigned int _fboId;
		unsigned int _colorTextId;
		unsigned int _depthStencilTextId;
	public:
		ft_scene3d();
		~ft_scene3d();
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
		void draw_peroperty_page();
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
		bool handle_mouse();
	};

	REGISTER_CONTROL(ft_scene3d)
}