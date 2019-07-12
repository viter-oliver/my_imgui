
#include "stdint.h" 
#include "coordinate_change.h"
#include <stdio.h>

void Coordinate_Change_Init(DefStructCoordinateChange *str)
{
	str->hud_setup.x = 350.196;
	str->hud_setup.y = -284.232;
	str->hud_setup.z = -379.838;
	
	str->hud_LU_setup.x = 6614.526;
	str->hud_LU_setup.y = -720.94;
	str->hud_LU_setup.z = 211.128;
	
	str->hud_LD_setup.x = 6595.496;
	str->hud_LD_setup.y = -723.925;
	str->hud_LD_setup.z = -225.467;
	
	str->hud_RD_setup.x = 6595.509;
	str->hud_RD_setup.y = 581.643;
	str->hud_RD_setup.z = -225.831;
	
	str->hud_RU_setup.x = 6614.334;
	str->hud_RU_setup.y = 581.093;
	str->hud_RU_setup.z = 206.048;
	
	str->eye_input.x = -888.092;
	str->eye_input.y =  900; //-75.748;
	str->eye_input.z = 400; //314.114;
	
	str->hud_display_pixel_x_setup = 800;
	str->hud_display_pixel_y_setup = 276;

    str->hud_to_ground = 1.088977;  //m
    str->ADAS_to_ground = 1.468815;  // m
	
}

int  Coordinate_Change(DefStructCoordinateChange *str)
{
    float  eye_to_ground = 0;
    float  eye_LU_setup_z = 0;
    float  eye_RU_setup_z = 0;
    float  eye_LD_setup_z = 0;
    float  eye_RD_setup_z = 0;

    if(str->hud_display_pixel_x_setup == 0 || \
       str->hud_display_pixel_y_setup == 0)
    {
        return  ERROR_SETUP;
    }

    // eye_to_ground = str->hud_to_ground + str->eye_input.z;
	
	str->obj_input.x = str->obj_input.x * 1000;
	str->obj_input.y = str->obj_input.y * 1000;
	str->obj_input.z = str->obj_input.z * 1000;

	printf("[xxl]input x:%f, y:%f, z:%f \r\n", str->obj_input.x, str->obj_input.y, str->obj_input.z);

    str->eye_obj.x = (str->obj_input.x - str->hud_setup.x - str->eye_input.x);
    str->eye_obj.y = (str->obj_input.y - str->hud_setup.y - str->eye_input.y);
    str->eye_obj.z = (str->obj_input.z - str->hud_setup.z - str->eye_input.z);

    // str->eye_obj.z = str->obj_input.z - eye_to_ground;

    str->lcd_obj.x = (str->hud_LU_setup.x + str->hud_LD_setup.x) / 2 - str->eye_input.x;

    str->lcd_obj.y = (str->eye_obj.y / str->eye_obj.x) * str->lcd_obj.x;
    str->lcd_obj.z = (str->eye_obj.z / str->eye_obj.x) * str->lcd_obj.x;

    eye_LU_setup_z = str->hud_LU_setup.z - str->eye_input.z;
    eye_RU_setup_z = str->hud_RU_setup.z - str->eye_input.z;
    eye_LD_setup_z = str->hud_LD_setup.z - str->eye_input.z;
    eye_RD_setup_z = str->hud_RD_setup.z - str->eye_input.z;

    str->hud_obj_pixel_x = (str->lcd_obj.y - str->hud_LU_setup.y) * str->hud_display_pixel_x_setup / (str->hud_RU_setup.y - str->hud_LU_setup.y);
    str->hud_obj_pixel_y = (str->lcd_obj.z - (eye_LU_setup_z + eye_RU_setup_z) / 2) / \
                           ((eye_LD_setup_z + eye_RD_setup_z) / 2 - (eye_LU_setup_z + eye_RU_setup_z) / 2) * \
                           str->hud_display_pixel_y_setup;
	
    
   //printf("[xxl] op x:%d, y:%d\n", str->hud_obj_pixel_x, str->hud_obj_pixel_y);
   /*
    if(str->hud_obj_pixel_x < 0 || str->hud_obj_pixel_x > str->hud_display_pixel_x_setup || \
       str->hud_obj_pixel_y < 0 || (str->hud_obj_pixel_y > str->hud_display_pixel_y_setup ))
    {
    	 printf("[xxl]Error\r\n");
        return ERROR_OVER_RANGE;
    }*/

	//str->hud_obj_pixel_y = 138;
   /*
   if(str->hud_obj_pixel_y < 0 && str->hud_obj_pixel_y > -100)
   {
	str->hud_obj_pixel_y = 0;
   }
   else if(str->hud_obj_pixel_y < str->hud_display_pixel_y_setup + 100 && str->hud_obj_pixel_y > str->hud_display_pixel_y_setup)
   {
	str->hud_obj_pixel_y = str->hud_display_pixel_y_setup;
   }
   else
   {
	printf("[xxl] error x:%d, y:%d\n", str->hud_obj_pixel_x, str->hud_obj_pixel_y);
	return ERROR_OVER_RANGE;
   }
   */
/*
   if(str->hud_obj_pixel_x < 0 || str->hud_obj_pixel_x > str->hud_display_pixel_x_setup )
   {
	printf("[xxl] error x:%d, y:%d\n", str->hud_obj_pixel_x, str->hud_obj_pixel_y);
	return ERROR_OVER_RANGE;
   }
*/
	printf("[xxl] op x:%d, y:%d\n", str->hud_obj_pixel_x, str->hud_obj_pixel_y);
	str->hud_obj_pixel_x =  (str->hud_display_pixel_x_setup / 2) - str->hud_obj_pixel_x;
	str->hud_obj_pixel_y = str->hud_obj_pixel_y - (str->hud_display_pixel_y_setup / 2);

	printf("[xxl] okay x:%d, y:%d\n", str->hud_obj_pixel_x, str->hud_obj_pixel_y);
    return OKAY;
}

