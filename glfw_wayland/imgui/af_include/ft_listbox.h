#pragma once
#include "ft_base.h"
namespace auto_future
{
	enum direction_type
	{
		direction_vertical,
		direction_horizontal,
	};

	class AFG_EXPORT ft_listbox :
		public ft_base
	{
		DEF_STRUCT_WITH_INIT(intl_pt, _lt_pt,
			(int, _direction, { direction_vertical }),
			(int, _cur_index_uhd, { 0 }),
			(int, _max_display_num_uhd, { 1 }),
			(af_vi2, _texture_index_txt)			
			)

	public:
		ft_listbox();
		void draw();
		void draw_frames();
		void set_cur_index(int index);
		void update_draw_list_pos();
		void update_highlight_pos();
		void add_child(base_ui_component* pchild);
		void remove_child(base_ui_component* pchild);
		void set_max_display_num(int num);

	private:
		int min_dis_index;
		int cur_index;
		int max_display_num;
		ImVec2 spacing;
		ImVec2 item_size;
		std::map<int, ImVec2> draw_list_pos_map;
	};
	REGISTER_CONTROL(ft_listbox)
}
