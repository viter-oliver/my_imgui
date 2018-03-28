#pragma once
#include <vector>
#include <string>
//ÎÆÀí×ÊÔ´
using namespace std;
struct res_texture_coordinate
{
	string _file_name;
	float _x0, _y0, _x1, _y1;
	res_texture_coordinate() :_x0(0.f), _y0(0.f), _x1(0.f), _y1(0.f){}
};
typedef vector<res_texture_coordinate> vres_txt_cd;
struct res_texture_list
{
	int texture_width;
	int texture_height;
	int texture_id;
	//string file_name_sets;
	char** file_name_sets;
	vres_txt_cd vtexture_coordinates;
};
typedef vector<res_texture_list> vres_txt_list;
extern vres_txt_list g_vres_texture_list;

