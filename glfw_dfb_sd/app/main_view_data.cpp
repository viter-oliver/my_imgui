#pragma execution_character_set("utf-8")

#include "main_view_data.h"
#include "msg_host_n.h"
#include "af_bind.h"
#include "af_primitive_object.h"
#include "af_material.h"
#include "af_state_manager.h"
//#include "enum_txt_name0.h"
#include "ft_base.h"
#include "afg.h"
#include<chrono>
#include "af_timer.h"
#include <thread>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include <mutex>
#include <map>
#include <vector>
#include <math.h>
#include <glfw/glfw3.h>
#include "PageManager.h"
#include "AppDef.h"

#include "enum_txt_res_name.h"

using namespace auto_future_utilities;


extern msg_host_n g_msg_host;
char show_num[0x100] = { 0 };

//int water_temp[water_temp_max] = { en_gid_0water_temp_2_0_png, en_gid_0water_temp_2_1_png, en_gid_0water_temp_2_2_png, en_gid_0water_temp_2_3_png,
//en_gid_0water_temp_2_4_png, en_gid_0water_temp_2_5_png, en_gid_0water_temp_2_6_png, en_gid_0water_temp_2_7_png };

//template<int byte_ltn> unsigned int bigend_to_smallend(BYTE(&byte)[byte_ltn])
//{
//	int id_max = byte_ltn - 1;
//	BYTE small_end[byte_ltn];
//	for (int ix = 0; ix < byte_ltn; ++ix)
//	{
//		small_end[ix] = byte[id_max - ix];
//	}
//	unsigned int ivalue = 0;
//	memcpy(&ivalue, small_end, byte_ltn);
//	return ivalue;
//}

template <class T>
int getArrayLen(T& array)
{
	return (sizeof(array) / sizeof(array[0]) );
}


template<int SIZE>
int bigend_2_small_byte_end(u8(&pbf)[SIZE])
{
	int tmp = 0;
	
	for (int i = 0; i < SIZE; i++)
	{
		tmp <<= 8;
		tmp |= pbf[i];
	}
	return tmp;
}

void reg_main_view_data()
{
	//车速
	g_msg_host.attach_monitor("speed", [&](u8*pbuff, int len){
		struct GNU_DEF struct_speed_msg
		{
			u8 valid;
			u8 warring;
			u8 v_byte[2];
		};
		struct_speed_msg *p = (struct_speed_msg *)(pbuff);
		int value = bigend_2_small_byte_end(p->v_byte);
		if (p->valid == em_normal)
		{
			static int stic_speed = 0;
			if (stic_speed == value)
				return;
			printf("speed=%d\n", value);

			ft_textblock *t = (ft_textblock*)get_aliase_ui_control("show_speed");
			if (t == NULL)
			{
				printf("can not find aliase name:show_speed\n");
				return;
			}
			if (value < 220)
			{
				sprintf(show_num, "%d", value);
				t->set_content(show_num);
			}
			else
			{
				t->set_content("220");
			}
			stic_speed = value;
		}
	});

	//转速
	g_msg_host.attach_monitor("rotate_speed", [&](u8*pbuff, int len){
		//printf("get rotate_speed\n");
		struct GNU_DEF struct_rotate_msg
		{
			u8 valid;
			u8 warring;
			u8 v_byte[2];
		};
		struct_rotate_msg *p = (struct_rotate_msg *)(pbuff);
		int value = bigend_2_small_byte_end(p->v_byte);
		if (p->valid == em_normal)
		{
			static int stic_rotate = 0;
			if (stic_rotate == value)
				return;

			printf("rotate_speed=%d\n", value);

			ft_textblock *t = (ft_textblock*)get_aliase_ui_control("show_rotate");
			if (t == NULL)
			{
				printf("can not find aliase name:show_rotate\n");
				return;
			}
			if (value < 9000)
			{
				sprintf(show_num, "%d.%d", value/1000,(value/100)%10 );
				t->set_content(show_num);
			}
			else
			{
				t->set_content("9.0");
			}
			stic_rotate = value;
		}
	});

	//水温
	g_msg_host.attach_monitor("coor_temp", [&](u8*pbuff, int len){
		struct GNU_DEF struct_coor_msg
		{
			u8 valid;
			u8 warring;
			u8 v_byte[2];
		};
		struct_coor_msg *p = (struct_coor_msg *)(pbuff);
		int value = bigend_2_small_byte_end(p->v_byte);
		ft_image *t = (ft_image*)get_aliase_ui_control("show_coor_process");
		if (t == NULL)
		{
			printf("can not find aliase name:show_coor_process\n");
			return;
		}
		if (p->valid == em_normal)
		{
			int v = (value-1000)*0.1;//v=hmi-value
			static int coor_v = 0;
			if (v != coor_v)
			{
				printf("coor_temp:value=%d\n");
			}
			if (v > coor_v)
			{
				if (v >= 125){
					t->set_texture_id({0, en_gid_0water_temp_2_8_png});
				}
				else if (v >= 115)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_7_png });
				}
				else if (v >= 110)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_6_png });

				}
				else if (v >= 105)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_5_png });

				}
				else if (v >= 85)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_4_png });

				}
				else if (v >= 70)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_3_png });

				}
				else if (v >= 60)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_2_png });

				}
				else if (v >= 54)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_1_png });
				}
				else
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_0_png });
				}

			}
			else if (v <coor_v)
			{
				if (v <= 50)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_0_png });
				}
				else if (v <= 56)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_1_png });
				}
				else if (v <= 66)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_2_png });
				}
				else if (v <= 80)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_3_png });
				}
				else if (v <= 100)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_4_png });
				}
				else if (v <= 108)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_5_png });
				}
				else if (v <= 112)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_6_png });
				}
				else if (v <= 121)
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_7_png });
				}
				else
				{
					t->set_texture_id({ 0, en_gid_0water_temp_2_8_png });
				}
			}
			coor_v = v;
		}
		else if (p->valid == em_invalid)
		{
			t->set_texture_id({ 0, en_gid_0water_temp_2_0_png });
		}
	});

	//小计里程 ,show_tripa
	g_msg_host.attach_monitor("tripa", [&](u8*pbuff, int len){
		struct GNU_DEF struct_tripa_msg
		{
			u8 valid;
			u8 v_byte[3];
		};
		struct_tripa_msg *p = (struct_tripa_msg *)(pbuff);
		int value = bigend_2_small_byte_end(p->v_byte);

		if (p->valid == em_normal)
		{
			ft_textblock *t = (ft_textblock*)get_aliase_ui_control("show_tripa");
			if (t == NULL)
			{
				printf("can not find aliase name:show_tripa\n");
			}
			static int tripa_v = 0;
			
			if (tripa_v == value)
				return;

			printf("tripa=%d\n", value);
			
			if (value < 9999)
			{
				sprintf(show_num, "%.1fkm", value *0.1);
				t->set_content(show_num);
			}
			else if (value>9999)
			{
				t->set_content("0.0km");
			}
			tripa_v = value;
		}
	});

	//瞬时油耗 ,show_inf
	g_msg_host.attach_monitor("inf", [&](u8*pbuff, int len){
		struct GNU_DEF struct_inf_msg
		{
			u8 valid;
			u8 v_byte[2];
		};
		struct_inf_msg *p = (struct_inf_msg *)(pbuff);
		int value = bigend_2_small_byte_end(p->v_byte);

		ft_textblock *t = (ft_textblock*)get_aliase_ui_control("show_inf");
		if (p->valid == em_normal)
		{
			static int inf_v = 0;
			if (inf_v == value)
				return;
			printf("inf=%d\n", value);

			if (value < 160)
			{
				sprintf(show_num, "%.1fL/100km", value *0.1);
				t->set_content(show_num);
			}
			else
			{
				t->set_content("16.0L/100km");
			}
			inf_v = value;
		}
		else if (p->valid == em_invalid)
		{
			t->set_content("--.-L/100km");
		}
	});

	//续航里程 show_recharge_mile
	g_msg_host.attach_monitor("recharge_mile", [&](u8*pbuff, int len){
		struct GNU_DEF struct_inf_msg
		{
			u8 valid;
			u8 v_byte[3];
		};
		struct_inf_msg *p = (struct_inf_msg *)(pbuff);
		int value = bigend_2_small_byte_end(p->v_byte);

		ft_textblock *t = (ft_textblock*)get_aliase_ui_control("show_recharge_mile");

		if (p->valid == em_normal)
		{

			if (value <500)
			{
				t->set_content("--");
			}

			static int recharge_v = -1;
			if (recharge_v +49 > value)
				return;
			printf("recharge=%d\n", value);

			if (value >= 500)
			{
				int v = 0;
				if (value > 9950)
				{
					v = 9950;
				}
				else
				{
					v = value*0.1;
				}

				//精度为5
				if (v % 5 == 0)
				{
					sprintf(show_num, "%d", v);
					t->set_content(show_num);
				}
			}

			recharge_v = value;
		}
	});

	// 总里程odo  show_odo
	g_msg_host.attach_monitor("odo", [&](u8*pbuff, int len){
		struct GNU_DEF struct_odo_msg
		{
			u8 valid;
			u8 v_byte[4];
		};
		struct_odo_msg *p = (struct_odo_msg *)(pbuff);
		int value = bigend_2_small_byte_end(p->v_byte);

		ft_textblock *t = (ft_textblock*)get_aliase_ui_control("show_odo");
		if (p->valid == em_normal)
		{
			static int odo_v = 0;
			if (odo_v == value)
				return;
			printf("odo=%d\n", value);

			if (value > 999999)
			{
				t->set_content("999999");

			}
			else
			{
				sprintf(show_num, "%d", value);
				t->set_content(show_num);
			}
			odo_v = value;
		}
	});

	//燃油 fuel
	g_msg_host.attach_monitor("fuel", [&](u8*pbuff, int len){
		int data1 = *pbuff;
		int data2 = *(pbuff + 1);
		if (data1 != em_normal)
			return;

		ft_image *t = (ft_image*)get_aliase_ui_control("show_fuel_process");
		int value = en_gid_0oil_2_0_png;
		value += data2;
		if (value > en_gid_0oil_2_8_png || value <en_gid_0oil_2_0_png)
			return;

		t->set_texture_id({0,value});
	});

	//档位
	g_msg_host.attach_monitor("gear", [&](u8*pbuff, int len){
		struct GNU_DEF struct_gear_msg
		{
			u8 valid;
			u8 gear_pos;//p,r,n,d,m,s
			u8 second_gear;
			u8 mode;
			u8 eco_level;
		};
		int gear_pic[] = { en_gid_0icon_gear_0_1_png, en_gid_0icon_gear_0_2_png, en_gid_0icon_gear_0_3_png,
			en_gid_0icon_gear_0_4_png, en_gid_0icon_gear_0_5_png
		};
		struct_gear_msg *p = (struct_gear_msg *)(pbuff);

		ft_image *t = (ft_image*)get_aliase_ui_control("show_gear");

		if (p->gear_pos == 255)
		{
			t->set_visible(false);
		}

		if (p->valid == em_normal)
		{
			static int stic_gear = 0;
			if (stic_gear == p->gear_pos)
				return;
			printf("gear=%d\n", p->gear_pos);

			if (t)
			{
				t->set_texture_id({ 0, gear_pic[p->gear_pos % getArrayLen(gear_pic)] });
			}
			stic_gear = p->gear_pos;
		}
	});

	//胎压信息
	g_msg_host.attach_monitor("tire_pressure", [&](u8*pbuff, int len){
		struct GNU_DEF struct_tire_pressure_msg
		{
			//数据有效
			u8 rr_tire_v_valid:2;
			u8 lr_tire_v_valid : 2;
			u8 rf_tire_v_valid : 2;
			u8 lf_tire_v_valid : 2;
			//漏气
			u8 rr_tire_leak : 2;
			u8 lr_tire_leak : 2;
			u8 rf_tire_leak : 2;
			u8 lf_tire_leak : 2;

			//胎压报警:0正常，  1，胎压过高，  2，胎压过低
			u8 rr_tire_warring : 2;
			u8 lr_tire_warring : 2;
			u8 rf_tire_warring : 2;
			u8 lf_tire_warring : 2;

			u8 lf_vbyte[2];
			u8 rf_vbyte[2];
			u8 lr_vbyte[2];
			u8 rr_vbyte[2];
		};
		struct_tire_pressure_msg * p = (struct_tire_pressure_msg *)pbuff;
		int lf_v = bigend_2_small_byte_end(p->lf_vbyte);
		int rf_v = bigend_2_small_byte_end(p->rf_vbyte);
		int lr_v = bigend_2_small_byte_end(p->lr_vbyte);
		int rr_v = bigend_2_small_byte_end(p->rr_vbyte);

		//printf("lf tire press valid=%d,v=%d\n", p->lf_tire_v_valid, lf_v);
		//printf("rf tire press valid=%d,v=%d\n", p->rf_tire_v_valid, rf_v);
		//printf("lr tire press valid=%d,v=%d\n", p->lr_tire_v_valid, lr_v);
		//printf("rr tire press valid=%d,v=%d\n", p->rr_tire_v_valid, rr_v);

		//左前胎压
		if (p->lf_tire_v_valid==em_normal)
		{
			static int stic_lf_v = 0;
			if (stic_lf_v == lf_v)
				return;

			//float a = (float)lf_v*0.1;

			if (lf_v >= 3480)
				strcpy(show_num, "348.0");
			else if (lf_v <=0)
				strcpy(show_num, "0.0");
			else
				sprintf(show_num, "%d.%d", lf_v / 10,lf_v&10);

			set_property_aliase_value("control_tc_fl_press_txt",show_num);
			
			stic_lf_v = lf_v;

			if (p->lf_tire_warring == em_tire_press_high || p->lf_tire_warring == em_tire_press_low)
			{
				bool b = true;
				set_property_aliase_value("show_tc_tire_fl_press_abnor", &b);
			}
			else
			{
				bool b = false;
				set_property_aliase_value("show_tc_tire_fl_press_abnor", &b);
			}

		}
		else if (p->lf_tire_v_valid == em_invalid)
		{
			strcpy(show_num, "--");
			set_property_aliase_value("control_tc_fl_press_txt", show_num);
		}

		//右前胎压
		if (p->rf_tire_v_valid)
		{
			static int stic_rf_v = 0;
			if (stic_rf_v == rf_v)
				return;

			//float a = (float)rf_v*0.1;
			if (rf_v >= 3480)
				strcpy(show_num, "348.0");
			else if (rf_v <= 0)
				strcpy(show_num, "0.0");
			else
				sprintf(show_num, "%d.%d", rf_v / 10,rf_v%10);

			set_property_aliase_value("control_tc_fr_press_txt", show_num);

			stic_rf_v = rf_v;

			if (p->rf_tire_warring == em_tire_press_high || p->rf_tire_warring == em_tire_press_low)
			{
				bool b = true;
				set_property_aliase_value("show_tc_tire_fr_press_abnor", &b);
			}
			else
			{
				bool b = false;
				set_property_aliase_value("show_tc_tire_fr_press_abnor", &b);
			}

		}
		else if (p->rf_tire_v_valid == em_invalid)
		{
			strcpy(show_num, "--");
			set_property_aliase_value("control_tc_fr_press_txt", show_num);
		}

		//左后胎压
		if (p->lf_tire_v_valid)
		{
			static int stic_rl_v = 0;
			if (stic_rl_v == lr_v)
				return;

			//float a = (float)lr_v*0.1;
			if (rf_v >= 3480)
				strcpy(show_num, "348.0");
			else if (rf_v <= 0)
				strcpy(show_num, "0.0");
			else
				sprintf(show_num, "%d.%d", lr_v / 10,lr_v%10);

			set_property_aliase_value("control_tc_rl_press_txt", show_num);

			stic_rl_v = lr_v;

			if (p->lr_tire_warring == em_tire_press_high || p->lr_tire_warring == em_tire_press_low)
			{
				bool b = true;
				set_property_aliase_value("show_tc_tire_rl_press_abnor", &b);
			}
			else
			{
				bool b = false;
				set_property_aliase_value("show_tc_tire_rl_press_abnor", &b);
			}

		}
		else if (p->lf_tire_v_valid == em_invalid)
		{
			strcpy(show_num, "--");
			set_property_aliase_value("control_tc_rl_press_txt", show_num);
		}

		//右后胎压
		if (p->rr_tire_v_valid)
		{
			static int stic_rr_v = 0;
			if (stic_rr_v == rr_v)
				return;

			//float a = (float)rr_v*0.1;
			if (rr_v >= 3480)
				strcpy(show_num, "348.0");
			else if (rr_v <= 0)
				strcpy(show_num, "0.0");
			else
				sprintf(show_num, "%d.%d", rr_v / 10,rr_v%10);

			set_property_aliase_value("control_tc_rr_press_txt", show_num);

			stic_rr_v = rr_v;

			if (p->rr_tire_warring == em_tire_press_high || p->rr_tire_warring == em_tire_press_low)
			{
				bool b = true;
				set_property_aliase_value("show_tc_tire_rr_press_abnor", &b);
			}
			else
			{
				bool b = false;
				set_property_aliase_value("show_tc_tire_rr_press_abnor", &b);
			}
		}
		else if (p->lf_tire_v_valid == em_invalid)
		{
			strcpy(show_num, "--");
			set_property_aliase_value("control_tc_rr_press_txt",show_num);
		}

	});

	//胎温
	g_msg_host.attach_monitor("tire_temperature", [&](u8*pbuff, int len){
		struct GNU_DEF struct_tire_temperature_msg
		{
			//数据有效
			u8 rr_tire_v_valid : 2;
			u8 lr_tire_v_valid : 2;
			u8 rf_tire_v_valid : 2;
			u8 lf_tire_v_valid : 2;

			//胎压报警:0正常，  1，胎压过高，  2，胎压过低
			u8 rr_tire_temp_warring : 2;
			u8 lr_tire_temp_warring : 2;
			u8 rf_tire_temp_warring : 2;
			u8 lf_tire_temp_warring : 2;

			u8 lf_temp_vbyte[2];
			u8 rf_temp_vbyte[2];
			u8 lr_temp_vbyte[2];
			u8 rr_temp_vbyte[2];
		};
		struct_tire_temperature_msg * p = (struct_tire_temperature_msg *)pbuff;

		int lf_v = bigend_2_small_byte_end(p->lf_temp_vbyte);
		int rf_v = bigend_2_small_byte_end(p->rf_temp_vbyte);
		int lr_v = bigend_2_small_byte_end(p->lr_temp_vbyte);
		int rr_v = bigend_2_small_byte_end(p->rr_temp_vbyte);

		//printf("lf temp valid=%d,vale=%d\n", p->lf_tire_v_valid, lf_v);
		//printf("rf temp valid=%d,vale=%d\n", p->rf_tire_v_valid, rf_v);
		//printf("lr temp valid=%d,vale=%d\n", p->lr_tire_v_valid, lr_v);
		//printf("rr temp valid=%d,vale=%d\n", p->rr_tire_v_valid, rr_v);

		//左前胎  ~50-204
		if (p->lf_tire_v_valid == em_normal)
		{
			static int stic_lf_v = 0;
			if (stic_lf_v == lf_v)
				return;

			//float a = (float)lf_v*0.1;

			if (lf_v <= -500)
				strcpy(show_num, "-50");
			else if (lf_v >= 2040)
				strcpy(show_num,"204");
			else 
				sprintf(show_num, "%d", lf_v/10);

			set_property_aliase_value("control_tc_fl_temp_txt", show_num);


			stic_lf_v = lf_v;

			if (p->lf_tire_temp_warring == em_tire_press_high || p->lf_tire_temp_warring == em_tire_press_low)
			{
				bool b = true;
				set_property_aliase_value("show_tc_tire_fl_temp_high", &b);
			}
			else
			{
				bool b = false;
				set_property_aliase_value("show_tc_tire_fl_temp_high", &b);
			}

		}
		else if (p->lf_tire_v_valid == em_invalid)
		{
			strcpy(show_num, "--");
			set_property_aliase_value("control_tc_fl_temp_txt", show_num);
		}
		//右前胎温
		if (p->rf_tire_v_valid == em_normal)
		{
			static int stic_lf_v = 0;
			if (stic_lf_v == rf_v)
				return;

			//float a = (float)rf_v*0.1;

			if (rf_v <= -500)
				strcpy(show_num, "-50");
			else if (rf_v >= 2040)
				strcpy(show_num, "204");
			else
				sprintf(show_num, "%d", rf_v/10);

			set_property_aliase_value("control_tc_fr_temp_txt", show_num);
			
			stic_lf_v = rf_v;

			if (p->rf_tire_temp_warring == em_tire_press_high || p->rf_tire_temp_warring == em_tire_press_low)
			{
				bool b = true;
				set_property_aliase_value("show_tc_tire_fr_temp_high", &b);
			}
			else
			{
				bool b = false;
				set_property_aliase_value("show_tc_tire_fr_temp_high", &b);
			}

		}
		else if (p->rf_tire_v_valid == em_invalid)
		{
			strcpy(show_num, "--");
			set_property_aliase_value("control_tc_fr_temp_txt",show_num);
		}


		//左后胎温
		if (p->lr_tire_v_valid == em_normal)
		{
			static int stic_lf_v = 0;
			if (stic_lf_v == lr_v)
				return;

			//float a = (float)lr_v*0.1;

			if (lr_v <= -500)
				strcpy(show_num, "-50");
			else if (lr_v >= 2040)
				strcpy(show_num, "204");
			else
				sprintf(show_num, "%d", lr_v/10);

			set_property_aliase_value("control_tc_rl_temp_txt", show_num);

			stic_lf_v = lr_v;

			if (p->lr_tire_temp_warring == em_tire_press_high || p->lr_tire_temp_warring == em_tire_press_low)
			{
				bool b = true;
				set_property_aliase_value("show_tc_tire_rl_temp_high", &b);
			}
			else
			{
				bool b = false;
				set_property_aliase_value("show_tc_tire_rl_temp_high", &b);
			}

		}
		else if (p->rf_tire_v_valid == em_invalid)
		{
			strcpy(show_num, "--");
			set_property_aliase_value("control_tc_rl_temp_txt", show_num);
		}
		//右后胎温
		if (p->rr_tire_v_valid == em_normal)
		{
			static int stic_lf_v = 0;
			if (stic_lf_v == rr_v)
				return;

			//float a = (float)rr_v*0.1;

			if (rr_v <= -500)
				strcpy(show_num, "-50");
			else if (rr_v >= 2040)
				strcpy(show_num, "204");
			else
				sprintf(show_num, "%d", rr_v/10);

			set_property_aliase_value("control_tc_rr_temp_txt", show_num);

			stic_lf_v = rr_v;

			if (p->rr_tire_temp_warring == em_tire_press_high || p->rr_tire_temp_warring == em_tire_press_low)
			{
				bool b = true;
				set_property_aliase_value("show_tc_tire_rr_temp_high", &b);
			}
			else
			{
				bool b = false;
				set_property_aliase_value("show_tc_tire_rr_temp_high", &b);
			}

		}
		else if (p->rf_tire_v_valid == em_invalid)
		{
			strcpy(show_num, "--");
			set_property_aliase_value("control_tc_rr_temp_txt", show_num);
		}
	});



}
