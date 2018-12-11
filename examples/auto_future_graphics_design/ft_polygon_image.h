#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_polygon_image :
		public ft_base
	{
		ImVector<ft_vertex>  _vertexes;
	public:
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		bool init_from_json(Value& jvalue);
#endif
		bool handle_mouse();
	};
	REGISTER_CONTROL(ft_polygon_image)
}