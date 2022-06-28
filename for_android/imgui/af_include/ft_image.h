#pragma once
#include "ft_base.h"
namespace zl_future
{
	class AFG_EXPORT ft_image :
		public ft_base
	{
	
		DEF_STRUCT_WITH_INIT(intl_pt,_img_pt,
			(float, _aposx, {0.f}),
			(float, _aposy, {0.f}),
			(int, _anchor_type, {en_anchor_top_left}),
			(af_vi2, _texture_index_txt),
			(float, _alpha_nml, {1.f} ),
            (af_vec3, _tin_clr ),
			(float, _angle_srd, {0.f}))
	public:	
		ft_image();// : ft_base(){}
		
		void set_size(const ImVec2& im_size)
		{
			switch (_img_pt._anchor_type)
			{
			case en_anchor_top_right:
				set_base_posx(base_pos().x - (im_size.x - _in_p._sizew));
				break;
			case en_anchor_bottom_right:
				set_base_posx(base_pos().x - (im_size.x - _in_p._sizew));
				set_base_posy(base_pos().y - (im_size.y - _in_p._sizeh));
				break;
			case en_anchor_bottom_left:
				set_base_posy(base_pos().y - (im_size.y - _in_p._sizeh));
				break;
			case en_anchor_center:
				set_base_posx(base_pos().x - (im_size.x - _in_p._sizew) / 2);
				set_base_posy(base_pos().y - (im_size.y - _in_p._sizeh) / 2);
				break;
			case en_anchor_top_left:
			default:
				break;
			}
			_in_p._sizew = im_size.x;
			_in_p._sizeh = im_size.y;
		}
		void set_anchor_type(anchor_type antp)
		{
			_img_pt._anchor_type = antp;
		}
          
		void set_texture_id(af_vi2 texture_id)
		{
			_img_pt._texture_index_txt = texture_id;
		}
		af_vi2 get_texture_id()
		{
			return _img_pt._texture_index_txt;
		}
		void draw();
#if !defined(DISABLE_DEMO)
	private:
		enum{ en_pt_sz_x, en_pt_sz_y, en_pt_ax_pos_x, en_pt_ax_pos_y,\
			en_pt_ac_type,en_pt_txt_id,en_pt_angle};
		ImVec2 _edit_size;

#endif

	public:
		void rotate(float angle){ _img_pt._angle_srd = angle; }
	};

	REGISTER_CONTROL(ft_image)
}