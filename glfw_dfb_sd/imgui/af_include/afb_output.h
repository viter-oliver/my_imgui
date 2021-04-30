#pragma once
#include "control_common_def.h"
using namespace auto_future;
class afb_output
{
	base_ui_component& _pj;
public:
	afb_output(base_ui_component&pr_root) :_pj(pr_root){}
	~afb_output(){}
	void output_afb(const char* afb_file);
};

