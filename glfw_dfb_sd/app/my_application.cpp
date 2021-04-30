#include "my_application.h"
#include <glfw/glfw3.h>
#include<chrono>
#include "main.h"
#include <sstream>
#include <thread>
#include <mutex>
#include "msg_host_n.h"
#include "af_state_manager.h"
#include "imgui_impl_glfw_gl3.h"
#include "af_bind.h"
#include "af_timer.h"
#include "pop_win.h"
#include "enum_txt_res_name.h"
#include "debug_var_set.h"
#ifdef WIN32
#include <gl/gl3w.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#endif // WIN32
#include <stdio.h>
#include "main_view_data.h"
#include "IndicateBase.h"


//#include "./PageDlg/PageManager.h"

#include <chrono>
#include <sstream>
using namespace chrono;
using namespace auto_future_utilities;
//#define debug_fps
//ro.build.fub.version=V20180123
msg_host_n g_msg_host;
extern void print_buff(u8* pbuff, int len);
af_timer g_timer;
//steady_clock::time_point  lastTime;
int delta_limit = 30;
const float delta_pg = 0.25f;
float pg_value = 0.f;
int dis_id = 0;
int uart_fd = -1;
#ifdef _SCOMMM
#include "scomm.h"
scomm s_scm;
#endif
char str_show[MAX_CONTENT_LEN] = { 0 };
bool be_visible = false;


#define  Debug_aro 

bool is_debug = false;

u8 de_char[3] = { 0 };

extern MyPopWin my_popWin;

void mcu_msg_handle(u8* pbuff, int ilen)
{
	//printf("come to mcu_msg_handle\n");
	//print_buff(pbuff, ilen);

	u8 frame_len = *pbuff;
	pbuff += 1;

	g_msg_host.pick_valid_data(pbuff, ilen - 1);
	if (is_debug)
	{
		//rev_middware(pbuff, de_char[0], de_char[1], de_char[2], ilen - 3);
		print_buff(pbuff, ilen);
		//print_buff(pbuff, ilen);
	}
}

void my_application::UpdateUartComm(std::string str)
{
	 //printf("UpdateUartComm get data \n");
	//for (int i = 0; i<str.length(); i++)
	//	printf("%02x ", str[i]);
	//printf("\n");

	mcu_msg_handle((u8 *)str.c_str(), str.length());
}


//media
#ifdef IMGUI_WAYLAND
// IVI  音乐信息
void my_application::UpdateMusicInfor(StuMusicInformation data)
{
	//string currentMusicName; //歌曲名
	//string currentSinger;    //演唱者
	//uint32_t currentTime;    //当前播放时间
	//uint32_t totalTime;      //总时长
	//bool isSwitchMusic;      //切歌状态
	//bool bPlay;              //true 播放, false 暂停
	//printf("get music data from IVI\n");

	static std::string str("");
	//printf("name=%s,singer=%s\n", data.currentMusicName.c_str(),data.currentSinger.c_str());
	StuMusicInformation *p = &data;
	//关闭顶部的FM/AM
	bool b = false;
	set_property_aliase_value("show_media_fm_am", &b);
	b = true;
	set_property_aliase_value("show_media_music", &b);

	float v = p->currentTime / (p->totalTime *1.0);
	set_property_aliase_value("control_small_music_progress", &v);

	ft_slider * t = (ft_slider*)get_aliase_ui_control("show_big_music_slider");
	t->set_progress(v);
	b=p->bPlay;
	b=!b;
	set_property_aliase_value("show_big_music_play", &b);
	set_property_aliase_value("show_small_music_play", &b);

	if(p->isSwitchMusic)
	{
		my_popWin.Pop_Window(em_bluetooth_talk,p->isSwitchMusic);
	}
	else
	{
		my_popWin.Pop_Window(em_bluetooth_talk,p->isSwitchMusic);
	}

	if (str != p->currentMusicName)
		str = p->currentMusicName;
	else
		return;
	DATA_STORE.set_song_name(p->currentMusicName);
	DATA_STORE.set_media_music_fm(media_2nd_usb_music);
	DATA_STORE.set_song_play(p->bPlay);

	printf("currT=%d,TTime=%d,switch=%d,bplay=%d\n", p->currentTime, p->totalTime,p->isSwitchMusic,p->bPlay);
	set_property_aliase_value("control_singer_name_big", (void *)(p->currentSinger.c_str()));
	set_property_aliase_value("control_song_name_big", (void *)(p->currentMusicName.c_str()));
	set_property_aliase_value("control_song_name_small", (void *)(p->currentMusicName.c_str()));
	set_property_aliase_value("control_media_menu_song_play",&(p->bPlay));
	set_property_aliase_value("control_media_song_play",&(p->bPlay));
	printf("IVI music end\n");
	b = true;
	set_property_aliase_value("show_music_big", &b);
	b = false;
	set_property_aliase_lazy_value("show_music_big", 2000, &b);

}
// IVI 电台
void my_application::UpdateRadioInfor(StuRadioInformation data)
{
	//EmRadioChannel channel; //频道
	//uint32_t frequency;     //频率
	printf("get radio data from IVI\n");

	static int chanel = 0;
	static int freq = 0;

	StuRadioInformation *p = &data;

	if (p->channel == chanel && p->frequency == freq)
		return;
	DATA_STORE.set_fm_freq(p->frequency);
	DATA_STORE.set_media_music_fm(media_2nd_radio_fm);
	//关闭顶部的音乐
	bool b = false;
	set_property_aliase_value("show_media_music", &b);
	b = true;
	set_property_aliase_value("show_media_fm_am", &b);

	if (p->channel == em_AM)
	{
		bool t = true;
		sprintf(str_show,"%d.%d", p->frequency / 10,p->frequency%10 );
		set_property_aliase_value("control_am_value", &(str_show));
		set_property_aliase_value("show_top_am", &t);
		}
	else if (p->channel == em_FM)
	{
		bool t = false;
		sprintf(str_show, "%d.%d", p->frequency / 10, p->frequency % 10);
		set_property_aliase_value("control_fm_value", &(str_show));
		set_property_aliase_value("show_top_am", &t);
	}
	chanel = p->channel;
	freq = p->frequency;

}
// IVI 电话
void my_application::UpdatePhoneInfor(StuPhoneInformation data)
{
	//string number;       //联系人电话号码
	//string name;         //联系人名字
	//EmPhoneState status; //电话状态
	//ANSWER = 0,
	//	HANG_UP = 1,
	printf("get calling data from IVI\n");
	g_msg_host.pick_valid_data((u8 *)&data, sizeof(data));
}//
#endif


void rev_middware(u8 *pbuff, u8 a, u8 b, u8 c, int ilen)
{
	if (*pbuff == a && *(pbuff + 1) == b &&*(pbuff + 2) == c)
	{
		for (int i = 0; i < ilen; i++)
		{
			printf("x%x ", pbuff[i]);
		}
		printf("\n");
	}
}

//bool be_debug_online=false;


my_application::my_application(int argc, char **argv)
	:application(argc, argv)  
#ifdef IMGUI_WAYLAND
	, ojIPC(HMI_AFG_MODULE)
#endif
{
	_screen_width = 1920;
	_screen_height = 720;
	_win_width = 1920;
	_win_height = 720;
	_wposx = 0.f;
	_wposy = 0.f;

}

//steady_clock::time_point  show_border_time;
void mem_recv(GLFWwindow*, unsigned long data_id, unsigned long data_long, void* pbuff)
{
	static u8 pdata[0x100];
	memcpy(pdata, &data_id, 2);
	g_msg_host.pick_valid_data(pdata, data_long + 2);
}
bool capture_buffer = false;
//work space $(ProjectDir)

#include <stdio.h>
bool debug_show_fps = true;


int cccc = 0;
void my_sum_fps(void)
{
	static steady_clock::time_point time_fps_pre = steady_clock::now();
	static steady_clock::time_point time_fps_now;
	time_fps_now = steady_clock::now();
	int delta = chrono::duration_cast<chrono::duration<int, std::milli>>(time_fps_now - time_fps_pre).count();

	if (delta > 1000 && debug_show_fps){
		time_fps_pre = time_fps_now;
		ft_textblock *p = (ft_textblock *)get_aliase_ui_control("show_fps");

		char buf[50] = { 0 };
		sprintf(buf, "fps=%d", cccc);
		p->set_content(buf);
		cccc = 0;
		//printf("fps=%d\n", (int)ImGui::GetIO().Framerate);
	}
}

std::string search_att(std::string &str)
{

	auto pos_pre = str.find_last_of(' ');
	auto pos_end = str.find_last_of(';');
	//std::string tt("tmpafg");
	if (pos_end > pos_pre)
	{
		std::string a = str.substr(pos_pre + 1, pos_end - (pos_pre)-1);
		a = a + "";
		//printf("pos =%d, size=%d\n", tt.find_last_of('f'), pos_end - (pos_pre)-1);
		return a;
	}
	else
	{
		std::string tmp = str.substr(0, pos_end);
		pos_pre = str.find_last_of(' ');
		pos_end = str.find_last_of(';');
		std::string a = str.substr(pos_pre + 1, pos_end - 1);
		return a;
	}
}

void my_application::resLoaded()
{
#ifdef WIN32
	//gl3wInit();
	glfwSetKeyCallback(_window, KeyCallback);
#endif // WIN32

	if (_arg_list.size() > 0)
	{
		//printf("-----------arg str=%s\n", _arg_list[0].c_str());
		g_msg_host.load_protocol_from_config(_arg_list[0].c_str());
		//#ifdef WIN32
		//		glfwSetMemCallback(_window, mem_recv);
		//#endif // WIN32
		g_msg_host.set_f_get_cmd([&](u8* pbuff, string& kid)->u8{
			static char num_tb[0x10] =
			{
				'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
			};
			struct cmd_data
			{
				u8 data0 : 4;
				u8 data1 : 4;
			};
			kid.resize(6);
			cmd_data* pcmd = (cmd_data*)pbuff;
			kid[0] = num_tb[pcmd->data1];
			kid[1] = num_tb[pcmd->data0];
			pcmd++;
			kid[2] = num_tb[pcmd->data1];
			kid[3] = num_tb[pcmd->data0];
			pcmd++;
			kid[4] = num_tb[pcmd->data1];
			kid[5] = num_tb[pcmd->data0];
			pbuff[0] = 0;
			pbuff[1] = 0;
			pbuff[2] = 0;
			// printf("-----------g_msg_host.set_f_get_cmd\n");
			return 3;
		});
#ifdef IMGUI_WAYLAND
		//RegisterMCUMsgCallback(mcu_msg_handle);

		//register rcf

#endif
	}
	/*if(_arg_list.size()>1)
	{
	thread thd_pcan([&](const char* pt_path){
	private_can_task(pt_path);
	},_arg_list[1].c_str());
	thd_pcan.detach();
	}*/

#ifdef Debug_aro
	extern mp_var_set g_mp_var_set;
	g_mp_var_set[std::string("d_mid")] = [&](char* str_buff){
		is_debug = !is_debug;

		//std::string str("06 12 14");
		std::string str(str_buff);

		int pos = str.find_first_of(' ');
		cout << "pos=" << pos << endl;
		std::string s = str.substr(0, pos);

		printf("s=%s\n", s.c_str());

		de_char[0] = atoi(s.c_str());

		s.clear();

		int p2 = str.find_first_of(' ', pos + 1);
		printf("p2=%d\n", p2);

		s = str.substr(pos + 1, p2 - pos - 1);
		de_char[1] = atoi(s.c_str());


		p2 = str.find_last_of(' ');
		s.clear();

		s = str.substr(p2 + 1);

		printf("s=%s\n", s.c_str());
		de_char[2] = atoi(s.c_str());

	};
#endif

#ifdef IMGUI_WAYLAND
	printf("init before\n");
	ojIPC.Subscribe< STRIUartData >(MCU_MODULE, *this);

	ojIPC.Subscribe< IMusicInfor >(MCU_MODULE, *this);
	ojIPC.Subscribe< IRadioInfor >(MCU_MODULE, *this);
	ojIPC.Subscribe< IPhoneInfor >(MCU_MODULE, *this);

	printf("init after\n");
#endif

	init_var_set_fifo();
	reg_main_view_data();
	register_main_view_page();
	register_tellte();
	register_pop_win_cmd();
	SingleDataStore::getInstance().init();
	//导航初始化
	PageBigNavi::getInstance()->init();
	//printf("-------debug_test\n");
	//HMIDisplayReady();//system("echo finish resloded>/tmp/fifo_hq");
}

#ifndef WIN32
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#endif

void my_application::onUpdate()
{
	g_msg_host.execute_data_handle_cmd();
	g_timer.execute();
	cmd_update();
	cccc++;
#ifdef WIN32
	my_sum_fps();
	//printf("cc=%d\n", cccc);
#endif

#ifdef IMGUI_WAYLAND
	ft_textblock *p = (ft_textblock *)get_aliase_ui_control("show_fps");
	p->set_visible(true);
	if (cccc == 50)
	{
		//printf("fps=%d\n", (int)ImGui::GetIO().Framerate);
		cccc = 0;
		char a[20] = { 0 };

		sprintf(a, "fps=%d", (int)ImGui::GetIO().Framerate);
		p->set_content(a);
	}
#endif // !WIN32


}

my_application::~my_application()
{
}
AFGUI_APP(my_application)

