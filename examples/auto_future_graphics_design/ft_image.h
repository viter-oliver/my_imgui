#pragma once
#include "ft_base.h"
class ft_image :
	public ft_base
{
	struct intl_pt
	{
		ImVec2 _size;
		ImVec2 _axis_pos;
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
	void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
	bool init_json_unit(Value& junit);
#endif

	void rotate(float angle){ _img_pt._angle = angle; }
};

REGISTER_CONTROL(ft_image)
