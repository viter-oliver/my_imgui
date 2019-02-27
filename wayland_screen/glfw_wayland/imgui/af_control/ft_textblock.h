#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_textblock :
		public ft_base
	{
#define MAX_CONTENT_LEN 0x100
		struct intl_pt
		{
			ImVec4 _txt_color;
			ImVec2 _txt_align;
			float _width;
			char _content[MAX_CONTENT_LEN];
			bool _wrapped;
			int _font_id;
			float _font_scale;
			intl_pt()
				:_width(ImGui::GetFontSize() * 35.0f),
				_txt_color(1.f, 1.f, 1.f, 1.f),_font_id(0), _font_scale(1.0f),
				_wrapped(false)
			{
				memset(_content, 0, MAX_CONTENT_LEN);
			}
		};
		intl_pt _txt_pt;
	public:
		ft_textblock() :_txt_pt(){}
		~ft_textblock(){}
		int collect_property_range(vproperty_list& vplist)
		{
			int plen = ft_base::collect_property_range(vplist);
			int len = sizeof(intl_pt);
			vplist.emplace_back(&_txt_pt, len);
			len += plen;
			return len;
		}
		void draw();
		void set_content(const char* strct)
		{
			strcpy(_txt_pt._content, strct);
		}
		const char* content()
		{
			return _txt_pt._content;
		}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page(int property_part = -1);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};
	REGISTER_CONTROL(ft_textblock)
}