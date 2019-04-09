#pragma once
#include "af_model.h"
using namespace std;
class model_edit
{
	shared_ptr<af_model> _pmodel;
public:
	model_edit();
	~model_edit();
	void draw_model_list();
	void draw_model_item_property();
};
bool import_models(const char* md_file_name);
