#pragma once


#ifdef WIN32
#include <gl/gl3w.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#endif // WIN32
#include <glfw/glfw3.h>


enum DATA_TYPE
{
	em_invalid,  //无效
	em_normal,   //正常
	em_abnormal,  //异常
	em_suspicious,//可疑
	em_data_max
};

enum tire_press_warring_sts
{
	em_tire_normal,
	em_tire_press_high,
	em_tire_press_low
};

//enum Em_water_temp
//{
//	water_temp_0,
//   	water_temp_1,
//   	water_temp_2,
//   	water_temp_3,
//   	water_temp_4,
//   	water_temp_5,
//   	water_temp_6,
//   	water_temp_7,
//	water_temp_max
//};


//typedef unsigned char u8;
//typedef unsigned short u16;

extern void reg_main_view_data();

