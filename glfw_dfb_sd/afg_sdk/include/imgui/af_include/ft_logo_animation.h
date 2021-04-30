#pragma once
#include "ft_base.h"
#include "ft_image_play.h"
#include "ft_slider_thumb.h"
/*
1、左边最大白条突然显现。
2、中间两小的白条两边扩展逐步显示出来(首先白亮显示一小部分，然后逐渐显示灰色显示剩余部分，白色光点向两边移动，照亮原来灰色部分).
3、两边中间产生两个蓝色碎片组成的光点，亮度逐渐增加，同时产生碎块。同时照出隧道边缘部分，随着光点亮度加强，相继照出隧道内壁白色纹理、隧道顶端白色横条。
4、蓝色光点达到最亮时，开始向下移动，移动过程中移动轨迹产生蓝色碎片，同时水温和油耗逐渐显示出来，隧道上部图案也逐渐显示出来。
5、光点达到最底下时，隧道地下的路面边缘显示出一部分。然后光点消失，只有蓝色光亮轨迹和其周围的碎片。
6、碎片自上而下逐渐消失，隧道上面白条全部显示出来，并开始逐步变宽。隧道底部速度指示逐渐显示出来。
  上边的白条变宽到一定程度后开始分裂出来，分裂的条开始向内移动。
  隧道下面显示出来的白色速度指示开始分裂出很多小的部分。
7、隧道上面分裂出来的部分开始向内移动，下面分裂的白色的速度指示白块之间的距离逐渐增大，向内移动，速度和档位逐渐显示出来

*/
namespace auto_future
{
	class ft_logo_animation :
		public ft_base
	{
		uint16_t _time_value;
		ft_image_play _left_pointer_animation, _right_pointer_animation;
		uint16_t _time_point[10];

	public:
		ft_logo_animation():ft_base(),_time_value(0)
		{
			_time_point[0] = 10;
			_time_point[1] = 20;
			_time_point[2] = 30;
			_time_point[3] = 40;
			_time_point[4] = 50;
			_time_point[5] = 60;
			_time_point[6] = 70;
			_time_point[7] = 80;
			_time_point[8] = 90;
			_time_point[9] = 100;
			
		}
		~ft_logo_animation(){}
		void draw();
		void set_time_line(uint16_t time_value){ _time_value = time_value; }
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page(int property_part = -1);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif

	};
	REGISTER_CONTROL(ft_logo_animation)
}
