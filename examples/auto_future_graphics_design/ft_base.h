#pragma once
#include "factory.h"
namespace auto_future
{
	class AFG_EXPORT ft_base :
		public base_ui_component
	{
	public:
		ft_base(); 
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	private:
		enum { en_pt_name, en_pt_pos_x, en_pt_pos_y, en_pt_visible };
	public:
		base_ui_component* get_hit_ui_object(float posx, float posy);
#endif
		bool handle_mouse();
		base_ui_component* get_copy_of_object();
		bool contains(float posx, float posy);
		bool relative_contain(af_vec2& point);
	};
	/**
	*@brief copy the list of the property data block of a ui component object to another
	*@param vdest destination
	*@param vsource source
	-#vdest and vsource must have same size
	*/
	void property_copy(vproperty_list& vdest, vproperty_list& vsource);
	base_ui_component* get_copy_of_object(base_ui_component* byobject);
	REGISTER_CONTROL(ft_base)
}