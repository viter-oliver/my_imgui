//////////////////////////////////////////////////////////////////////////
// 描述： 通过命令行方式得到MAC地址
// 作者： 郭洪锋
// 日期： 2005年7月1日

// email: guohongfeng@gmail.com
/////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <windows.h>
#include <string>
#include <iostream>
using namespace std;

//命令行输出缓冲大小
const long MAX_COMMAND_SIZE = 10000;

//获取MAC命令行
char szFetCmd[] = "ipconfig /all";
//网卡MAC地址的前导信息
const string str4Search = "Physical Address";

//用命令行方式获取网卡MAC地址
BOOL GetMacByCmd(char *lpszMac);

////////////////////////////////////////////////////////////////////////////
// 函数名： GetMacByCmd(char *lpszMac)
// 参数：
//      输入： void
//      输出： lpszMac,返回的MAC地址串
// 返回值：
//      TRUE:  获得MAC地址。
//      FALSE: 获取MAC地址失败。
// 过程：
//      1. 创建一个无名管道。
//      2. 创建一个IPCONFIG 的进程，并将输出重定向到管道。
//      3. 从管道获取命令行返回的所有信息放入缓冲区lpszBuffer。
//      4. 从缓冲区lpszBuffer中获得抽取出MAC串。
//
//  提示：可以方便的由此程序获得IP地址等其他信息。
//        对于其他的可以通过其他命令方式得到的信息只需改变strFetCmd 和 
//        str4Search的内容即可。
///////////////////////////////////////////////////////////////////////////

BOOL GetMacByCmd(char *lpszMac)
{
	//初始化返回MAC地址缓冲区
	BOOL bret; 

	SECURITY_ATTRIBUTES sa; 
	HANDLE hReadPipe,hWritePipe;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	sa.lpSecurityDescriptor = NULL; 
	sa.bInheritHandle = TRUE; 
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if(!bret)
	{
		return FALSE;
	}

	STARTUPINFO si; 
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFO); 
    GetStartupInfo(&si); 
    si.hStdError = hWritePipe; 
    si.hStdOutput = hWritePipe; 
    si.wShowWindow = SW_HIDE; 
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	bret = CreateProcess (NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, 
      NULL, &si, &pi ); 
 
    char szBuffer[MAX_COMMAND_SIZE+1]; 
	string strBuffer;

	 if (bret) 
	 { 
		  WaitForSingleObject (pi.hProcess, INFINITE); 
		  unsigned long count;

		  memset(szBuffer, 0x00, sizeof(szBuffer));
		  bret  =  ReadFile(hReadPipe,  szBuffer,  MAX_COMMAND_SIZE,  &count,  0);
		  if(!bret)
		  {
			   //关闭所有的句柄
			   CloseHandle(hWritePipe);
			   CloseHandle(pi.hProcess); 
			   CloseHandle(pi.hThread); 
			   CloseHandle(hReadPipe);
			   return FALSE;
		  }
		  else
		  {
			   strBuffer = szBuffer;
			   long ipos;
			   ipos = strBuffer.find(str4Search);
			   strBuffer = strBuffer.substr(ipos+str4Search.length());
			   ipos = strBuffer.find(":");
			   strBuffer = strBuffer.substr(ipos+1);
			   ipos = strBuffer.find("\r\n");
			   strBuffer = strBuffer.substr(0, ipos);
		  }
  
	 }

 strcpy(lpszMac, strBuffer.c_str());

 //去掉中间的“00-50-EB-0F-27-82”中间的'-'得到0050EB0F2782


 //关闭所有的句柄
 CloseHandle(hWritePipe);
 CloseHandle(pi.hProcess); 
 CloseHandle(pi.hThread); 
 CloseHandle(hReadPipe);
 return TRUE;

}

