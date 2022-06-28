#pragma once
#include "ft_base.h"
#include "ft_image_play.h"
#include "ft_slider_thumb.h"
/*
1�����������ͻȻ���֡�
2���м���С�İ���������չ����ʾ����(���Ȱ�����ʾһС���֣�Ȼ������ʾ��ɫ��ʾʣ�ಿ�֣���ɫ����������ƶ�������ԭ����ɫ����).
3�������м����������ɫ��Ƭ��ɵĹ�㣬���������ӣ�ͬʱ������顣ͬʱ�ճ�������Ե���֣����Ź�����ȼ�ǿ������ճ������ڱڰ�ɫ�������������˰�ɫ������
4����ɫ���ﵽ����ʱ����ʼ�����ƶ����ƶ��������ƶ��켣������ɫ��Ƭ��ͬʱˮ�º��ͺ�����ʾ�����������ϲ�ͼ��Ҳ����ʾ������
5�����ﵽ�����ʱ���������µ�·���Ե��ʾ��һ���֡�Ȼ������ʧ��ֻ����ɫ�����켣������Χ����Ƭ��
6����Ƭ���϶�������ʧ�������������ȫ����ʾ����������ʼ�𲽱���������ײ��ٶ�ָʾ����ʾ������
  �ϱߵİ��������һ���̶Ⱥ�ʼ���ѳ��������ѵ�����ʼ�����ƶ���
  ����������ʾ�����İ�ɫ�ٶ�ָʾ��ʼ���ѳ��ܶ�С�Ĳ��֡�
7������������ѳ����Ĳ��ֿ�ʼ�����ƶ���������ѵİ�ɫ���ٶ�ָʾ�׿�֮��ľ��������������ƶ����ٶȺ͵�λ����ʾ����

*/
namespace zl_future
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
#if !defined(DISABLE_DEMO)
		void draw_peroperty_page(int property_part = -1);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif

	};
	REGISTER_CONTROL(ft_logo_animation)
}
