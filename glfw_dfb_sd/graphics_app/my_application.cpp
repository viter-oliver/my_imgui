#include "my_application.h"
#include<chrono>
#include "main.h"
#include <sstream>
#include <thread>
#include "uart.h"
#include <mutex>
#include "msg_host_n.h"
#include "car_info.h"
#include "media_info.h"
#include "mobileye_adas.h"
#include "navi_info.h"
#include "upgrade.h"
#include "imgui_impl_glfw_gl3.h"
#include "af_bind.h"
#include "af_timer.h"
#include "enum_txt_name0.h"
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
using namespace chrono;
using namespace auto_future_utilities;
//#define debug_fps
//ro.build.fub.version=V20180123
msg_host_n g_msg_host;
af_timer g_timer;
steady_clock::time_point  lastTime;
int delta_limit = 30;
const float delta_pg = 0.25f;
float pg_value = 0.f;
int dis_id = 0;
int uart_fd=-1;
#ifdef _SCOMMM
#include "scomm.h"
scomm s_scm;
#endif
char str_show[MAX_CONTENT_LEN] = { 0 };

ft_material_3d* planes3d=nullptr;
/*float lane_camera_z = 134.0;
float lane_camera_u = 0.5f;*/
static float screen_y = 133.5;
#define		MCU_SYNC1			0xc2
#define		MCU_SYNC2			0x3d
#define MAX_BYTE_SIZE 0x400
#define MAX_CACH_SIZE MAX_BYTE_SIZE*4
enum 
{
	en_pos_signal,
	en_pos_index,
	en_pos_length_h,
	en_pos_length_l,
	en_pos_cmd_head,
};
void send_cmd_2_uart(u8* pbuff,int len)
{
	wait_fd_write_eable(uart_fd);
		u8* plink=new u8 [len+7];
		plink[0]=MCU_SYNC1;
		plink[1]=MCU_SYNC2;
		struct GNU_DEF signal_def
		{
			u8 reserve:5;
			u8 need_ac:1;
			u8 frame_tp:1;
			u8 frame_dir:1;
		};
		signal_def* psg=(signal_def*)(plink+2);
		psg->reserve=0;
		psg->need_ac=1;
		psg->frame_tp=1;
		psg->frame_dir=0;
		static u8 frame_idx=0;
		plink[2+en_pos_index]=frame_idx;
		frame_idx++;
		u16 frame_len=len+1;
		u8* pfm_ln=(u8*)&frame_len;
		plink[2+en_pos_length_h]=*pfm_ln++;
		plink[2+en_pos_length_l]=*pfm_ln;
		u8 chk_sm=plink[0]^plink[1]^plink[2]^plink[3]^plink[4]^plink[5];
		for(int ix=0;ix<len;ix++)
		{
			plink[2+en_pos_cmd_head+ix]=pbuff[ix];
			chk_sm=chk_sm^pbuff[ix];
		}
		plink[2+en_pos_cmd_head+len]=chk_sm;
		write(uart_fd,plink,len+7);
		delete[] plink;
}
bool be_show_req_update=false;
steady_clock::time_point  show_req_update_timer;

void action_update(int signum, siginfo_t * info, void * myact) 
{ //U盘插入，并检测到升级文件
        printf("Ready to upgrade %d\n", signum);
	u8 cmd_rq_update[4]={0,0x09,0x02,0};
	send_cmd_2_uart(cmd_rq_update,4);
	be_show_req_update=true;
	show_req_update_timer= steady_clock::now();
}

void interrupt_update(int signum, siginfo_t * info, void * myact) 
{ //U盘被拔出
        printf("Upgrade was interrupted %d\n", signum);
	u8 cmd_rq_update[4]={0,0x09,00,3};
	send_cmd_2_uart(cmd_rq_update,4);
	steady_clock::duration dtn(duration<int>(10));
	show_req_update_timer-= dtn;		
}

my_application::my_application(int argc, char **argv)
	:application(argc,argv)
{
	_screen_width = 800;
	_screen_height = 534;
	_win_width = 800;
	_win_height = 267;
	_wposx = 0.f;
	_wposy = screen_y;

	 struct sigaction update_act; 
        struct sigaction old_update_act; 
	 struct sigaction update_interrupt; 
        struct sigaction old_update_interrupt; 
	 sigemptyset(&update_act.sa_mask);
        update_act.sa_flags = SA_SIGINFO;
        update_act.sa_sigaction = action_update; //指定升级响应函数
	 sigemptyset(&update_interrupt.sa_mask);
        update_interrupt.sa_flags = SA_SIGINFO;
        update_interrupt.sa_sigaction = interrupt_update; //指定升级响应函数
        if(sigaction(SIGUSR1, &update_act, &old_update_act) < 0) { // 设定响应信号为 SIGUSR1
                printf("install sigal error\n");
        }
 
	if(sigaction(SIGUSR2, &update_interrupt, &old_update_interrupt) < 0) { // 设定响应信号为 SIGUSR2
                printf("install sigal error\n");
        }
}
bool be_show_border = false;
bool be_show_horizon=false;
bool be_show_brightness=false;
int brightness_value=5;
int height_value=0;
steady_clock::time_point  show_border_time;
steady_clock::time_point  show_horizon_time;
steady_clock::time_point  show_brightness_time;

void reg_cmd_handle()
{

	register_adas_cmd_handl();
	register_car_cmd_handl();
	register_media_cmd_handl();
	register_navi_cmd_handl();
	register_upgrade_cmd_handl();
}

static s8 *g_data_buff;
static u8 *g_data_cache, *g_data_cmd;
static int ifront = 0, irear = 0;
class mg_sdata
{
public:
	mg_sdata()
	{ 
		g_data_buff = new s8[MAX_BYTE_SIZE];
		g_data_cmd = new u8[MAX_BYTE_SIZE];
		g_data_cache = new u8[MAX_CACH_SIZE];
	}
	~mg_sdata(){ 
		delete[] g_data_buff; 
		delete[] g_data_cmd;
		delete[] g_data_cache; 
	}

};

mg_sdata mgsdata;
#if  1
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

#define I2C_ADDR               (0x61)
#define I2C_DEFDEV_NAME      "/dev/i2c-0"
static int i2c_write_cmd(int fd, unsigned char addr, unsigned char *cmdaddr, unsigned char cmdaddrlen) {
    struct i2c_rdwr_ioctl_data cmd;
    struct i2c_msg msg[1];
    msg[0].addr  = addr;
    msg[0].flags = 0;
    msg[0].len = cmdaddrlen;
    msg[0].buf = cmdaddr;
    cmd.msgs =  msg;
    cmd.nmsgs = sizeof(msg) / sizeof(struct i2c_msg);
    if (ioctl(fd, I2C_RDWR, (unsigned long)&cmd) < 0) {
        printf("Unable to send data,cmdaddr:%s !\n", strerror(errno));
        printf(" i2c_write_cmd addr:%04x, cmdaddr:%08x, len:%x \n", addr, *cmdaddr, cmdaddrlen );
        return -1;
    }
    return 0;
}
static int get_ioc_fd()
{
int fd = open(I2C_DEFDEV_NAME, O_RDWR);

if (ioctl(fd, I2C_SLAVE, I2C_ADDR) < 0) {
        printf("ioctl error: %s\n", strerror(errno));
        return 1;
    }else{
        printf("open success\n"); 
    }

//int ret = i2c_write_cmd(fd, I2C_ADDR, &Entry_flush_mode, 1);
//if( ret < 0) exit(0);
return fd;
}
unsigned char cmd_angle_up[] = {0x01, 0x01, 0x02};
unsigned char cmd_angle_down[] = {0x01, 0x02, 0x03};
unsigned char cmd_brightness_up[] = {0x02, 0x01, 0x03};
unsigned char cmd_brightness_down[] = {0x02, 0x02, 0x04};
static int fd_i2c=0;
#endif
extern void set_eula_x_ect_y(float eulax,float ecty);
int param_idx=30;
float eula_x_ect_y[61][2]=
{
{	1.379271459	,	-5.528	},
{	1.377732077	,	-5.428	},
{	1.376192695	,	-5.328	},
{	1.374653313	,	-5.228	},
{	1.373113931	,	-5.128	},
{	1.371574549	,	-5.028	},
{	1.370035167	,	-4.928	},
{	1.368495785	,	-4.828	},
{	1.366956403	,	-4.728	},
{	1.365417021	,	-4.628	},
{	1.363877639	,	-4.528	},
{	1.362338257	,	-4.428	},
{	1.360798875	,	-4.328	},
{	1.359259493	,	-4.228	},
{	1.357720111	,	-4.128	},
{	1.356180729	,	-4.028	},
{	1.354641347	,	-3.928	},
{	1.353101966	,	-3.828	},
{	1.351562584	,	-3.728	},
{	1.350023202	,	-3.628	},
{	1.34848382	,	-3.528	},
{	1.346944438	,	-3.428	},
{	1.345405056	,	-3.328	},
{	1.343865674	,	-3.228	},
{	1.342326292	,	-3.128	},
{	1.34078691	,	-3.028	},
{	1.339247528	,	-2.928	},
{	1.337708146	,	-2.828	},
{	1.336168764	,	-2.728	},
{	1.334629382	,	-2.628	},
{	1.33309	,	-2.528	},//0-30
{	1.331550618	,	-2.428	},
{	1.330011236	,	-2.328	},
{	1.328471854	,	-2.228	},
{	1.326932472	,	-2.128	},
{	1.32539309	,	-2.028	},
{	1.323853708	,	-1.928	},
{	1.322314326	,	-1.828	},
{	1.320774944	,	-1.728	},
{	1.319235562	,	-1.628	},
{	1.31769618	,	-1.528	},
{	1.316156798	,	-1.428	},
{	1.314617416	,	-1.328	},
{	1.313078034	,	-1.228	},
{	1.311538653	,	-1.128	},
{	1.309999271	,	-1.028	},
{	1.308459889	,	-0.928	},
{	1.306920507	,	-0.828	},
{	1.305381125	,	-0.728	},
{	1.303841743	,	-0.628	},
{	1.302302361	,	-0.528	},
{	1.300762979	,	-0.428	},
{	1.299223597	,	-0.328	},
{	1.297684215	,	-0.228	},
{	1.296144833	,	-0.128	},
{	1.294605451	,	-0.028	},
{	1.293066069	,	0.072	},
{	1.291526687	,	0.172	},
{	1.289987305	,	0.272	},
{	1.288447923	,	0.372	},
{	1.286908541	,	0.472	},
};
void my_application::resLoaded()
{
	lastTime = steady_clock::now();

	if (_arg_list.size() > 0)
	{
		g_msg_host.load_protocol_from_config(_arg_list[0].c_str());
		planes3d = (ft_material_3d*)get_aliase_ui_control("cur_lane");
		assert(planes3d != nullptr);
		reg_cmd_handle();
		fd_i2c=get_ioc_fd();		
		g_msg_host.attach_monitor("steering keys", [&](u8*pbuff, int len){
			enum
			{
				en_powr_on,
				en_horizontal_cal_up,
				en_horizontal_cal_dn,
				en_height_up,
				en_height_dn,
				en_brightness_up,
				en_brightness_dn,
				en_buzzer_on,
				en_buzzer_off,
			};
			enum
			{
				en_key_release,
				en_key_short_press,
				en_key_long_press,
				en_key_hold,
			};
			u8 key_value = *pbuff++;
			u8 key_status = *pbuff;
			static u8 prev_key_status=0;
			int ret;
			printf("key_value=%d,key_status=%d\n",key_value,key_status);
			if (key_status == en_key_release)
			{
				static float screeen_angle = 0.0;
				const float angl_unit = 0.001f;
				switch (key_value)
				{
				case en_powr_on:
					break;
				case en_horizontal_cal_up:
				{
					show_horizon_time= steady_clock::now();
					be_show_horizon = true;
					set_property_aliase_value("show_horizon", &be_show_horizon);
					be_show_border=false;
					set_property_aliase_value("show_bg_border", &be_show_border);
					be_show_brightness=false;
					set_property_aliase_value("show_brightness", &be_show_brightness);
					if(param_idx<60)
					{
						param_idx++;
						set_eula_x_ect_y(eula_x_ect_y[param_idx][1],eula_x_ect_y[param_idx][0]);
						
						sprintf(str_show,"%d",param_idx);
						set_property_aliase_value("horizon_value", str_show);
						float height=165.f-param_idx*2.7f;
						set_property_aliase_value("horizon_pg", &height);

					}
					//screeen_angle += angl_unit;
					//set_rotate_angle(screeen_angle);
					/*if (be_adj_fov)
					{
						ui_xpos+=1.f;
						uilocation->set_base_pos(ui_xpos-16,ui_ypos-16);
						printf("loc posx=%f, posy=%f\n",ui_xpos,ui_ypos);
					}*/
				}
				break;
				case en_horizontal_cal_dn:
				{
					
					show_horizon_time= steady_clock::now();
					be_show_horizon = true;
					set_property_aliase_value("show_horizon", &be_show_horizon);
					be_show_border=false;
					set_property_aliase_value("show_bg_border", &be_show_border);
					be_show_brightness=false;
					set_property_aliase_value("show_brightness", &be_show_brightness);
					if(param_idx>0)
					{
						param_idx--;
						set_eula_x_ect_y(eula_x_ect_y[param_idx][1],eula_x_ect_y[param_idx][0]);
						sprintf(str_show,"%d",param_idx);
						set_property_aliase_value("horizon_value", str_show);
						float height=165.f-param_idx*2.7f;
						set_property_aliase_value("horizon_pg", &height);

					}
					//screeen_angle -= angl_unit;
					//set_rotate_angle(screeen_angle);
					/*if (be_adj_fov)
					{
						ui_xpos-=1.f;
						uilocation->set_base_pos(ui_xpos-16,ui_ypos-16);
						printf("loc posx=%f, posy=%f\n",ui_xpos,ui_ypos);
						
					}*/
				}
				break;
				case en_height_up:
				{
					show_border_time = steady_clock::now();
					be_show_border = true;
					set_property_aliase_value("show_bg_border", &be_show_border);
					be_show_horizon = false;
					set_property_aliase_value("show_horizon", &be_show_horizon);
					be_show_brightness=false;
					set_property_aliase_value("show_brightness", &be_show_brightness);
					if(height_value<30)
					{
						height_value++;
						sprintf(str_show,"%d",height_value);
						set_property_aliase_value("height_value", str_show);
						float height=165.f-(height_value+30)*2.7f;
						set_property_aliase_value("height_pg", &height);
					}
					ret = i2c_write_cmd(fd_i2c, I2C_ADDR, cmd_angle_up, 3);
			              if( ret < 0){
			                  printf("Error cmd_angle_up: %s\n", strerror(errno));
			              }
					
				}
				break;
				case en_height_dn:
				{				
					show_border_time = steady_clock::now();
					be_show_border = true;
					set_property_aliase_value("show_bg_border", &be_show_border);
					be_show_horizon = false;
					set_property_aliase_value("show_horizon", &be_show_horizon);
					be_show_brightness=false;
					set_property_aliase_value("show_brightness", &be_show_brightness);
					if(height_value>-30)
					{
						height_value--;
						sprintf(str_show,"%d",height_value);
						set_property_aliase_value("height_value", str_show);
						float height=165.f-(height_value+30)*2.7f;
						set_property_aliase_value("height_pg", &height);
					}
					ret = i2c_write_cmd(fd_i2c, I2C_ADDR, cmd_angle_down, 3);
				       if( ret < 0){
				            printf("Error cmd_angle_down: %s\n", strerror(errno));
				       }

				}
				break;
				case en_brightness_up:

					show_brightness_time=steady_clock::now();
					be_show_brightness=true;
					set_property_aliase_value("show_brightness", &be_show_brightness);
					be_show_border = false;
					set_property_aliase_value("show_bg_border", &be_show_border);
					be_show_horizon = false;
					set_property_aliase_value("show_horizon", &be_show_horizon);
					if(brightness_value<9)
					{
						brightness_value++;
						sprintf(str_show,"%d",brightness_value);
						set_property_aliase_value("brightness_value", str_show);
						float height=165.f-brightness_value*18.3f;
						set_property_aliase_value("brightness_pg", &height);
					}
					ret = i2c_write_cmd(fd_i2c, I2C_ADDR, cmd_brightness_up, 3);
			              if( ret < 0){
			                  printf("Error cmd_brightness_up: %s\n", strerror(errno));
			              }
					break;
				case en_brightness_dn:

					show_brightness_time=steady_clock::now();
					be_show_brightness=true;
					set_property_aliase_value("show_brightness", &be_show_brightness);
					be_show_border = false;
					set_property_aliase_value("show_bg_border", &be_show_border);
					be_show_horizon = false;
					set_property_aliase_value("show_horizon", &be_show_horizon);
					
					if(brightness_value>0)
					{
						brightness_value--;
						sprintf(str_show,"%d",brightness_value);
						set_property_aliase_value("brightness_value", str_show);
						float height=165.f-brightness_value*18.3f;
						set_property_aliase_value("brightness_pg", &height);
					}
					ret = i2c_write_cmd(fd_i2c, I2C_ADDR, cmd_brightness_down, 3);
			              if( ret < 0){
			                  printf("Error cmd_brightness_up: %s\n", strerror(errno));
			              }
					break;
				case en_buzzer_on:
					break;
				case en_buzzer_off:
					break;

				}
			}
			prev_key_status=key_status;
			/*
			else
			if (key_status == en_key_long_press)
			{
				if (key_value == en_horizontal_cal_dn)
				{
					be_adj_fov = !be_adj_fov;
					set_property_aliase_value("show_location", &be_adj_fov);
				}
			}*/
		});
	}
	auto currentTime = steady_clock::now();
	int delta = chrono::duration_cast<chrono::duration<int, std::milli>>(currentTime - lastTime).count();
	printf("msg_host consume%d milli seconds\n", delta);
	lastTime = currentTime;
	if(_arg_list.size()>1)
	{
		//thread th_uart_comm1([&](string dev_path){},_arg_list[1]);
	
		thread th_uart_comm([&](string  dev_path){
	#if 1
			uart_fd=openport(dev_path.c_str());
			if(uart_fd>0)
			{
				setport(uart_fd,115200,8,1,0);
				_be_uart_comm=true;
				while(_be_uart_comm)
				{
					wait_fd_read_eable(uart_fd);
					int len = read(uart_fd, g_data_buff, MAX_BYTE_SIZE);
					//printf("length of data is %d\n",len);
					//print_buff(g_data_buff,len);
					if (ifront + len<=MAX_CACH_SIZE)
					{
						memcpy(g_data_cache + ifront, g_data_buff, len);
					}
					else
					{
						int itrim = MAX_CACH_SIZE - ifront;
						memcpy(g_data_cache + ifront, g_data_buff, itrim);
						memcpy(g_data_cache, g_data_buff + itrim, len - itrim);
					}
					ifront += len;
					ifront %= MAX_CACH_SIZE;
					for (; irear!=ifront;++irear,irear%=MAX_CACH_SIZE)
					{
						static u8 prebyte=0,check_sum=0;
						static bool be_picking_data = false;
						static int pick_index = 0;
						static u16 frame_len;

						if (!be_picking_data)
						{
							if (prebyte == MCU_SYNC1&&g_data_cache[irear] == MCU_SYNC2)
							{
								be_picking_data = true;
								pick_index = 0;
								prebyte = 0;
								check_sum=MCU_SYNC1^MCU_SYNC2;
							}
							prebyte = g_data_cache[irear];
						}
						else
						{
							g_data_cmd[pick_index] = g_data_cache[irear];
							bool finish_frame=false;
							if (pick_index==en_pos_length_l)
							{
							      u16* plen=(u16*)& g_data_cmd[en_pos_length_h];
								frame_len = *plen;// g_data_cmd[en_pos_length_h]*0x100+g_data_cmd[en_pos_length_l];
								if (frame_len>MAX_BYTE_SIZE - en_pos_length_l)
								{
									printf("invalid command length:%d\n", frame_len);
									be_picking_data = false;
									finish_frame = true;
								}
							}
							else
							if (pick_index==(frame_len+en_pos_length_l))
							{
								u8 fchk_sum = g_data_cmd[pick_index];
								if (fchk_sum==check_sum)
								{
									//RmtAdpter::GetInstance()->interpretCommand(g_data_cmd + 3);
									//printf("get a frame data\n");
									g_msg_host.pick_valid_data(&g_data_cmd[en_pos_cmd_head],frame_len-1);
								}
								else
								{
									printf("fail to checksum1 check_sum=0x%x fchk_sum=0x%x\n",check_sum,fchk_sum);
								}
								be_picking_data = false;
								finish_frame=true;
							}
							if(!finish_frame)
							{
								check_sum^=g_data_cmd[pick_index] ;
								pick_index++;
							}
						}
					}
				}
		}
		else{
			printf("fail to open %s",dev_path.c_str());
			return;
		}
#endif
		},_arg_list[1]);
          th_uart_comm.detach();
	   g_msg_host.set_send_cmd(send_cmd_2_uart);
	}

	
#ifdef RCF_COMM
	g_rcf_server.set_cmd_handle(std::bind(&msg_host_n::pick_valid_data, &g_msg_host, std::placeholders::_1, std::placeholders::_2));
	g_rcf_server.link();
	currentTime = std::chrono::high_resolution_clock::now();
	delta = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(currentTime - lastTime).count();
	printf("rcf server consume %d milli seconds\n",delta);
#endif

#ifdef _SCOMMM
	s_scm.set_msg_handle(std::bind(&msg_host_n::pick_valid_data, &g_msg_host, std::placeholders::_1, std::placeholders::_2));
	DWORD m_dwThreadId;
	CreateThread(NULL, 0, ThreadLoadApps, &s_scm, 0, &m_dwThreadId);
	currentTime = std::chrono::high_resolution_clock::now();
	delta = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(currentTime - lastTime).count();
	printf("comm consume %d milli seconds\n", delta);
	/*thread scmm_thd([this]{s_scm(); });
	scmm_thd.detach();*/
#endif
}

void my_application::onUpdate()
{
	g_msg_host.execute_data_handle_cmd();
	g_timer.execute();
	adas_cmd_update();
	lane_moving_animation();
	if(be_show_border)
	{
		auto currentTime = steady_clock::now();
		int delta = chrono::duration_cast<chrono::duration<int,std::milli>>(currentTime - show_border_time).count();
		if(delta>5000)
		{
			be_show_border=false;
			set_property_aliase_value("show_bg_border",&be_show_border);
		}
	}
	if(be_show_horizon)
	{
		auto currentTime = steady_clock::now();
		int delta = chrono::duration_cast<chrono::duration<int,std::milli>>(currentTime - show_horizon_time).count();
		if(delta>5000)
		{
			be_show_horizon=false;
			set_property_aliase_value("show_horizon",&be_show_horizon);
		}
	}
	if(be_show_brightness)
	{
		
		auto currentTime = steady_clock::now();
		int delta = chrono::duration_cast<chrono::duration<int,std::milli>>(currentTime - show_brightness_time).count();
		if(delta>5000)
		{
			be_show_brightness=false;
			set_property_aliase_value("show_brightness",&be_show_brightness);
		}
	}
	if(be_show_req_update)
	{
		auto currentTime = steady_clock::now();
		int delta = chrono::duration_cast<chrono::duration<int,std::milli>>(currentTime - show_req_update_timer).count();
		set_property_aliase_value("show_rq_update",&be_show_req_update);
		int delta_sec=15-delta/1000;
		sprintf(str_show,"%dS",delta_sec);
		set_property_aliase_value("update_counter",str_show);
		
		if(delta>15000)
		{
			be_show_req_update=false;
			set_property_aliase_value("show_rq_update",&be_show_req_update);
		}
	}
#ifdef debug_fps
	static int frame_count=0;
	frame_count++;
	auto currentTime = steady_clock::now();
	int delta = chrono::duration_cast<chrono::duration<int, std::milli>>(currentTime - lastTime).count();
	if(delta>=1000)
	{
		printf("rendering rate=%dfps\n",frame_count);
		lastTime = currentTime;
		frame_count=0;
	}
#endif
#ifdef SHOW_ANIMATION
	auto currentTime = steady_clock::now();
	int delta = chrono::duration_cast<chrono::duration<int, std::milli>>(currentTime - lastTime).count();
	if (delta < delta_limit)
	{
		return;
	}
	pg_value += delta_pg;
	if (pg_value > 1.f)
	{
		pg_value = 0.f;
	}
	dis_id++;
	dis_id %= 4;
	float rt_value = 1 - pg_value;
	set_property_aliase_value("fuel_consume", &pg_value);
	set_property_aliase_value("roate_speed", &rt_value);
	bool be_v = false;
	set_property_aliase_value("show_distance0", &be_v);
	set_property_aliase_value("show_distance1", &be_v);
	set_property_aliase_value("show_distance2", &be_v);
	set_property_aliase_value("show_distance3", &be_v);
	be_v = true;
	stringstream stm_tp;
	stm_tp << "show_distance" << dis_id;
	string pt_dis = stm_tp.str();
	set_property_aliase_value(pt_dis, &be_v);
#endif


}

my_application::~my_application()
{
}
#ifdef _SCOMMM

bool my_application::create_run()
{
	if (_arg_list.size()<1)
	{
		printf("at least you should provide a serial comm number!\n");
		return false;
	}
	int comm_id = atoi(_arg_list[1].c_str());
	int baudrate = 115200;
	if (_arg_list.size()>2)
	   baudrate=atoi(_arg_list[2].c_str());
	if (!s_scm.open(comm_id, baudrate))
	{
		return false;
	}
	
	return application::create_run();
}
#endif
AFGUI_APP(my_application)


