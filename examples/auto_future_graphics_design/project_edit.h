#pragma once
#include "ft_base.h"
class project_edit
{
	base_ui_component& _root;
	base_ui_component* _pcurrent_object;
	base_ui_component* _pcopy_object;
     base_ui_component* _pcut_object;

	void view_object(base_ui_component& fb);
public:
	project_edit(base_ui_component& fb) 
          :_root(fb), _pcurrent_object(nullptr), _pcopy_object(nullptr), _pcut_object(nullptr)
	{}
	void objects_view();
	void popup_context_menu();
	void clear_sel_item(){ 
		if (_pcurrent_object)
		{
			_pcurrent_object->set_selected(false);
		}
		_pcurrent_object = nullptr;
		_pcopy_object = nullptr;
	}
	void sel_ui_component(base_ui_component* ui_target){
		assert(ui_target);
		if (_pcurrent_object)
		{
			_pcurrent_object->set_selected(false);
		}
		ui_target->set_selected(true);
		_pcurrent_object = ui_target;
	}
	base_ui_component* current_object(){ return _pcurrent_object; }
};
