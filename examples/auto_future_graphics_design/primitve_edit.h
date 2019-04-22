#pragma once
#include "af_primitive_object.h"
class primitve_edit
{
	shared_ptr<primitive_object> _pmobj;
	vector<float> _vertex_buff;
	float canvas_w{ 800.0 };
	float canvas_h{ 800.0 };
public:
	void draw_primitive_list();
	void draw_primitive_item_property();
};

