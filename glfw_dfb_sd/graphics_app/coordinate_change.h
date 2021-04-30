

#ifndef  __COORDINATE_CHANGE_H__
#define  __COORDINATE_CHANGE_H__

#include "stdint.h" 

typedef struct
{
    float  x;   //纵向位置，精度0.01米
    float  y;   //横向位置，精度0.01米
    float  z;   //高度位置，精度0.01米
}DefStructCoordinatePiont;

// 以HUD坐标系为参考，计算目标在HUD投影区的位置
typedef struct
{
/*********************************系统输入参数****************************/
    DefStructCoordinatePiont  obj_input;  //目标的ADAS坐标系位置，精度0.001米，自变量，由ADAS输出；
                                          //在摄像头下方z轴为负，反之为正；如果是障碍物坐标的话z值要减去ADAS到地面高度。
                                          
    DefStructCoordinatePiont  eye_input;  //人眼在HUD坐标系位置，精度0.001米，自变量，由双目摄像头输出
/***********************************************************************/

/*********************************系统配置参数****************************/
    DefStructCoordinatePiont  hud_setup;  //HUD在ADAS坐标系位置，精度0.001米，定量值

    DefStructCoordinatePiont  hud_LU_setup;  //HUD投影区左上角在HUD坐标系位置，精度0.001米，定量值

    DefStructCoordinatePiont  hud_RU_setup;  //HUD投影区右上角在HUD坐标系位置，精度0.001米，定量值

    DefStructCoordinatePiont  hud_LD_setup;  //HUD投影区左下角在HUD坐标系位置，精度0.001米，定量值

    DefStructCoordinatePiont  hud_RD_setup;  //HUD投影区右下角在HUD坐标系纵向位置，精度0.001米，定量值

    int    hud_display_pixel_x_setup;  // HUD有效显示区X轴像素点，定量值
    int    hud_display_pixel_y_setup;  // HUD有效显示区Y轴像素点，定量值

    float  hud_to_ground;  //hud到地面的距离
    float  ADAS_to_ground;  //ADAS到地面的距离
/***********************************************************************/

/*********************************系统输出参数****************************/
    DefStructCoordinatePiont  eye_obj; //在人眼坐标系中，目标的位置，精度0.001米，定量值
    DefStructCoordinatePiont  lcd_obj; //在人眼坐标系中，目标在HUD投影区的位置，精度0.001米，定量值

    int    hud_obj_pixel_x;  // 目标在HUD有效显示区X轴像素位置，输出值
    int    hud_obj_pixel_y;  // 目标在HUD有效显示区X轴像素位置，输出值
/***********************************************************************/
}DefStructCoordinateChange;

void Coordinate_Change_Init(DefStructCoordinateChange *str);

#define  ERROR_SETUP -2
#define  ERROR_OVER_RANGE -1
#define  OKAY 0
int  Coordinate_Change(DefStructCoordinateChange *str);

#endif
