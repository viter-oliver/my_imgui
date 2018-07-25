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
			float _position{ 0.f };
			int _direction_item{ 0 };
			ImVec2 _bg_axi_pos;
			float _bg_angle{ 0.f };
			int _texture_bg_index{ 0 };

			ImVec2 _head_pos;
			int _texture_head_index{ 0 };

			ImVec2 _thumb_pos;
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
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page();
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
		bool handle_mouse();
	private:

		bool read_point_position_file(const char *str);
		//save texture size
		ImVec2 _bg_texture_size;
		ImVec2 _head_texture_size;
		ImVec2 _thumb_texture_size;

		char _cbuffer_random_text[128];

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