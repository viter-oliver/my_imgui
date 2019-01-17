#pragma once
#include "ft_base.h"
#include "af_font_res_set.h"
namespace auto_future
{
	class AFG_EXPORT ft_textblock_n :
		public ft_base
	{
#define MAX_CONTENT_LEN 0x100

		DEF_STRUCT_WITH_INIT(intl_pt,_txt_pt,
			(af_vec3, _txt_clr),
			(float, _txt_alignh_nml, {0.f}),
			(float, _txt_alignv_nml, {0.f}),
			(float, _width, { 200.0f }),
			(char, _content[MAX_CONTENT_LEN]),
			(bool, _width_limit, {false}),
			(bool, _omit_rest, { false }),
			(int, _font_id, {0}),
			(float, _font_size, { 16.f }),
			(float, _font_scale, { 1.f }))
		ImRect _txt_area;
		shared_ptr<af_font_res_set> _pfont_res_set;
	public:
		ft_textblock_n();// : _txt_pt(), _txt_area(0.f, 0.f, 0.f, 0.f){}
		~ft_textblock_n(){}
		
		void draw();
		void set_content(const char* strct)
		{
			strcpy(_txt_pt._content, strct);
		}
		const char* content()
		{
			return _txt_pt._content;
		}
		ImRect get_cover_area()
		{
			return _txt_area;
		}
		void set_txt_clr(float r, float g, float b)
		{
			_txt_pt._txt_clr.x = r;
			_txt_pt._txt_clr.y = g;
			_txt_pt._txt_clr.z = b;

		}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		base_ui_component* get_hit_ui_object(float posx, float posy);
#endif
	};
	REGISTER_CONTROL(ft_textblock_n)
}