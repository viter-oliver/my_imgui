
#pragma execution_character_set("utf-8")


#include "NormalDlg.h"
#include "af_bind.h"
#include "af_primitive_object.h"
#include "af_material.h"
#include "af_state_manager.h"
#include "PageManager.h"
#include "pop_win.h"
#include "AppDef.h"
#include "ft_textblock.h"
//#include "msg_host_n.h"
#include "af_timer.h"
//using namespace auto_future_utilities;
using namespace auto_future;
extern af_timer g_timer;

extern Menu_admin menu_ani;
extern std::vector<Em_PoPId> vec_fault_list;

int tmid_back_tc = 0;

//行车电脑
REGISTER_CLASS(TC_menu, PageTCDlg)
void PageTCDlg::Init(void)
{
	tmid_back_tc=g_timer.register_timer_ex([&](int tid){
		g_timer.deactive_time_ex(tmid_back_tc);
		PageBigNavi::getInstance()->hide();
		PageAdas::getInstance()->hide();
		if (PageBigNavi::getInstance()->needShow)
		{
			PageBigNavi::getInstance()->show();
			return;
		}
		else if (PageAdas::getInstance()->needShow)
		{
			PageAdas::getInstance()->show();
			return;
		}
		PageManager::getInstance()->pageSwitchTo(TC_menu);
		printf("tmid_back_tc call back\n");
	});
}

void PageTCDlg::onToBeginStage(void)
{
	printf(" PageTCDlg::onToBeginStage\n");
	bool b = false;
	set_property_aliase_value("show_tc_bg", &b);

	b = isShowUp;
	set_property_aliase_value("show_tc_up", &b);
	set_property_aliase_value("show_tc_car", &b);
	b = !b;
	set_property_aliase_value("show_tc_down", &b);
	set_property_aliase_value("show_tc_base_value", &b);
}

void PageTCDlg::onToLeaveStage(void)
{
	printf(" PageTCDlg::onToLeaveStage\n");
}

bool PageTCDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		switch (key)
		{
		case key_up:
			show_up(true);
			break;
		case key_down:
			show_up(false);
			break;
		case key_left:
			break;
		case key_right:
			PageManager::getInstance()->pageSwitchTo(main_menu);
			break;
		default:
			break;
		}
	}

	return true;
}
void PageTCDlg::show_up(bool visable)
{
	ft_base *p=(ft_base *)get_aliase_ui_control("show_tc_up");

	if (p->is_visible() && !visable || (!(p->is_visible() ) ) && (visable) )
	{
		return;
	}

	if (!visable)
	{
		bool b = true;
		set_property_aliase_value("show_tc_up", &b);
		set_property_aliase_value("show_tc_car", &b);
		b = false;
		set_property_aliase_value("show_tc_down", &b);
		set_property_aliase_value("show_tc_base_value", &b);
		isShowUp = true;
	}
	else
	{
		bool b = false;
		set_property_aliase_value("show_tc_up", &b);
		set_property_aliase_value("show_tc_car", &b);
		b = true;
		set_property_aliase_value("show_tc_down", &b);
		set_property_aliase_value("show_tc_base_value", &b);
		isShowUp = false;
	}
}

REGISTER_CLASS(TC_in_main_menu, PageTCMainMenuDlg)
void PageTCMainMenuDlg::Init(void)
{

}

void PageTCMainMenuDlg::onToBeginStage(void)
{
	printf(" PageTCMainMenuDlg::onToBeginStage\n");
	g_timer.active_timer_ex(tmid_back_tc, 5000);

	bool b = true;
	set_property_aliase_value("show_tc_bg", &b);

	b = isShowUp;
	set_property_aliase_value("show_tc_up", &b);
	set_property_aliase_value("show_tc_car", &b);
	b = !b;
	set_property_aliase_value("show_tc_down", &b);
	set_property_aliase_value("show_tc_base_value", &b);
}

void PageTCMainMenuDlg::onToLeaveStage(void)
{
	printf(" PageTCMainMenuDlg::onToLeaveStage\n");
}

bool PageTCMainMenuDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		g_timer.active_timer_ex(tmid_back_tc, 5000);

		switch (key)
		{
		case key_up:
			show_up(true);
			break;
		case key_down:
			show_up(false);
			break;
		case key_left:
			PageManager::getInstance()->pageSwitchTo(main_menu);
			break;
		case key_right:
			break;
		default:
			break;
		}
	}

	return true;
}

void PageTCMainMenuDlg::show_up(bool visable)
{
	ft_base *p = (ft_base *)get_aliase_ui_control("show_tc_up");

	if (p->is_visible() && !visable || (!(p->is_visible())) && (visable))
	{
		return;
	}

	if (!visable)
	{
		bool b = true;
		set_property_aliase_value("show_tc_up", &b);
		set_property_aliase_value("show_tc_car", &b);
		b = false;
		set_property_aliase_value("show_tc_down", &b);
		set_property_aliase_value("show_tc_base_value", &b);
		isShowUp = true;
	}
	else
	{
		bool b = false;
		set_property_aliase_value("show_tc_up", &b);
		set_property_aliase_value("show_tc_car", &b);
		b = true;
		set_property_aliase_value("show_tc_down", &b);
		set_property_aliase_value("show_tc_base_value", &b);
		isShowUp = false;
	}
}


//主菜单
REGISTER_CLASS(main_menu, PageMainMenuDlg)
void PageMainMenuDlg::onToBeginStage(void)
{
	printf(" PageMainMenuDlg::onToBeginStage\n");
	g_timer.active_timer_ex(tmid_back_tc, 5000);
}

void PageMainMenuDlg::onToLeaveStage(void)
{
	printf(" PageMainMenuDlg::onToLeaveStage\n");
}
bool PageMainMenuDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		int d = id;
		g_timer.active_timer_ex(tmid_back_tc,5000);
		switch (key)
		{
		case key_up:
			menu_ani.switch_to_pre();
			id = menu_ani.getCurrentMenuId();
			break;
		case key_down:
			menu_ani.switch_to_next();
			id = menu_ani.getCurrentMenuId();
			break;
		case key_left:
			break;
		case key_right:
			break;
		case key_ok:
			if (id == main_menu_media)
			{
				if (DATA_STORE.get_media_music_fm() == media_2nd_usb_music)
					PageManager::getInstance()->pageSwitchTo(media_2nd_usb_music);
				else if (DATA_STORE.get_media_music_fm() == media_2nd_radio_fm)
					PageManager::getInstance()->pageSwitchTo(media_2nd_radio_fm);
				else if (DATA_STORE.get_media_music_fm() == id_none)
					PageManager::getInstance()->pageSwitchTo(media);
			}
			else if (id==main_menu_setting)
				PageManager::getInstance()->pageSwitchTo(setting_menu_1st);
			else if (id == main_menu_tc)
			{
				PageManager::getInstance()->pageSwitchTo(TC_in_main_menu);
			}
			else if (id==main_menu_fault_list)
				PageManager::getInstance()->pageSwitchTo(fault_list);
			break;
		default:
			break;
		}
	}

	return true;
}

//设置界面
REGISTER_CLASS(setting_menu_1st, PageSettingDlg)
void PageSettingDlg::Init(void)
{

}

void PageSettingDlg::onToBeginStage(void)
{
	printf("PageSettingDlg::onToBeginStage\n");
	g_timer.active_timer_ex(tmid_back_tc, 5000);

}

void PageSettingDlg::onToLeaveStage(void)
{
	printf("PageSettingDlg::onToLeaveStage\n");
	//g_timer.deactive_time_ex(tmid_back_tc);
}

void PageSettingDlg::show_cursor(Em_setting id)
{
	if (id == setting_2nd_map)
	{
		bool b = true;
		set_property_aliase_value("show_cursor_map", &b);
		b = false;
		set_property_aliase_value("show_cursor_reset", &b);
		set_property_aliase_value("show_cursor_factory", &b);
	}
	else if (id == setting_2nd_reset)
	{
		bool b = true;
		set_property_aliase_value("show_cursor_reset", &b);
		b = false;
		set_property_aliase_value("show_cursor_map", &b);
		set_property_aliase_value("show_cursor_factory", &b);
	}
	else if (id == setting_2nd_factory)
	{
		bool b = true;
		set_property_aliase_value("show_cursor_factory", &b);
		b = false;
		set_property_aliase_value("show_cursor_map", &b);
		set_property_aliase_value("show_cursor_reset", &b);
	}

}

bool PageSettingDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		int d = id;
		g_timer.active_timer_ex(tmid_back_tc, 5000);
		switch (key)
		{
		case key_down:
			d = (++d) % setting_2nd_max;
			id = Em_setting(d);
			show_cursor(id);
			break;
		case key_up:
			d = (--d) < 0 ? (setting_2nd_max-1) : d;
			id = Em_setting(d);
			show_cursor(id);
			break;
		case key_left:
			PageManager::getInstance()->pageSwitchTo(main_menu);
			break;
		case key_right:
			break;
		case key_ok:
			if (id == setting_2nd_map)
				PageManager::getInstance()->pageSwitchTo(setting_2nd_navi);
			else if (id == setting_2nd_reset)
				PageManager::getInstance()->pageSwitchTo(setting_2nd_reset_TC_data);
			else if (id == setting_2nd_factory)
			{
				PageManager::getInstance()->pageSwitchTo(setting_2nd_reset_factory);
			}
			break;
		default:
			break;
		}
	}

	return true;

}

//还没有联调多媒体
REGISTER_CLASS(media, PageMediaDlg)
void PageMediaDlg::Init(void)
{

}

void PageMediaDlg::onToBeginStage(void)
{
	printf("PageMediaDlg::onToBeginStage\n");
	g_timer.active_timer_ex(tmid_back_tc, 5000);

}

void PageMediaDlg::onToLeaveStage(void)
{
	printf("PageMediaDlg::onToLeaveStage\n");
	bool b = false;
	set_property_aliase_value("show_media_no_signal", &b);
	//g_timer.deactive_time_ex(tmid_back_tc);
}

bool PageMediaDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		g_timer.active_timer_ex(tmid_back_tc, 5000);
		switch (key)
		{
		case key_left:
			PageManager::getInstance()->pageSwitchTo(main_menu);
			break;
		default:
			break;
		}
	}
	return true;
}
//多媒体音乐
REGISTER_CLASS(media_2nd_usb_music, PageMediaMusicDlg)
void PageMediaMusicDlg::Init(void)
{

}

void PageMediaMusicDlg::onToBeginStage(void)
{
	printf("PageMediaMusicDlg::onToBeginStage\n");
	g_timer.active_timer_ex(tmid_back_tc, 5000);
	bool b = true;
	set_property_aliase_value("show_media_music", &b);
	set_property_aliase_value("show_media_menu_music", &b);

	set_property_aliase_value("control_media_menu_song", (void *)(DATA_STORE.get_song_name().c_str()) );

	b = false;
	set_property_aliase_value("show_media_menu_fm", &b);
}

void PageMediaMusicDlg::onToLeaveStage(void)
{
	printf("PageMediaMusicDlg::onToLeaveStage\n");
	//g_timer.deactive_time_ex(tmid_back_tc);
}

bool PageMediaMusicDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		g_timer.active_timer_ex(tmid_back_tc, 5000);
		switch (key)
		{
		case key_left:
			PageManager::getInstance()->pageSwitchTo(main_menu);
			break;
		default:
			break;
		}
	}
	return true;
}

REGISTER_CLASS(media_2nd_radio_fm, PageMediaRadioDlg)
void PageMediaRadioDlg::Init(void)
{

}

void PageMediaRadioDlg::onToBeginStage(void)
{
	printf("PageMediaMusicDlg::onToBeginStage\n");
	g_timer.active_timer_ex(tmid_back_tc, 5000);
	bool b = true;
	set_property_aliase_value("show_media_fm_am", &b);
	set_property_aliase_value("show_media_menu_fm", &b);
	b = false;
	set_property_aliase_value("show_media_menu_music", &b);

	int frq = DATA_STORE.get_fm_freq();
	char buf[0x20] = { 0 };
	sprintf(buf,"%d.%d",frq/10,frq%10);
	set_property_aliase_value("control_menu_fm_freq", &frq);
}

void PageMediaRadioDlg::onToLeaveStage(void)
{
	printf("PageMediaMusicDlg::onToLeaveStage\n");
	//g_timer.deactive_time_ex(tmid_back_tc);
	bool b = false;
	set_property_aliase_value("show_media_menu_fm", &b);
	//set_property_aliase_value("show_media_fm_am", &b);


}

bool PageMediaRadioDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		g_timer.active_timer_ex(tmid_back_tc, 5000);
		switch (key)
		{
		case key_left:
			PageManager::getInstance()->pageSwitchTo(main_menu);
			break;
		default:
			break;
		}
	}
	return true;
}


//故障列表
REGISTER_CLASS(fault_list, PageFaultListDlg)
void PageFaultListDlg::Init(void)
{

}

void PageFaultListDlg::onToBeginStage(void)
{
	bool b = true;
	if (vec_fault_list.size() >= 1)
		b = true;
	else
		b = false;

	set_property_aliase_value("show_fault_list", &b);
	printf("PageFaultListDlg::onToBeginStage\n");

	g_timer.active_timer_ex(tmid_back_tc, 5000);
}

void PageFaultListDlg::onToLeaveStage(void)
{
	printf("PageFaultListDlg::onToLeaveStage\n");
	//g_timer.deactive_time_ex(tmid_back_tc);

}

bool PageFaultListDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		g_timer.active_timer_ex(tmid_back_tc, 5000);

		switch (key)
		{
		case key_left:
			PageManager::getInstance()->pageSwitchTo(main_menu);
			break;
		default:
			break;
		}
	}
	return true;
}


//设置界面的导航
REGISTER_CLASS(setting_2nd_navi, PageNaviDlg)
void PageNaviDlg::Init(void)
{
}

void PageNaviDlg::onToBeginStage(void)
{
	printf("PageNaviDlg::onToLeaveStage\n");
	g_timer.active_timer_ex(tmid_back_tc, 5000);
}

void PageNaviDlg::onToLeaveStage(void)
{
	printf("PageNaviDlg::onToLeaveStage\n");
	//g_timer.deactive_time_ex(tmid_back_tc);

}

void PageNaviDlg::show_cursor(Em_map id)
{
	bool b = true;
	if (id != map_esy)
		b = false;

	set_property_aliase_value("show_map_easy", &b);

}

bool PageNaviDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		g_timer.active_timer_ex(tmid_back_tc, 5000);

		int d = id;
		switch (key)
		{
		case key_left:
			PageManager::getInstance()->pageSwitchTo(setting_menu_1st);
			break;
		case key_down:
			d = (++d) % map_max;
			id = Em_map(d);
			show_cursor(id);
			break;
		case key_up:
			d = (--d) < 0 ? (map_max - 1) : d;
			id = Em_map(d);
			show_cursor(id);
			break;
		case key_ok:
			if (id == map_big)
			{
				if (PageBigNavi::getInstance()->needShow)
					PageBigNavi::getInstance()->show();
			}
			break;
		default:
			break;
		}
	}
	return true;
}

//清零界面
REGISTER_CLASS(setting_2nd_reset_TC_data, PageResetTcDlg)
void PageResetTcDlg::Init(void)
{
}

void PageResetTcDlg::onToBeginStage(void)
{
	printf("PageResetTcDlg::onToLeaveStage\n");
	g_timer.active_timer_ex(tmid_back_tc, 5000);
}

void PageResetTcDlg::onToLeaveStage(void)
{
	printf("PageResetTcDlg::onToLeaveStage\n");
	//g_timer.deactive_time_ex(tmid_back_tc);
}

void PageResetTcDlg::show_cursor(Em_tc_trip id)
{
	bool b = true;
	if (id != tc_tripa)
		b = false;

	set_property_aliase_value("show_tc_tripa_cursor", &b);
}

bool PageResetTcDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		g_timer.active_timer_ex(tmid_back_tc, 5000);

		int d = id;
		switch (key)
		{
		case key_left:
			PageManager::getInstance()->pageSwitchTo(setting_menu_1st);
			break;
		case key_down:
			d = (++d) % tc_max;
			id = Em_tc_trip(d);
			show_cursor(id);
			break;
		case key_up:
			d = (--d) < 0 ? (tc_max - 1) : d;
			id = Em_tc_trip(d);
			show_cursor(id);
			break;
		case key_ok:
			if (id == tc_tripa)
				PageManager::getInstance()->pageSwitchTo(setting_3rd_reset_tripa);
			else if (id==tc_avg)
				PageManager::getInstance()->pageSwitchTo(setting_3rd_reset_avg);

			break;
		default:
			break;
		}
	}
	return true;
}


//清零tripa
REGISTER_CLASS(setting_3rd_reset_tripa, PageResetTripaDlg)
void PageResetTripaDlg::Init(void)
{
}

void PageResetTripaDlg::onToBeginStage(void)
{
	printf("PageResetTripaDlg::onToBeginStage\n");
	g_timer.active_timer_ex(tmid_back_tc, 5000);
}

void PageResetTripaDlg::onToLeaveStage(void)
{
	printf("PageResetTripaDlg::onToLeaveStage\n");
	//g_timer.deactive_time_ex(tmid_back_tc);

}

void PageResetTripaDlg::show_cursor(Em_decide id)
{
	bool b = true;
	if (id != decide_ok)
		b = false;

	set_property_aliase_value("show_clear_tripa_cursor", &b);
}

bool PageResetTripaDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		g_timer.active_timer_ex(tmid_back_tc, 5000);

		int d = id;
		switch (key)
		{
		case key_left:
			PageManager::getInstance()->pageSwitchTo(setting_2nd_reset_TC_data);
			break;
		case key_down:
			d = (++d) % decide_max;
			id = Em_decide(d);
			show_cursor(id);
			break;
		case key_up:
			d = (--d) < 0 ? (decide_max - 1) : d;
			id = Em_decide(d);
			show_cursor(id);
			break;
		case key_ok:
			if (id == decide_ok)
			{
				PageManager::getInstance()->pageSwitchTo(setting_2nd_reset_TC_data);
			}				
			else if (id == decide_cancle)
			{
				PageManager::getInstance()->pageSwitchTo(setting_2nd_reset_TC_data);
			}
			break;
		default:
			break;
		}
	}
	return true;
}

//清零avg
REGISTER_CLASS(setting_3rd_reset_avg, PageResetAvgDlg)
void PageResetAvgDlg::Init(void)
{
}

void PageResetAvgDlg::onToBeginStage(void)
{
	printf("PageResetAvgDlg::onToBeginStage\n");
	g_timer.active_timer_ex(tmid_back_tc, 5000);
}

void PageResetAvgDlg::onToLeaveStage(void)
{
	printf("PageResetAvgDlg::onToLeaveStage\n");
	//g_timer.deactive_time_ex(tmid_back_tc);

}

void PageResetAvgDlg::show_cursor(Em_decide id)
{
	bool b = true;
	if (id != decide_ok)
		b = false;

	set_property_aliase_value("show_clear_avg_cursor", &b);
}

bool PageResetAvgDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		int d = id;
		g_timer.active_timer_ex(tmid_back_tc, 5000);

		switch (key)
		{
		case key_left:
			PageManager::getInstance()->pageSwitchTo(setting_2nd_reset_TC_data);
			break;
		case key_down:
			d = (++d) % decide_max;
			id = Em_decide(d);
			show_cursor(id);
			break;
		case key_up:
			d = (--d) < 0 ? (decide_max - 1) : d;
			id = Em_decide(d);
			show_cursor(id);
			break;
		case key_ok:
			if (id == decide_ok)
			{
				PageManager::getInstance()->pageSwitchTo(setting_2nd_reset_TC_data);
			}
			else if (id == decide_cancle)
			{
				PageManager::getInstance()->pageSwitchTo(setting_2nd_reset_TC_data);
			}
			break;
		default:
			break;
		}
	}
	return true;
}

//恢复出厂设置
int tmid_factory = 0;

REGISTER_CLASS(setting_2nd_reset_factory, PageResetFactoryDlg)
void PageResetFactoryDlg::Init(void)
{
	tmid_factory=g_timer.register_timer_ex([&](int tid){
		bool b = false;
		set_property_aliase_value("show_setting_3rd_reset_ok", &b);
		PageManager::getInstance()->pageSwitchTo(setting_menu_1st);
		is_show_ok = false;
		g_timer.deactive_time_ex(tmid_factory);
		 });
}

void PageResetFactoryDlg::onToBeginStage(void)
{
	printf("PageResetFactoryDlg::onToBeginStage\n");
	g_timer.active_timer_ex(tmid_back_tc, 5000);
}

void PageResetFactoryDlg::onToLeaveStage(void)
{
	printf("PageResetFactoryDlg::onToLeaveStage\n");
	is_show_ok = false;
	//g_timer.deactive_time_ex(tmid_back_tc);

}


bool PageResetFactoryDlg::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		if (is_show_ok)
			return true;

		g_timer.active_timer_ex(tmid_back_tc, 5000);

		switch (key)
		{

		case key_left:
			PageManager::getInstance()->pageSwitchTo(setting_menu_1st);
			break;
		case key_ok:
			if (id == decide_ok)
			{
				bool b = true;
				set_property_aliase_value("show_setting_3rd_reset_ok", &b);
				b = false;
				set_property_aliase_value("show_setting_2nd_reset", &b);
				g_timer.active_timer_ex(tmid_factory, 500);
				is_show_ok = true;
			}
			else if (id == decide_cancle)
			{
				PageManager::getInstance()->pageSwitchTo(setting_menu_1st);
			}
			break;
		default:
			break;
		}
	}
	return true;
}

#include "client.h"
#include "debug_var_set.h"

extern unsigned char *img_p;
extern bool socket_img_ok;
int tmid_navi_img;
//全屏大导航
void PageBigNavi::init(void)
{
	//导航数据初始化
#ifdef IMGUI_WAYLAND
	socket_init();

	//导航线程
	std::thread th_navi([&](){
		socket_navi();
	});
	th_navi.detach();
#endif

	tmid_navi_img=g_timer.register_timer_ex([&](int tid){
		if (socket_img_ok)
		{
			//printf("11111111 in timer img_navi ok\n");
			socket_img_ok = false;
			if (img_p && visable)
			{
				ft_video *stream = (ft_video *)get_aliase_ui_control("show_big_navi");
				if (stream)
				{
					stream->update_text_from_image_buff(img_p, IMG_NAVI_SIZE);
					stream->set_visible(true);
					printf("-------stream is ok\n");
				}

			}
		}
	});
	g_timer.active_timer_ex(tmid_navi_img, 50);

	extern mp_var_set g_mp_var_set;
	g_mp_var_set[std::string("show_navi")] = [&](char* str_buff){
		static bool bshow=false;
		bshow = !bshow;
		PageBigNavi::getInstance()->visable = bshow;
	};
}

void PageBigNavi::show(void)
{

	visable = true;
}

void PageBigNavi::hide(void)
{
	visable = false;
}

void PageBigNavi::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		switch (key)
		{
		case key_right:
			this->hide();
			PageManager::getInstance()->pageSwitchTo(main_menu);
			break;
		default:
			break;
		}
	}
}

//ADAS
void PageAdas::init(void)
{

}

void PageAdas::show(void)
{
	visable = true;
}

void PageAdas::hide(void)
{
	visable = false;
}

void PageAdas::key_event(EmKey key, Em_key_state state)
{
	if (state == key_state_press)
	{
		switch (key)
		{
		case key_right:
			this->hide();
			PageManager::getInstance()->pageSwitchTo(main_menu);
			break;
		default:
			break;
		}
	}
}

REGISTER_CLASS(id_none, PageNoneDlg)
void PageNoneDlg::Init(void)
{

}

void PageNoneDlg::onToBeginStage(void)
{

}

void PageNoneDlg::onToLeaveStage(void)
{

}

bool PageNoneDlg::key_event(EmKey key, Em_key_state state)
{
	return true;
}
