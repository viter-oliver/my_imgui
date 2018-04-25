#pragma once
#include <vector>
#define  FILE_NAME_LEN   0x100
using namespace std;
class res_edit
{
	vector<char*> _resshow;
	int _tmp_current_txt_id_index;
public:
	res_edit();
	~res_edit();
	void draw_res_list();
	void draw_res_item_property();
};

