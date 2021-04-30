

#pragma execution_character_set("utf-8")

#include "IndicateBase.h"
#include <string>
#include <stdio.h>
#include <map>
#include "ft_base.h"
#include "af_timer.h"
#include "af_bind.h"
#include "msg_host_n.h"

#include "af_primitive_object.h"
#include "af_material.h"
#include "af_state_manager.h"
//#include "enum_txt_name0.h"
#include "ft_base.h"
#include "afg.h"
#include<chrono>
#include <thread>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include <map>
#include <vector>
#include <math.h>
#include "enum_txt_res_name.h"

using namespace auto_future_utilities;


extern af_timer g_timer;
extern msg_host_n g_msg_host;
int tmid_indc=0,tmid_self=0,tmid_msc_10=0;
int tmid_indc_test = 0;


enum safe_belt_pre
{
	main_safe,
	sub_safe,
	safe_belt_pre_max
};

enum safe_belt_rear
{
	rear_left_safe,
	rear_mid_safe,
	rear_right_safe,
	safe_belt_rear_max
};



//前排安全带
int m_main_safe[safe_belt_pre_max]{0};
//后排安全带
int m_rear_safe[safe_belt_rear_max]{0};


void register_tellte(void)
{
	SingIndicate::getInstance().indicate_init();
}

SingIndicate::~SingIndicate()
{
	for (auto n = map_indicate.begin(); n != map_indicate.end(); n++)
	{
		if (n->second)
		{
			delete n->second;
			n->second = nullptr;
		}
	}
}

void SingIndicate::indicate_init(void)
{
	map_indicate.clear();
	map_sts.clear();

	REG_TT(TT_RearFog_On, light_off,"rear_fog",TT_POWER_ING_OFF_AND_ING_ON);
	REG_TT(TT_HighBeam_On, light_off, "high_beam", TT_POWER_ING_OFF_AND_ING_ON);
	REG_TT(TT_TurnLeft_On, light_off, "turn_left", TT_POWER_ING_OFF_AND_ING_ON);
	REG_TT(TT_TurnRight_On, light_off, "turn_right", TT_POWER_ING_OFF_AND_ING_ON);
	REG_TT(TT_Charge_Fault, light_off, "charge", TT_POWER_ING_ON);
	REG_TT(TT_Brake_Fault, light_off, "brake", TT_POWER_ING_ON);
	REG_TT(TT_EPB_Fault, light_off, "epb_fault", TT_POWER_ING_ON);
	REG_TT(TT_Engine_Fault, light_off, "engine_fault", TT_POWER_ING_ON);
	REG_TT(TT_AirBag_Fault, light_off, "air_bag", TT_POWER_ING_ON);
	REG_TT(TT_ABS_Fault, light_off, "abs", TT_POWER_ING_ON);
	REG_TT(TT_FuelOilLow_On, light_off, "fuel_low", TT_POWER_ING_ON);
	//REG_TT(TT_SafetyBeltMain_On, light_off, "safe_main", TT_POWER_ING_ON);
	//REG_TT(TT_SafetyBeltSecondMid_On, light_off, "safe_rear", TT_POWER_ING_ON);

	REG_TT(TT_Position_On, light_off, "position", TT_POWER_ING_ON);
	REG_TT(TT_CoolantTmpHigh_On, light_off, "temp_high", TT_POWER_ING_ON);
	REG_TT(TT_ESP_On, light_off, "esp_fault", TT_POWER_ING_ON);
	REG_TT(TT_ESP_Off, light_off, "esp_off", TT_POWER_ING_ON);
	REG_TT(TT_StartStop_On, light_off, "start_stop", TT_POWER_ING_ON);
	REG_TT(TT_StartStop_Fault, light_off, "start_stop_fault", TT_POWER_ING_ON);
	REG_TT(TT_TirePressure_Fault, light_off, "tire_fault", TT_POWER_ING_ON);
	REG_TT(TT_EngineOilLow_On, light_off, "engine_oil", TT_POWER_ING_ON);


	reg_telltle_light();
	register_light_timer();
	self_check();
}

void SingIndicate::reg_telltle_light(void)
{
	//前排安全带指示灯
	map_indicate[TT_SafetyBeltMain_On] = new DataIndicate(TT_SafetyBeltMain_On, light_off, 
		"safe_main", TT_POWER_ING_ON);
	map_sts[TT_SafetyBeltMain_On] = light_off;


	g_msg_host.attach_monitor("safe_main", [&](u8*pbuff, int len){
		static int tmp = 0;

		int temp = *pbuff;
		if (temp > blink_1hz)
			return;

		m_main_safe[main_safe] = temp;

		if (m_main_safe[sub_safe] == blink_1hz || m_main_safe[main_safe] == blink_1hz)
		{
			temp = blink_1hz;
		}
		else if (m_main_safe[sub_safe] == light_on || m_main_safe[main_safe] == light_on)
		{
			temp = light_on;
		}
		else if (m_main_safe[sub_safe] == light_off && m_main_safe[main_safe] == light_off)
		{
			temp = light_off;
		}

		if (temp == tmp)
			return;

		deal_show_indicate(TT_SafetyBeltMain_On, TT_light_sts(tmp));
		tmp = temp;

	});

	g_msg_host.attach_monitor("safe_main_sub", [&](u8*pbuff, int len){
		static int tmp = 0;

		int temp = *pbuff;
		if (temp > blink_1hz || temp==tmp)
			return;

		m_main_safe[sub_safe] = temp;
		tmp = temp;

	});


	//后排安全带指示灯
	map_indicate[TT_SafetyBeltSecondMid_On] = new DataIndicate(TT_SafetyBeltSecondMid_On, light_off,
		"safe_rear", TT_POWER_ING_ON);
	map_sts[TT_SafetyBeltSecondMid_On] = light_off;

	//后排左安全带
	g_msg_host.attach_monitor("safe_rear_left", [&](u8*pbuff, int len){
		static int tmp = 0;

		int temp = *pbuff;
		if (temp > blink_1hz)
			return;

		m_rear_safe[rear_left_safe] = temp;

		if (m_rear_safe[rear_right_safe] == blink_1hz || m_rear_safe[rear_mid_safe] == blink_1hz || m_rear_safe[rear_left_safe] == blink_1hz)
		{
			temp = blink_1hz;
		}
		else if (m_rear_safe[rear_right_safe] == light_on || m_rear_safe[rear_mid_safe] == light_on || m_rear_safe[rear_left_safe] == light_on)
		{
			temp = light_on;
		}
		else if (m_rear_safe[rear_right_safe] == light_off && m_rear_safe[rear_mid_safe] == light_off && m_rear_safe[rear_left_safe] == light_off)
		{
			temp = light_off;
		}

		if (temp == tmp)
			return;

		deal_show_indicate(TT_SafetyBeltSecondMid_On, TT_light_sts(tmp));
		tmp = temp;

	});

	//后排中安全带
	g_msg_host.attach_monitor("safe_rear", [&](u8*pbuff, int len){
		static int tmp = 0;
		int temp = *pbuff;
		if (temp > blink_1hz || temp ==tmp)
			return;
		m_rear_safe[rear_mid_safe] = temp;
		tmp = temp;
	});
	//后排右安全带
	g_msg_host.attach_monitor("safe_rear_right", [&](u8*pbuff, int len){
		static int tmp = 0;
		int temp = *pbuff;
		if (temp > blink_1hz || temp == tmp)
			return;
		m_rear_safe[rear_right_safe] = temp;
		tmp = temp;
	});


}

void SingIndicate::call_back_ms(void)
{
	
}

void SingIndicate::deal_with_1hz(bool visable)
{
	for (auto n = map_indicate.begin(); n != map_indicate.end(); n++)
	{
		if (n->second->sts == blink_1hz)
		{
			ft_base *p = (ft_base *)get_aliase_ui_control(n->second->name);
			if (p)
				p->set_visible(visable);
		}
	}
}

void SingIndicate::deal_with_2hz(bool visable)
{
	for (auto n = map_indicate.begin(); n != map_indicate.end(); n++)
	{
		if (n->second->sts == blink_2hz)
		{
			ft_base *p = (ft_base *)get_aliase_ui_control(n->second->name);
			if (p)
				p->set_visible(visable);
		}
	}
}

void SingIndicate::deal_with_hz_sts(TT_light_sts sts, bool visable)
{

}


void SingIndicate::need_change(TT_light_sts sts)
{
	if (sts == blink_1hz)
	{
		for (auto n = map_indicate.begin(); n != map_indicate.end(); n++)
		{
			if (is_self_check && is_self_check_id(n->first))
			{
				;
			}
			else
			{
					if (n->second->sts != map_sts[n->first])
						n->second->sts = map_sts[n->first];
			}
			//else if ( (!is_self_check_id(n->first)))
			//{
			//	if ((n->second->sts != map_sts[n->first])
			//		n->second->sts = map_sts[n->first];
			//}

			//else if (!is_self_check  && is_self_check(n->first))
			//{
			//	if ((n->second->sts != map_sts[n->first])
			//		n->second->sts = map_sts[n->first];
			//}

		}
	}
	else if (sts == blink_2hz)
	{
		for (auto n = map_indicate.begin(); n != map_indicate.end(); n++)
		{
			if (n->second->sts == blink_2hz || map_sts[n->first] == blink_2hz)
			{

				if (is_self_check && is_self_check_id(n->first))
				{
					;
				}
				else
				{
					if (n->second->sts != map_sts[n->first])
						n->second->sts = map_sts[n->first];
				}
			}
		}
	}
}

bool SingIndicate::is_self_check_id(TTBaseID id)
{	
	return (id != TT_RearFog_On &&id != TT_HighBeam_On &&id != TT_TurnLeft_On
		&& id != TT_TurnRight_On && id != TT_Position_On &&id != TT_StartStop_On
		&&id != TT_StartStop_Fault
		);
}

void SingIndicate::update_indicate(TTBaseID id, TT_light_sts sts)
{

}

void SingIndicate::register_light_timer(void)
{
	tmid_self = g_timer.register_timer_ex([&](int n){
		is_self_check = false;
		for (auto iter = map_indicate.begin(); iter != map_indicate.end(); iter++)
		{
			if (is_self_check_id(iter->first))
			{

				if (iter->second->sts != map_sts[iter->first])
				{
					TT_light_sts sts = map_sts[iter->first];

					if (sts == light_off || sts == light_on)
					{

						if (iter->first == TT_FuelOilLow_On)
						{
							if (sts)
							{
								af_vec3 v3{ 1.0, 1.0, 0.0 };
								set_property_aliase_value("control_fuel_low_clr", &v3);
							}
							else
							{
								af_vec3 v3{ 1.0, 1.0, 1.0 };
								set_property_aliase_value("control_fuel_low_clr", &v3);
							}
							continue;

						}
						bool b = sts;
						set_property_aliase_value(iter->second->name, &b);
						iter->second->sts = sts;
					}
				}
				
			}
		}
		g_timer.deactive_time_ex(tmid_self);
		printf("self check end\n");

	});


	tmid_indc = g_timer.register_timer_ex([&](int n){
		//deal with long show
		//deal_with_hz_sts(light_off, false);
		//deal_with_hz_sts(light_on, true);

		//deal with 1hz
		m_cout_1_hz += 50;

		if (m_cout_1_hz <= 500 && m_cout_1_hz > 0)
		{
			deal_with_1hz(true);
			//deal_with_hz_sts(blink_1hz, true);

		}
		else if (m_cout_1_hz > 500 && m_cout_1_hz < 1000)
		{
			deal_with_1hz(false);
			//deal_with_hz_sts(blink_1hz, false);
		}

		if (m_cout_1_hz >= 1000)
		{
			m_cout_1_hz = 0;
			
			bool b = false;
			b = map_indicate[TT_FuelOilLow_On]->sts;
			need_change(blink_1hz);

			//处理燃油灯
			if (b != map_indicate[TT_FuelOilLow_On]->sts && map_indicate[TT_FuelOilLow_On]->sts)
			{
				af_vec3 v3{1.0,1.0,0.0};
				set_property_aliase_value("control_fuel_low_clr", &v3);
			}
			else if (b != map_indicate[TT_FuelOilLow_On]->sts && map_indicate[TT_FuelOilLow_On]->sts == light_off)
			{
				af_vec3 v3{ 1.0, 1.0, 1.0 };
				set_property_aliase_value("control_fuel_low_clr", &v3);
			}
		}

		//deal with 2hz
		m_cout_2_hz += 50;
		if (m_cout_2_hz <= 250 && m_cout_2_hz > 0)
		{
			deal_with_2hz(true);
			//deal_with_hz_sts(blink_2hz, true);
		}
		else if (m_cout_2_hz > 250 && m_cout_2_hz < 500)
		{
			deal_with_2hz(false);
			//deal_with_hz_sts(blink_2hz, false);
		}

		if (m_cout_2_hz >= 500)
		{
			m_cout_2_hz = 0;
			need_change(blink_2hz);
		}
	});
}

void SingIndicate::self_check(void)
{
	if (g_timer.is_actived_timer_ex(tmid_indc))
		g_timer.deactive_time_ex(tmid_indc);
	g_timer.active_timer_ex(tmid_indc, 50);

	if (g_timer.is_actived_timer_ex(tmid_self))
		g_timer.deactive_time_ex(tmid_self);
	g_timer.active_timer_ex(tmid_self,3000);

	is_self_check = true;
	for (auto n = map_indicate.begin(); n != map_indicate.end();n++)
	{
		if (is_self_check_id(n->first))
		{
			ft_base *p = (ft_base *)get_aliase_ui_control(n->second->name);
			if (p)
				p->set_visible(true);
			n->second->sts = light_on;
		}
		else
		{
			ft_base *p = (ft_base *)get_aliase_ui_control(n->second->name);
			if (p)
				p->set_visible(false);
			n->second->sts = light_off;
		}
	}
}

void SingIndicate::deal_indicate(TTBaseID id, TT_light_sts sts)
{
	//update data;

	bool b = true;
	for (auto n = map_sts.begin(); n != map_sts.end();n++)
	{
		if (n->second >= blink_1hz && n->second < blink_max)
		{
			b = false;
			break;
		}
	}

	bool a = true;
	for (auto n = map_indicate.begin(); n != map_indicate.end(); n++)
	{
		if (n->second->sts >= blink_1hz && n->second->sts < blink_max)
		{
			a = false;
			break;
		}
	}
	//no blink but now need
	if (a && b && (sts >= blink_1hz && sts < blink_max))
	{
		if (g_timer.is_actived_timer_ex(tmid_indc))
		{
			g_timer.active_timer_ex(tmid_indc, 50);	
			m_cout_1_hz = 1000;
			m_cout_2_hz = 1000;
			ft_base *p = (ft_base *)get_aliase_ui_control(map_indicate[id]->name);
			if (p)
			{
				p->set_visible(true);
			}
		}
	}

	map_sts[id] = sts;
	//0 to 1, 1 to 0, change now
	if (map_indicate[id]->sts != map_sts[id])
	{
		if (map_indicate[id]->sts <= light_on && map_sts[id] <= light_on)
		{
			//ft_base *p = (ft_base *)get_aliase_ui_control(map_indicate[id]->name);
			//if (p)
			//{
			//	p->set_visible(bool(sts));
			//}
			set_property_aliase_value(map_indicate[id]->name, &sts);
		}
	}


}

void SingIndicate::deal_show_indicate(TTBaseID id, TT_light_sts sts)
{
	//switch and change sts
	if (map_indicate.find(id) == map_indicate.end())
		return;
	deal_indicate(id, sts);
}

void SingIndicate::pow_sts_change(TT_power_sts sts)
{
	if (sts == TT_POWER_ING_OFF)
	{
		g_timer.deactive_time_ex(tmid_indc);
		for (auto n = map_indicate.begin(); n != map_indicate.end();n++)
		{
			if (n->second->pow_sts != TT_POWER_ING_OFF_AND_ING_ON)
			{
				ft_base *p = (ft_base *)get_aliase_ui_control(n->second->name);
				if (p)
				{
					p->set_visible(false);
				}
			}
		}
	}
}

#ifdef WIN32
#include <time.h>
void SingIndicate::test_indicate(void)
{
	//static bool stic_b = true;
	//if (stic_b)
	//{
	//	srand(time(NULL));

	//	tmid_indc_test = g_timer.register_timer_ex([&](int f){
	//		int id = rand() % 42 + 1;
	//		int sts = rand() % 4;
	//		deal_show_indicate(TTBaseID(id), TT_light_sts(sts));
	//	});
	//	g_timer.deactive_time_ex(tmid_indc_test);
	//	stic_b = false;

	//	//for (auto &n = map_indicate.begin(); n != map_indicate.end(); n++)
	//	//{
	//	//	if (n->first < 26)
	//	//	{
	//	//		deal_show_indicate(n->first, blink_1hz);
	//	//	}
	//	//	else if (n->first >26)
	//	//	{
	//	//		deal_show_indicate(n->first, blink_2hz);
	//	//	}
	//	//}
	//	//g_timer.active_timer_ex(tmid_indc_test, 300);

	//	deal_show_indicate(TT_TirePressure_Fault, blink_1hz);
	//	deal_show_indicate(TT_AirBag_Fault, blink_2hz);
	//	deal_show_indicate(TT_RearFog_On, blink_1hz);
	//}
	//static bool stic_b = true;
	//for (auto n = map_indicate.begin(); n != map_indicate.end(); n++)
	//{
	//	if (stic_b)
	//		deal_show_indicate(n->first, light_on);
	//	else
	//		deal_show_indicate(n->first, light_off);
	//}
	//stic_b = !stic_b;
	//deal_show_indicate(TT_RearFog_On, blink_1hz);
	ft_image *p = static_cast<ft_image *> (get_aliase_ui_control("show_TT_fuel_low"));
	p->set_visible(true);
	af_vec3 v3{ 1.0, 1.0, 0.0 };
	set_property_aliase_value("control_fuel_low_clr", &v3);
}
#endif