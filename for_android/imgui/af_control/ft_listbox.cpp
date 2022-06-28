#include "ft_listbox.h"
#include "res_output.h"

namespace zl_future
{
	ft_listbox::ft_listbox()
		:ft_base()
		, min_dis_index(0)
		, spacing(0.0,0.0)
		, cur_index(0)
		, max_display_num(-1)
	{
		/*_be_window = true;*/
          _lt_pt._texture_index_txt = { 0, 0 };
#if !defined(DISABLE_DEMO)
		reg_property_handle(&_in_p, 2, [this](void*){

			if (ImGui::SliderFloat("Width", &_in_p._sizew, 0.f, base_ui_component::screenw))
			{
				update_draw_list_pos();
			}
		});
		reg_property_handle(&_in_p, 3, [this](void*){

			if (ImGui::SliderFloat("Height", &_in_p._sizeh, 0, base_ui_component::screenh))
			{
				update_draw_list_pos();
			}
		});

		reg_property_handle(&_lt_pt, 0, [this](void*){
			if (ImGui::Combo("direction", &_lt_pt._direction, "vertical\0horizontal\0\0"))
			{
				update_draw_list_pos();
			}
		});
		reg_property_handle(&_lt_pt, 1, [this](void*){		
			bool changed_flag = false;
			if (_vchilds.empty())
			{
				changed_flag = ImGui::SliderInt("cur index", &_lt_pt._cur_index_uhd, -1, -1);
			}
			else
			{
				changed_flag = ImGui::SliderInt("cur index", &_lt_pt._cur_index_uhd, 0, _vchilds.size() - 1);
			}	
			if (changed_flag)
			{
				set_cur_index(_lt_pt._cur_index_uhd);
			}
		});
		reg_property_handle(&_lt_pt, 2, [this](void*){			
			if (ImGui::SliderInt("max display num", &_lt_pt._max_display_num_uhd, 1, 10))
			{
				set_max_display_num(_lt_pt._max_display_num_uhd);
			}
		});
#endif
	}

	void ft_listbox::draw()
	{
		if (max_display_num == -1)
		{
			set_max_display_num(_lt_pt._max_display_num_uhd);
		}
          auto img_txt_id = _lt_pt._texture_index_txt;
          auto& cur_res_list = *g_vres_texture_list[ img_txt_id.x ];
          int texture_id = cur_res_list.texture_id();
          vres_txt_cd& ptext_cd = cur_res_list.vtexture_coordinates;
		if (ptext_cd.size() == 0)
			return;
          if( img_txt_id.y >= ptext_cd.size() )
		{
               LOGE( "invalid texture index:%d\n", img_txt_id.y);
               return;
		}
          int texture_width = cur_res_list.texture_width;
          int texture_height = cur_res_list.texture_height;


		ImVec2 apos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		
		apos += winpos;
		ImVec2 szpos = apos + ImVec2(_in_p._sizew, _in_p._sizeh);
		

		int num_space = cur_index - min_dis_index;
          ImVec2 offset = ( item_size - ImVec2( ptext_cd[ img_txt_id.y ].owidth(), ptext_cd[ img_txt_id.y ].oheight() ) ) / 2;
		
		ImVec2 pos0 = apos + offset + spacing*num_space;
          ImVec2 pos1 = { pos0.x, pos0.y + ptext_cd[ img_txt_id.y ].oheight() };
          ImVec2 pos2 = { pos0.x + ptext_cd[ img_txt_id.y ].owidth(), pos0.y + ptext_cd[ img_txt_id.y ].oheight() };
          ImVec2 pos3 = { pos0.x + ptext_cd[ img_txt_id.y ].owidth(), pos0.y };
		
          ImVec2 uv0 = ImVec2( ptext_cd[ img_txt_id.y ]._x0 / texture_width, ptext_cd[ img_txt_id.y ]._y0 / texture_height );
          ImVec2 uv1 = ImVec2( ptext_cd[ img_txt_id.y ]._x0 / texture_width, ( ptext_cd[ img_txt_id.y ]._y1 ) / texture_height );
          ImVec2 uv2 = ImVec2( ( ptext_cd[ img_txt_id.y ]._x1 ) / texture_width, ( ptext_cd[ img_txt_id.y ]._y1 ) / texture_height );
          ImVec2 uv3 = ImVec2( ( ptext_cd[ img_txt_id.y ]._x1 ) / texture_width, ( ptext_cd[ img_txt_id.y ]._y0 ) / texture_height );

		ImGui::PushClipRect(apos, szpos, true);

		ImGui::ImageQuad((ImTextureID)texture_id, pos0, pos1, pos2, pos3, uv0, uv1, uv2, uv3);

		for (auto item : draw_list_pos_map)
		{
			ImGui::PushClipRect(apos + item.second, apos + item_size + item.second, true);
			//_vchilds[item.first]->set_base_pos(item.second.x, item.second.y);
			_vchilds[item.first]->draw_frames();
			ImGui::PopClipRect();
		}
		ImGui::PopClipRect();
	}

	void ft_listbox::draw_frames()
	{
		draw();
	}

	void ft_listbox::update_draw_list_pos()
	{
		if (_vchilds.empty())
		{
			LOGE("list is empty!\n");
			return;
		}
		if (max_display_num < 0)
		{
			return;
		}

		if (_lt_pt._direction == direction_vertical)
		{
			spacing = ImVec2(0, _in_p._sizeh / max_display_num);

			item_size = ImVec2(_in_p._sizew, _in_p._sizeh / max_display_num);
		}
		else
		{
			spacing = ImVec2(_in_p._sizew / max_display_num,0);

			item_size = ImVec2(_in_p._sizew / max_display_num, _in_p._sizeh);
		}

		draw_list_pos_map.clear();

		if (min_dis_index > 0)
		{
			draw_list_pos_map[min_dis_index - 1] = spacing*(-1);
			_vchilds[min_dis_index - 1]->set_base_pos(draw_list_pos_map[min_dis_index - 1].x, draw_list_pos_map[min_dis_index - 1].y);
		}
		for (int i = 0; i < max_display_num; i++)
		{
			draw_list_pos_map[min_dis_index + i] = spacing*i;
			_vchilds[min_dis_index + i]->set_base_pos(draw_list_pos_map[min_dis_index + i].x, draw_list_pos_map[min_dis_index + i].y);
		}
		if (min_dis_index + max_display_num < _vchilds.size())
		{
			int max_draw_index = min_dis_index + max_display_num;
			draw_list_pos_map[max_draw_index] = spacing*max_display_num;
			_vchilds[max_draw_index]->set_base_pos(draw_list_pos_map[max_draw_index].x, draw_list_pos_map[max_draw_index].y);
		}
	}

	void ft_listbox::update_highlight_pos()
	{

	}

	void ft_listbox::add_child(base_ui_component* pchild)
	{
		ft_base* _pnode = new ft_base;

		ft_base::add_child(_pnode);
		_pnode->add_child(pchild);

		max_display_num = _lt_pt._max_display_num_uhd > _vchilds.size() ? _vchilds.size() : _lt_pt._max_display_num_uhd;

		update_draw_list_pos();
	}

	void ft_listbox::remove_child(base_ui_component* pchild)
	{
		for (auto item = _vchilds.begin(); item != _vchilds.end(); item++)
		{
			if ((*item)->get_child(0) == pchild)
			{
				base_ui_component* _pchild = *item;
				_vchilds.erase(item);
				delete _pchild;
				break;
			}
		}

		max_display_num = _lt_pt._max_display_num_uhd > _vchilds.size() ? _vchilds.size() : _lt_pt._max_display_num_uhd;

		update_draw_list_pos();
	}


	void ft_listbox::set_cur_index(int index)
	{
		if (index == cur_index ||index < 0 || index >= _vchilds.size())
		{
			return;
		}

		cur_index = index;

		if (cur_index < min_dis_index)
		{
			min_dis_index = cur_index;
		}
		else if (cur_index >= min_dis_index + max_display_num)
		{
			min_dis_index = cur_index - max_display_num + 1;
		}

		update_draw_list_pos();
	}

	void ft_listbox::set_max_display_num(int num)
	{
		if (max_display_num == num || num <= 0)
		{
			return;
		}
		if (_lt_pt._max_display_num_uhd != num)
		{
			_lt_pt._max_display_num_uhd = num;
		}

		max_display_num = num > _vchilds.size() ? _vchilds.size():num;
	

		if (cur_index - min_dis_index >= max_display_num)
		{
			min_dis_index = cur_index - max_display_num + 1;
		}

		if (min_dis_index + max_display_num + 1 > _vchilds.size())
		{
			min_dis_index = _vchilds.size() - max_display_num;
		}

		update_draw_list_pos();
	}
}
