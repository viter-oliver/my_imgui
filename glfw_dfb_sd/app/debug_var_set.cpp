#include "debug_var_set.h"
#include <thread>

#include<string.h>
//#include<fcntl.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include <atomic>
mp_var_set g_mp_var_set;
const char* fifo_path="/tmp/fifo_hq";
atomic<unsigned int > _rear_id{ 0 }, _front_id{ 0 };
const int FF_BUFF_LEN =0x100;
const int queque_len=50;
char buff_queue[queque_len][FF_BUFF_LEN];
//char MyBuff_queue[FF_BUFF_LEN];

bool recieve_cmd=false;
int fd_fifo=0;
#ifdef WIN32
#include <windows.h>
#else
#include<unistd.h>
#include "uart.h"
#endif


void init_var_set_fifo()
{
#ifndef WIN32

	int ret=-1;
	if(access(fifo_path,F_OK))
	{
		ret=mkfifo(fifo_path,0666);
		if(ret==-1)
		{
			printf("fail to create fifo:%s\n",fifo_path);
		}
	}
	else
	{
		ret=1;
	}
	if(ret!=-1)
	{
		thread th_fifo([&](){

			 fd_fifo=open(fifo_path,O_RDWR|O_CLOEXEC);
			if(fd_fifo==-1)
			{
				printf("fail to open fifo:%s\n",fifo_path);
				return;
				
			}
			recieve_cmd=true;
			while(recieve_cmd)
			{
				wait_fd_read_eable(fd_fifo);
				int n=read(fd_fifo,buff_queue[_front_id],FF_BUFF_LEN);
				if(n>0)
				{
					buff_queue[_front_id][n-1]='\0';
					//printf("recieve cmd:%s\n",buff_queue[_front_id]);
					unsigned int front_next_id = _front_id + 1;
					front_next_id %= queque_len;
					_front_id = front_next_id;
					
				}
				
			}
			close(fd_fifo);
		});
		th_fifo.detach();
	}
#else

	thread th_fifo([&](){

		HANDLE hPipe = CreateNamedPipe(
			TEXT("\\\\.\\Pipe\\mypipe"),                            //管道名
			PIPE_ACCESS_INBOUND,                                     //管道类型,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,  //管道参数
			PIPE_UNLIMITED_INSTANCES,                               //管道能创建的最大实例数量
			0,                                                      //输出缓冲区长度 0表示默认
			0,                                                      //输入缓冲区长度 0表示默认
			NMPWAIT_NOWAIT,                                   //超时时间
			NULL);                                                  //指定一个SECURITY_ATTRIBUTES结构,或者传递零值.

		if (INVALID_HANDLE_VALUE == hPipe) {
			printf("Create Pipe Error(%d)\n", GetLastError());
		}
		else {
			printf("Waiting For Client Connection...\n");

			DWORD rlen = 0;
			while (true)
			{
				if (ConnectNamedPipe(hPipe, NULL) != NULL) //阻塞等待客户端连接。
				{

					if (ReadFile(hPipe, buff_queue[_front_id], 256, &rlen, NULL) == false)   //接受客户端发送过来的内容
					{
						printf("Read data from client fault\n");
						//break;
					}
					else
					{
						buff_queue[_front_id][rlen- 2] = '\0';
						//printf("recieve cmd:%s\n", buff_queue[_front_id]);
						unsigned int front_next_id = _front_id + 1;
						front_next_id %= queque_len;
						_front_id = front_next_id;
					}
					DisconnectNamedPipe(hPipe);
				}
				//Sleep(100);
			}
			CloseHandle(hPipe);
		}
	});
	th_fifo.detach();

#endif
}

void mycmd_update()
{
	for (; _rear_id != _front_id; ++_rear_id, _rear_id = _rear_id %queque_len)
	{
		auto& cur_buff = buff_queue[_rear_id];
		string str_buff(cur_buff);
		auto eq_pos = str_buff.find_last_of('=');
		if (eq_pos != -1)
		{
			string cmd_key = str_buff.substr(0, eq_pos);
			const auto& icmd = g_mp_var_set.find(cmd_key);
			if (icmd != g_mp_var_set.end())
			{
				string str_value = str_buff.substr(eq_pos + 1);
				icmd->second(const_cast<char*>(str_value.c_str()));

			}
		}
	}
}

void cmd_update()
{

	for (; _rear_id != _front_id;++_rear_id,_rear_id=_rear_id %queque_len)
	{
		auto& cur_buff=buff_queue[_rear_id];
		string str_buff(cur_buff);
		auto eq_pos=str_buff.find_last_of('=');
		if(eq_pos!=-1)
		{
			string cmd_key=str_buff.substr(0,eq_pos);
			//printf("cmd_update  cmd_key:%s\n", cmd_key.c_str());
			const auto& icmd=g_mp_var_set.find(cmd_key);
			if(icmd!=g_mp_var_set.end())
			{
				string str_value=str_buff.substr(eq_pos+1);
				icmd->second(const_cast<char*>(str_value.c_str()));
				
			}
		}
	}
	
	/*if(update_cmd)
	{
		update_cmd=false;
		//g_image_show->load_image_file("cat.png");
		if(show_image_path.size()>0)
		{
			g_image_show->load_image_file((char*)show_image_path.c_str());

		}
	}*/
}


