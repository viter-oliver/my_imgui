#pragma once
#include "af_type.h"
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <string>
#include <vector>
using namespace auto_future;
using namespace std;
class slider_path_picker
{
	af_vi2 _img_id;
	int _pair_id{ 0 };
	vector<ImVec2> _vtrack0;
	vector<ImVec2> _vtrack1;
	string _cur_tacks_file_name;
public:
     slider_path_picker()
     {
          _img_id = { 0, 0 };
     }
	void view();
};

