#pragma once
#include "af_feedback.h"
class feedback_edit
{
	psfeed_back _psfb_sel = {nullptr};
	vector<float> _output_buff;

public:
	feedback_edit();
	~feedback_edit();
	void draw_feedback_list();
	void draw_feedback_item_property();
};

