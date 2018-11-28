#pragma once
#include "ft_base.h"
class project_edit
{
	base_ui_component& _root;
	base_ui_component* _pcurrent_object;
	base_ui_component* _pcopy_object;
	void view_object(base_ui_component& fb);
public:
	project_edit(base_ui_component& fb) 
		:_root(fb), _pcurrent_object(NULL), _pcopy_object(NULL)
	{}
	void objects_view();
	void popup_context_menu();
	void clear_sel_item(){ _pcopy_object = NULL; }
	base_ui_component* current_object(){ return _pcurrent_object; }
};
