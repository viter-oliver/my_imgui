#include <stdio.h>
#include <windows.h>
#include <iphlpapi.h>

#include "get_web_time.h"
#include "winsock2.h"
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
extern void hex_to_char( vector<unsigned char>& hex_list, string& str_list );
string get_mac_address( vector<BYTE>& vbyte_mac )
{
     IP_ADAPTER_INFO AdapterInfo[ 6 ];       // Allocate information for up to 16 NICs
     DWORD dwBufLen = sizeof( AdapterInfo );  // Save memory size of buffer

     DWORD dwStatus = GetAdaptersInfo( AdapterInfo, &dwBufLen );                  // [in] size of receive data buffer
     if( dwStatus != ERROR_SUCCESS )
     {
          printf( "GetAdaptersInfo failed. err=%d\n", GetLastError() );
          return "";
     }

     PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to  current adapter info
     BYTE* addr = pAdapterInfo->Address;
     vbyte_mac.resize( 6 );
     for( int ix = 0; ix < 6;++ix )
     {
          vbyte_mac[ ix ] = *addr++;
     }
     string mac_str;
     hex_to_char( vbyte_mac, mac_str );
     return mac_str;
     
}


DWORD GetTimeFromServer(char *ip_addr)
{
	if (ip_addr == NULL)
	{
		ip_addr = TEXT("128.138.140.44");
	}
	WSADATA date;
	WORD w = MAKEWORD(2, 0);
	int err = ::WSAStartup(w, &date);
	if (err != 0 )
	{
		printf("WSAStartup error:%d\n",err);
		return 0;
	}
	SOCKET s;
	DWORD m_serverTime;
	s = ::socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == s)
	{
		printf("fail to create socket\n");
		::closesocket(s);
		::WSACleanup();
		return 0;
	}
	sockaddr_in addr;
    addr.sin_family = AF_INET;
	addr.sin_port = htons(37);
	addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);
	if ( ::connect(s, (sockaddr*)&addr, sizeof(addr)) !=0 )
	{
		int errorCode = ::WSAGetLastError();
		switch(errorCode)
		{
		case 10060:
			printf("time out\n");
			break;
		case 10051:
			printf("fail to connect to network\n");
			break;
		default:
			break;
		}
		::closesocket(s);
		::WSACleanup();
		return 0;
	}
	if ( ::recv(s, (char *)&m_serverTime, 4, MSG_PEEK) <= 0 )
	{
		printf("fail to recieve\n");
		::closesocket(s);
		::WSACleanup();
		return 0;
	}
	::closesocket(s);
	::WSACleanup();
	m_serverTime = ::ntohl(m_serverTime);
	//SYSTEMTIME severTImer = FormatServerTime(m_serverTime);
	return m_serverTime;
}

/************************************************************************/
/* 将从毫秒数转化为SYSTEMTIME */
/************************************************************************/
SYSTEMTIME FormatServerTime(DWORD serverTime)
{
	FILETIME ftNew ; 
	SYSTEMTIME stNew ; 

	stNew.wYear = 1900 ;
	stNew.wMonth = 1 ;
	stNew.wDay = 1 ;
	stNew.wHour = 0 ;
	stNew.wMinute = 0 ;
	stNew.wSecond = 0 ;
	stNew.wMilliseconds = 0 ;
	::SystemTimeToFileTime(&stNew, &ftNew);
	LARGE_INTEGER li ;	//64位大整数
	li = * (LARGE_INTEGER *) &ftNew;
	li.QuadPart += (LONGLONG) 10000000 * serverTime; 
	ftNew = * (FILETIME *) &li;
	::FileTimeToSystemTime (&ftNew, &stNew);
	return stNew;
}
