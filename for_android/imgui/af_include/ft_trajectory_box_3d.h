#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_trajectory_box_3d :
		public ft_base
	{
		ImVector<ImVec4>  _path;
		vector<ImVector<ImDrawVert>> _components;
	public:
		void draw();
#if !defined(DISABLE_DEMO)
#endif
		bool handle_mouse();
	};

	REGISTER_CONTROL(ft_trajectory_box_3d)
}