

#include <stdio.h>
//unsigned char buf_img[10 * 1024 * 1024 / 8] = "/0"; // 10M
unsigned char *img_p = NULL;
bool socket_img_ok = false;

#ifndef WIN32
#include "client.h"
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <unistd.h>

#include<string.h>
#include "stdint.h"
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "NormalDlg.h"


void socket_init(void)
{
	if (img_p==NULL)
		img_p = new unsigned char[10 * 1024 * 1024 / 8];
}
void delete_socket(void)
{
	if (img_p)
		delete[]img_p;
}
int socket_navi()
{
    while(1) {

    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("[client] line%d socket create fail\n",__LINE__);
        return -1;
    }
    printf("[client] line%d socket create success\n",__LINE__);

    //服务器(IVI)的为172.20.30.9，端口号8000
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("172.20.30.9");
    serv_addr.sin_port = htons(8000);

    int ret = 0;

    //向服务器发送连接请求
    printf("[client] line%d connecting\n",__LINE__);
    while(1) {
        ret = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

        if (ret == 0)
        {
            break;
        }
        else if (ret == -1)
        {
            sleep(1);
            continue;
        }
    }
    printf("[client] line%d connected success\n",__LINE__);

    unsigned char buf[1024]; // 1M:1024*1024/8
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 500*1000;

    long buf_len = 0;
    long buf_img_len = 0;
    int no_data_count = 0;

    while(1)
    {
        tv.tv_sec = 0;
        tv.tv_usec = 500*1000;

        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO,&fds);
        FD_SET(sock,&fds);

		//printf("[client] line%d sock == %d,before select\n",__LINE__,sock);
        ret = select(sock+1,&fds,NULL,NULL,&tv);
        //printf("[client] line%d ret == %d\n",__LINE__,ret);

        if (ret == -1)
        {
            printf("[client] line%d select err\n",__LINE__);
            no_data_count = 0;
            break;
        }
        else if (ret != 0)
        {
			no_data_count = 0;
            if (FD_ISSET(sock,&fds)) // 可读
            {
		         memset(buf,0,sizeof(buf));
                ret = read(sock, buf, sizeof(buf));
                if (ret > 0)
                {
					// head
					if (ret >= 10 && buf[0] == 0xF && buf[1] == 0xA && buf[2] == 0xA && buf[3] == 0xF) {
						buf_len = buf[4] << 20 | buf[5] << 16 | buf[6] << 12 | buf[7] << 8 | buf[8] << 4 | buf[9];
						buf_img_len = ret-10;
						printf("[client=%d] line%d buf_len=%ld\n",__LINE__,ret,buf_len);
						//memset(buf_img,0,sizeof(buf_img));
						memset(img_p, 0, IMG_NAVI_SIZE);
						memcpy(img_p, buf + 10, ret - 10);
					}
					else
					{
						memcpy(img_p + buf_img_len, buf, ret);
						buf_img_len += ret;
						//printf("[client=%d] \n",ret);
						if (buf_img_len == buf_len) {
							printf("[client] line%d complete a frame:%d\n",__LINE__,buf_img_len);
							socket_img_ok = true;
							PageBigNavi::getInstance()->needShow=true;
						}
						else if (buf_img_len > buf_len) {
							printf("[client] line%d frame len err\n",__LINE__);
						}
					}
                }
                else if (ret ==  0) {
				   printf("[client] line%d read ret == 0\n",__LINE__);
					break;
				}
				else
				{
						printf("[client] line%d read ret < 0\n",__LINE__);
						break;
				}
            }
        }
		else
        {
			if ((++no_data_count)>10) {
                no_data_count = 0;
                printf("[client] line%d no data timeout\n",__LINE__);
				PageBigNavi::getInstance()->needShow=false;
				PageBigNavi::getInstance()->hide();
                break;
            }
            printf("[client] line%d no_data_count == %d\n",__LINE__,no_data_count);
        }
    }

    printf("[client] line%d close socket\n",__LINE__);
    close(sock);
    sleep(1);
    
    }

    return 0;
}


#endif
