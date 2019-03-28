#pragma once
#include "af_primitive_object.h"
class primitve_edit
{
	shared_ptr<primitive_object> _pmobj;
public:
	primitve_edit()
	{
		_pmobj = make_shared<primitive_object>();
	}
	void draw_primitive_list();
	void draw_primitive_item_property();
};

