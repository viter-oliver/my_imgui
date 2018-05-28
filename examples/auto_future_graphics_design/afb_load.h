#pragma once
#include "control_common_def.h"
class afb_load
{
	base_ui_component*& _pj;
public:
	afb_load(base_ui_component*&pr_root) :_pj(pr_root){}
	~afb_load(){}
	
	void load_afb(const char* afb_file);
};

