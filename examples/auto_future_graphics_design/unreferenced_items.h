#pragma once
#include <string>
#include <vector>
using namespace std;
struct un_ref_res_unit 
{
	string _item_name;
	bool _be_sel{ false };
};
using unref_items_list = vector<un_ref_res_unit>;
class unreferenced_items
{
	unref_items_list _model_list;
	unref_items_list _material_list;
	unref_items_list _shader_list;
	unref_items_list _primitive_list;
	unref_items_list _file_list;
	unref_items_list _txt_list;

public:
	void search_unreferenced_items();
	void show_delete_unreferenced_items();
};

