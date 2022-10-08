#pragma once
#include "af_primitive_object.h"
class primitve_edit
{
	shared_ptr<primitive_object> _pmobj;
	string _key_name;
	vector<float> _vertex_buff;
	float* _pvertex;
	float canvas_w{ 800.0 };
	float canvas_h{ 800.0 };
public:
	void draw_primitive_list();
	void draw_primitive_item_property();
    void clear_states();
	string get_cur_key(){
		return _key_name;
	}
};

