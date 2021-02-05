#pragma once
#define  FILE_NAME_LEN   50
using namespace std;
class res_edit
{
     int _cur_texture_id_index = { 0 };
public:
	res_edit();
	~res_edit();
	void draw_res_list();
	void draw_res_item_property();
};

