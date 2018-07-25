#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_trajectory_box :
		public ft_base
	{
		ImVector<ImVec4>  _path;
		vector<ImVector<ImDrawVert>> _components;
	public:
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page();
		bool init_from_json(Value& jvalue);
#endif
		bool handle_mouse();
	};

	REGISTER_CONTROL(ft_trajectory_box)
}