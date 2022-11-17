#pragma once
#include "af_model.h"
#include "ft_base.h"
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

class model_ui_generator{
	auto_future::base_ui_component* _proot;
	float x_r[2];
	float y_r[2];
	float z_r[2];
public:
	model_ui_generator(auto_future::base_ui_component* proot) :_proot(proot){}
	bool import_models(const char* md_file_name);
};