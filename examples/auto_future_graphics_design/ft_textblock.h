#pragma once
#include "ft_base.h"
class ft_textblock :
	public ft_base
{
#define MAX_CONTENT_LEN 0x100
	struct intl_pt
	{
		ImVec4 _txt_color;
		float _width;
		char _content[MAX_CONTENT_LEN];
		bool _wrapped;
		intl_pt()
			:_width(ImGui::GetFontSize() * 35.0f),
			_txt_color(1.f, 1.f, 1.f, 1.f),
			_wrapped(false)
		{
			memset(_content, 0, MAX_CONTENT_LEN);
		}
	};
	intl_pt _txt_pt;
public:
	ft_textblock() :_txt_pt(){}
	~ft_textblock(){}
	void collect_property_range(vproperty_list& vplist)
	{
		ft_base::collect_property_range(vplist);
		vplist.emplace_back(&_txt_pt, sizeof(intl_pt));
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
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
	bool init_json_unit(Value& junit);
#endif
};
REGISTER_CONTROL(ft_textblock)
