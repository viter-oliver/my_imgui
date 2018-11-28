#pragma once
#include "factory.h"
namespace auto_future
{
	class AFG_EXPORT ft_base :
		public base_ui_component
	{
	public:
		ft_base() :base_ui_component(){}

		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	private:
		base_prop _in_p_bk;
		enum { en_pt_name, en_pt_pos_x, en_pt_pos_y, en_pt_visible };
	public:
		void draw_peroperty_page(int property_part=-1);
		void back_up_property();
		void execute_command(command_elemment& ele_cmd);
		command_elemment clone_cmd_ele(command_elemment&ele_cmd);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
		bool handle_mouse();
		base_ui_component* get_copy_of_object();
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