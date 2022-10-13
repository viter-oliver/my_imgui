#pragma once
#include "ft_base.h"

class project_edit
{
	base_ui_component& _root;
	base_ui_component* _pcurrent_object;
	base_ui_component* _pcopy_object;
     base_ui_component* _pcut_object;
     bool _trigger_focus_switch = { false };
	void view_object(base_ui_component& fb);
	void add_model();
public:
	project_edit(base_ui_component& fb) 
          :_root(fb), _pcurrent_object(nullptr), _pcopy_object(nullptr), _pcut_object(nullptr)
	{}
	void objects_view();

     void move_item_pre();
     void move_item_next();
     void copy_item();
     void cut_item();
     void past_item();
     void add_item();
     void insert_item();
     void add_sibling();
     void delete_item();
	void popup_context_menu();
     void clear_sel_item();
     void trigger_focus_switch()
     {
          _trigger_focus_switch = true;
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
