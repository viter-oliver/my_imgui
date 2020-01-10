#pragma once
#include "af_model.h"
using namespace std;
class model_edit
{
	shared_ptr<af_model> _pmodel;
	string _key_name;
public:
	model_edit();
	~model_edit();
	void draw_model_list();
	void draw_model_item_property();
     void clear_states();
};
bool import_models(const char* md_file_name);
