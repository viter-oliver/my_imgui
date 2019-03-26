#pragma once
#include "af_state_manager.h"
#include<chrono>
using namespace chrono;
class state_manager_edit
{
	enum trans_edit_target{
		en_trans_any,
		en_trans_existed,
		en_trans_new,
	};
	ps_state_manager _psel{nullptr};
	bool _be_draging{ false }, _hit_sm_window{ false };
	int _pgidx{ 0 }, _fdidx{0};
	trans_key _cur_trans_edit_key;
	state_transition _trans_edit;
	trans_edit_target _trans_tar{ en_trans_any };
public:
	void set_dragging(bool be_dragging, base_ui_component* pobj = nullptr, uint16_t page_idx = 0, uint16_t off_idx = 0);
	void view_state_managers();
	void view_state_manager_item_property();
};

