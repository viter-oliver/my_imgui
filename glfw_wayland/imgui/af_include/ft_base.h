#pragma once
#include "af_factory.h"
namespace auto_future
{
	enum anchor_type
	{
		en_anchor_top_left,
		en_anchor_top_right,
		en_anchor_bottom_right,
		en_anchor_bottom_left,
		en_anchor_center,
	};
    #define MAX_CONTENT_LEN 0x100

	class AFG_EXPORT ft_base :
		public base_ui_component
	{
	public:
		ft_base(); 
		void draw();
		void draw_frames();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	private:
		enum { en_pt_name, en_pt_pos_x, en_pt_pos_y, en_pt_visible };
		bool _keep_scale{false};
		float _w2h{1.};
#endif
	public:
		base_ui_component* get_hit_ui_object(float posx, float posy);
		bool handle_mouse();
		base_ui_component* get_copy_of_object();
		bool contains(float posx, float posy);
		bool relative_contain(af_vec2& point);
		bool relative_contain(float pos, bool be_h);
		bool set_prop_fd_value(int pg_id, int fd_id,void* pvalue);
	};
	REGISTER_CONTROL(ft_base)

	/**
	*@brief copy the list of the property data block of a ui component object to another
	*@param vdest destination
	*@param vsource source
	-#vdest and vsource must have same size
	*/
	void property_copy(vproperty_list& vdest, vproperty_list& vsource);
     AFG_EXPORT  base_ui_component*  get_copy_of_object( base_ui_component* byobject );
	void get_uic_path(base_ui_component* pobj, string& path_rtn);
}
