#pragma once
#include "af_type.h"
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <vector>
using namespace auto_future;
using namespace std;
class slider_path_picker
{
	int _img_id{0};
	int _pair_id{ 0 };
	vector<ImVec2> _vtrack0;
	vector<ImVec2> _vtrack1;
	string _cur_tacks_file_name;
public:
	int get_img_id()
	{
		return _img_id;
	}
	void view();
};

