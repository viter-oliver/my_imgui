#pragma once
#include "af_state_manager.h"
class state_manager_edit
{
	ps_state_manager _psel{nullptr};
public:
	state_manager_edit();
	~state_manager_edit();
	void view_state_managers();
	void view_state_manager_item_property();
};

