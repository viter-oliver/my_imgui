#pragma once
#include <map>
#include <string>
using namespace std;

struct texture_unit
{
	float _x0, _y0, _x1, _y1;
	texture_unit() :_x0(0), _y0(0), _x1(0), _y1(0){}
};
typedef map<string, texture_unit> mtxt_internal;
extern int g_txt_width_intl;
extern int g_txt_height_intl;
extern int g_txt_id_intl;
extern mtxt_internal g_mtxt_intl;