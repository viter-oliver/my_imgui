#pragma once
#include <vector>
#include <math.h>
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_slider :
		public ft_base
	{
		struct intl_pt
		{
			char _cbuffer_random_text[128];
			//save texture size
			//ImVec2 _bg_texture_size;
			float _bg_txtw, _bg_txth;
			//ImVec2 _head_texture_size;
			float _hd_txtw, _hd_txth;
			//ImVec2 _thumb_texture_size;
			float _tb_txtw, _tb_txth;//thumb size
			float _position{ 0.f };
			int _direction_item{ 0 };
			//ImVec2 _bg_axi_pos;
			float _bg_aposx, _bg_aposy;
			float _bg_angle{ 0.f };
			int _texture_bg_index{ 0 };

			//ImVec2 _head_pos;
			float _hd_posx, _hd_posy;
			int _texture_head_index{ 0 };

			//ImVec2 _thumb_pos;
			float _tb_posx, _tb_posy;
			bool _thumb_visible{false};
			int _texture_thumb_index{ 0 };
		};
		intl_pt _slider_pt;
	public:
		ft_slider();
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int len = sizeof(intl_pt);
			vplist.emplace_back(&_slider_pt, len);
			len += plen;
			return len;
		}

		void link()
		{
			if (2 == _slider_pt._direction_item) //如果保存的是random，这时候就需从文件中读出点数据
			{
				if (NULL != _slider_pt._cbuffer_random_text)
					read_point_position_file(_slider_pt._cbuffer_random_text);
			}
		}

		void set_progress(float value){ _slider_pt._position = value; }
		float set_progress(){ return _slider_pt._position; }

		void set_bg_texture_id(int id){ _slider_pt._texture_bg_index = id; }
		void set_progress_texture_id(int id){ _slider_pt._texture_head_index = id; }
		void set_thumb_texture_id(int id){ _slider_pt._texture_thumb_index = id; }

		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page(int property_part = -1);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	private:
		bool read_point_position_file(const char *str);

		struct random_point_array
		{
			ImVec2 top_point;
			ImVec2 bottom_point;
			random_point_array(){}
			random_point_array(ImVec2 &itop, ImVec2 &ibottom)
				:top_point(itop), bottom_point(ibottom)
			{}

			void operator=(random_point_array &it)
			{
				top_point = it.top_point;
				bottom_point = it.bottom_point;
			}

			bool operator==(random_point_array &it)
			{
				if (it.top_point.x == top_point.x && it.top_point.y == top_point.y && it.bottom_point.x == bottom_point.x && it.bottom_point.x == bottom_point.x)
				{
					return true;
				}
				return false;
			}
		};
		typedef std::vector<random_point_array> random_point_vec;
		random_point_vec ft_slider_random_point_vec;
		float _random_all_length{ 0.f };

		random_point_array _pre_point_2vec2;
		random_point_array _next_point_2vec2;

		random_point_array _current_point_2vec2_thumb_use;
	};

	REGISTER_CONTROL(ft_slider)
}