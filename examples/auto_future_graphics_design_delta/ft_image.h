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
	
		DEF_STRUCT_WITH_INIT(intl_pt,_img_pt,
			(float, _sizew, {20.f}),
			(float, _sizeh, {20.f}),
			(float, _aposx, {0.f}),
			(float, _aposy, {0.f}),
			(int, _anchor_type, {en_anchor_top_left}),
			(int, _texture_index_txt, {0}),
			(float, _angle_srd, {0.f}))
	public:	
		ft_image();// : ft_base(){}
		ImVec2 get_size()
		{
			return ImVec2(_img_pt._sizew, _img_pt._sizeh);
		}
		void set_size(const ImVec2& im_size)
		{
			switch (_img_pt._anchor_type)
			{
			case en_anchor_top_right:
				set_base_posx( base_pos().x - (im_size.x - _img_pt._sizew));
				break;
			case en_anchor_bottom_right:
				set_base_posx(base_pos().x - (im_size.x - _img_pt._sizew));
				set_base_posy(base_pos().y - (im_size.y - _img_pt._sizeh));
				break;
			case en_anchor_bottom_left:
				set_base_posy(base_pos().y - (im_size.y - _img_pt._sizeh));
				break;
			case en_anchor_center:
				set_base_posx(base_pos().x - (im_size.x - _img_pt._sizew)/2);
				set_base_posy(base_pos().y - (im_size.y - _img_pt._sizeh)/2);
				break;
			case en_anchor_top_left:
			default:
				break;
			}
			_img_pt._sizew = im_size.x;
			_img_pt._sizeh = im_size.y;
		}
		void set_anchor_type(anchor_type antp)
		{
			_img_pt._anchor_type = antp;
		}
		void set_texture_id(int texture_id)
		{
			_img_pt._texture_index_txt = texture_id;
		}
		int get_texture_id()
		{
			return _img_pt._texture_index_txt;
		}
		void draw();
		base_ui_component* get_hit_ui_object(float posx, float posy);

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
		
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif

		void rotate(float angle){ _img_pt._angle_srd = angle; }
	};

	REGISTER_CONTROL(ft_image)
}