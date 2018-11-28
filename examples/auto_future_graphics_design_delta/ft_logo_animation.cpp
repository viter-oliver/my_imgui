#include "ft_logo_animation.h"

namespace auto_future
{

	void ft_logo_animation::draw()
	{
		if (_time_value<_time_point[0])
		{
		}
		else
		if (_time_value>=_time_point[1]&&_time_value<_time_point[2])
		{

		}
		else
		if (_time_value >= _time_point[2] && _time_value < _time_point[3])
		{

		}
		else
		if (_time_value >= _time_point[3] && _time_value < _time_point[4])
		{
		}
		else
		if (_time_value >= _time_point[4] && _time_value < _time_point[5])
		{

		}
		else
		if (_time_value>=_time_point[5]&&_time_value<_time_point[6])
		{
		}
		else
		if (_time_value >= _time_point[6] && _time_value < _time_point[7])
		{

		}
		else
		if (_time_value >= _time_point[7] && _time_value < _time_point[8])
		{
		}
		else
		if (_time_value >= _time_point[8] && _time_value < _time_point[9])
		{

		}
		
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void ft_logo_animation::draw_peroperty_page(int property_part)
	{

	}

	bool ft_logo_animation::init_from_json(Value& jvalue)
	{
		return true;
	}

	bool ft_logo_animation::init_json_unit(Value& junit)
	{
		return true;
	}
#endif
}