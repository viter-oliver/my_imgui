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
	
	ft_scene3d();
	~ft_scene3d();
	en_light_type light_type()
	{
		return _light_type;
	}
	void set_light_type(en_light_type ltype);
};

REGISTER_CONTROL(ft_scene3d)