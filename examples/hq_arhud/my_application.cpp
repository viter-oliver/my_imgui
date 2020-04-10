#include "my_application.h"
#include <GLFW/glfw3.h>
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
#include "enum_txt_name0.h"
#include "SOIL.h"
#include "near_scenario.h"
#include "far_scenario.h"

using namespace chrono;
using namespace auto_future_utilities;
//#define debug_fps
//ro.build.fub.version=V20180123
msg_host_n g_msg_host;
extern void print_buff(u8* pbuff, int len);
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

my_application::my_application(int argc, char **argv)
	:application(argc,argv)
{
	_screen_width = 800;
	_screen_height = 450;
	_win_width = 800;
	_win_height = 300;
	_wposx = 0.f;
	_wposy = 150.f; 
}

//steady_clock::time_point  show_border_time;

void mem_recv( GLFWwindow*, unsigned long data_id, unsigned long data_long, void* pbuff )
{
     static u8 pdata[ 0x100 ];
     memcpy( pdata, &data_id, 2 );
     g_msg_host.pick_valid_data( pdata, data_long + 2 );
}
bool capture_buffer = false;
void KeyCallback( GLFWwindow*, int key, int, int action, int mods )
{

     if( action == GLFW_RELEASE)
     {
          KeyTest( key );
          if (key==GLFW_KEY_C)
          {
               capture_buffer = !capture_buffer;
          }
     }

}
void my_application::resLoaded()
{
     gl3wInit();
	lastTime = steady_clock::now();
      // ImGui_ImplGlfwGL3_Init_Shader_Source(vtsource,ftsource);
     glfwSetKeyCallback( _window, KeyCallback );
	if (_arg_list.size() > 0)
	{
		g_msg_host.load_protocol_from_config(_arg_list[0].c_str());
          glfwSetMemCallback( _window, mem_recv );
		g_msg_host.set_f_get_cmd([&](u8* pbuff,string& kid)->u8{
			static char num_tb[ 0x10 ] =
			{
			  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
			};
			struct cmd_data
			{
			  u8 data0 : 4;
			  u8 data1 : 4;
			};
			kid.resize(6);
			cmd_data* pcmd=(cmd_data*)pbuff;
			kid[0]=num_tb[pcmd->data1];
			kid[1]=num_tb[pcmd->data0];
			pcmd++;
			kid[2]=num_tb[pcmd->data1];
			kid[3]=num_tb[pcmd->data0];
			pcmd++;
			kid[4]=num_tb[pcmd->data1];
			kid[5]=num_tb[pcmd->data0];
			
			return 3;
		});
		register_near_cmd_handl();
		register_adas_cmd_handl();
		//RegisterMCUMsgCallback(mcu_msg_handle);

		//fd_i2c=get_ioc_fd();		

	}
	auto currentTime = steady_clock::now();
	int delta = chrono::duration_cast<chrono::duration<int, std::milli>>(currentTime - lastTime).count();
	printf("msg_host consume%d milli seconds\n", delta);
	lastTime = currentTime;

}

void my_application::onUpdate()
{
     /*if (ImGui::GetIO().KeysDown[GLFW_KEY_LEFT])
     {
          printf( "left down\n" );
     }
     else if (ImGui::GetIO().KeysDown[GLFW_KEY_RIGHT])
     {
          printf( "right down\n" );
     }*/
	g_msg_host.execute_data_handle_cmd();
	g_timer.execute();
	adas_cmd_update();
	
}
struct  opgenl_container
{
     int _wd = 0, _ht = 0;

public:
     opgenl_container( int wd, int ht)
          :_wd(wd), _ht(ht){}
     uint8_t* get_image(int& tdsz)
     {
          tdsz = _wd*_ht * 4;
          uint8_t* txtdata = new uint8_t[ tdsz ];
          /*glBindTexture( GL_TEXTURE_2D, 0 );
          glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, txtdata );
          */
          glReadPixels( 0, 0, _wd, _ht, GL_RGBA, GL_UNSIGNED_BYTE, txtdata );
          uint8_t* pdata = txtdata + _wd * 4 * 10;
          memset( pdata, 0xff, _wd * 4 * 20 );

          return txtdata;
     }
};
void my_application::before_swap()
{
     if (capture_buffer)
     {
          //capture_buffer = false;
          opgenl_container glc( _screen_width, _screen_height );
          int dsz = 0;
          auto tdat = glc.get_image( dsz );
          save_image( tdat, dsz );

          delete[] tdat;
     }
}

my_application::~my_application()
{
}
AFGUI_APP(my_application)


