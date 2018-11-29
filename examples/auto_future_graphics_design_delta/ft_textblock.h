#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_textblock :
		public ft_base
	{
#define MAX_CONTENT_LEN 0x100

		DEF_STRUCT_WITH_INIT(intl_pt,_txt_pt,
			(float, _txtr, {1.f}),
			(float, _txtg, { 1.f }),
			(float, _txtb, { 1.f }),
			(float, _txta, { 1.f }),
			(float, _txt_alignh),
			(float, _txt_alignv),
			(float, _width, { ImGui::GetFontSize() * 35.0f }),
			(char, _content[MAX_CONTENT_LEN]),
			(bool, _wrapped),
			(int, _font_id, {0}),
			(float, _font_scale, { 1.f }))
		ImRect _txt_area;
	public:
		ft_textblock();// : _txt_pt(), _txt_area(0.f, 0.f, 0.f, 0.f){}
		~ft_textblock(){}
		
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
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page(int property_part = -1);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};
	REGISTER_CONTROL(ft_textblock)
}