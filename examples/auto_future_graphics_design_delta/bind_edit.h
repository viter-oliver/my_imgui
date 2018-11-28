#pragma once
#include "ft_base.h"
class bind_edit
{
	base_ui_component& _root;
public:
	bind_edit(base_ui_component& fb)
		:_root(fb)
	{}
	void bind_source_view();
	
};
