#pragma once
#include "af_playlist_group.h"
class playlist_group_edit
{
	sp_playlist_group_unit _sl_playlist_group_unit;
	string _key_name;
	bool _be_draging{ false }, _hit_sm_window{ false };
	string _sel_statemanager_name;
	int _playlist_id = { 0 };
public:
	playlist_group_edit(){}
	~playlist_group_edit(){}
	void set_dragging(bool be_dragging, string stm_name,int playlist_id);
	void view_playlit_group_list();
	void playlist_group_item();
};

