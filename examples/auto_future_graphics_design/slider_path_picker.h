#pragma once
#include "af_type.h"
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <vector>
using namespace auto_future;
using namespace std;
class slider_path_picker
{
	bool _be_open{false};
	int _img_id{0};
	vector<ImVec2> _vtrack0;
	vector<ImVec2> _vtrack1;
public:
	void set_open(bool be_open)
	{
		_be_open = be_open;
	}
	bool is_open()
	{
		return _be_open;
	}
	int get_img_id()
	{
		return _img_id;
	}

	void view();
};

