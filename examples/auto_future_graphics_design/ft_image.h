#pragma once
#include "ft_base.h"
namespace auto_future
{
	enum anchor_type
	{
		en_anchor_top_left,
		en_anchor_top_right,
		en_anchor_bottom_right,
		en_anchor_bottom_left,
		en_anchor_center,
	};
	class AFG_EXPORT ft_image :
		public ft_base
	{
		struct intl_pt
		{
			ImVec2 _size;
			ImVec2 _axis_pos;
			int _anchor_type = { en_anchor_top_left };
			int _texture_index;
			float _angle;
			intl_pt() :_texture_index(0), _angle(0.0){}
		};
		intl_pt _img_pt;
	public:	
		ft_image() :ft_base(), _img_pt(){}
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int len = sizeof(intl_pt);
			vplist.emplace_back(&_img_pt, len);
			len += plen;
			return len;
		}
		ImVec2 get_size()
		{
			return _img_pt._size;
		}
		void set_size(ImVec2& im_size)
		{
			switch (_img_pt._anchor_type)
			{
			case en_anchor_top_right:
				base_pos().x -= (im_size.x - _img_pt._size.x);
				break;
			case en_anchor_bottom_right:
				base_pos().x -= (im_size.x - _img_pt._size.x);
				base_pos().y -= (im_size.y - _img_pt._size.y);
				break;
			case en_anchor_bottom_left:
				base_pos().y -= (im_size.y - _img_pt._size.y);
				break;
			case en_anchor_center:
				base_pos().x -= (im_size.x - _img_pt._size.x)/2;
				base_pos().y -= (im_size.y - _img_pt._size.y)/2;
				break;
			case en_anchor_top_left:
			default:
				break;
			}
			_img_pt._size = im_size;
		}
		void set_anchor_type(anchor_type antp)
		{
			_img_pt._anchor_type = antp;
		}
		void set_texture_id(int texture_id)
		{
			_img_pt._texture_index = texture_id;
		}
		int get_texture_id()
		{
			return _img_pt._texture_index;
		}
		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		enum 
		{
			en_parent_property=1,
			en_geometry_property=2,
			en_texture_property=8,
		};
	private:
		enum{ en_pt_sz_x, en_pt_sz_y, en_pt_ax_pos_x, en_pt_ax_pos_y,\
			en_pt_ac_type,en_pt_txt_id,en_pt_angle};
		intl_pt _img_pt_bk;
		ImVec2 _edit_size;

	public:
		void draw_peroperty_page(int property_part = -1);
		void execute_command(command_elemment& ele_cmd);
		command_elemment clone_cmd_ele(command_elemment&ele_cmd);

		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif

		void rotate(float angle){ _img_pt._angle = angle; }
	};

	REGISTER_CONTROL(ft_image)
}