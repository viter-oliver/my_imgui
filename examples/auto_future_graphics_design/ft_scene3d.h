#pragma once
#include "ft_base.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class ft_scene3d :
	public ft_base
{
	ImVec2 _size;
	int _texture_index;
	//camera
	//view
	glm::vec3 _camera_eye;
	glm::vec3 _camera_center;
	glm::vec3 _camera_up;
    //projection
	float _view_angle;
	float _aspect;
	float _near;
	float _far;

	//frame buffer
	unsigned int _fboId;
	unsigned int _colorTextId;
	unsigned int _depthStencilTextId;
	//light
	bool _light_enabled;
	unsigned int _light_color;
public:
	typedef enum
	{
		en_directional_light,
		en_point_light,
		en_spot_light,
	} en_light_type;
	struct light_attenuation
	{
		float _constant;
		float _linear;
		float _quadratic;
	};
private:
	en_light_type _light_type;
	light_attenuation _light_attenuation;
public:
	
	ft_scene3d()
		:ft_base(),_texture_index(0),_view_angle(45.0f),
		_size(screenw / 2, screenh / 2), _aspect(screenh / screenw), _near(1.f), _far(10.f),
		_fboId(0), _colorTextId(0), _depthStencilTextId(0),
		_light_enabled(true), _light_color(IM_COL32(126,126,126,255))
	{}
	ft_scene3d(ft_scene3d& bsource);
	~ft_scene3d();
	base_ui_component* get_a_copy();
	//ft_button(int tid) :base_ui_component(tid){}
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
#endif
	bool init_from_json(Value& jvalue);
	bool handle_mouse();

	en_light_type light_type()
	{
		return _light_type;
	}
	void set_light_type(en_light_type ltype);

};

REGISTER_CONTROL(ft_scene3d)