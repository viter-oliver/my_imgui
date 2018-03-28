#pragma once
#include "control_common_def.h"
class ui_assembler
{
	base_ui_component& _root;
public:
	ui_assembler(base_ui_component& root):_root(root){}
	~ui_assembler(){}
	bool load_ui_component_from_file(const char* file_path);
	bool output_ui_component_to_file(const char* file_path);
};

