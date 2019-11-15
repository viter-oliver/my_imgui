#pragma once
#include<functional>
#include <thread>
using namespace auto_future;
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
DWORD WINAPI ThreadLoadApps(LPVOID lpParameter);

class scomm
{
	HANDLE _hcomm = { INVALID_HANDLE_VALUE };
	int _nmb;
	int _baudrate;
	bool _running = false;
	msg_handle _msg_handl;
#ifdef _OVER_LAP_OP
	OVERLAPPED m_osReader;
	OVERLAPPED m_osWriter;
#endif
	uint8_t calc_check_sum(uint8_t * pBuf, int iLen)
	{
		int i = 0;
		uint8_t check_sum = 0;
		check_sum = pBuf[0];
		for (i = 1; i < iLen; i++)
		{
			check_sum ^= pBuf[i];
		}
		return check_sum;
	}
public:
	scomm()
	{}
	~scomm()
	{
		if (_hcomm != INVALID_HANDLE_VALUE)
		{
			PurgeComm(_hcomm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_TXABORT | PURGE_RXABORT);
			unsigned long etat;
			ClearCommError(_hcomm, &etat, NULL);
			CloseHandle(_hcomm);
		}
	}
	bool is_open()
	{
		return _hcomm != INVALID_HANDLE_VALUE;
	}
	bool open(int nmb, int baudrate)
	{
		_nmb = nmb;
		_baudrate = baudrate;
		char comm_str[50] = { 0 };
		if (nmb<10)
		{
			sprintf(comm_str, "COM%d", _nmb);
		}
		else
		{
			sprintf(comm_str, "\\\\.\\COM%d", _nmb);
		}
		_hcomm = CreateFile(comm_str, GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_EXISTING,
#ifdef _OVER_LAP_OP
			FILE_FLAG_OVERLAPPED, 
#else
			NULL,
#endif 
			NULL);
		if (_hcomm == INVALID_HANDLE_VALUE)
		{
			printf("fail to open%s!\n", comm_str);
			return false;
		}
#ifdef _OVER_LAP_OP
		FillMemory(&m_osReader, sizeof(OVERLAPPED), 0);
		FillMemory(&m_osWriter, sizeof(OVERLAPPED), 0);
		m_osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_osWriter.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
		int byteUsedTime = 14400 / baudrate + 1;
		COMMTIMEOUTS timeouts = { 20 + byteUsedTime, byteUsedTime, 1000, byteUsedTime, 20 };
		if (!SetCommTimeouts(_hcomm, &timeouts))
		{
			printf("fail to SetCommTimeouts!\n");
			return false;
		}
		DCB commParam;
		if (!GetCommState(_hcomm, &commParam))
		{
			printf("fail to get commstate!\n");
			return false;
		}
		commParam.BaudRate = _baudrate;				// 设置波特率 

		commParam.fBinary = TRUE;					// 设置二进制模式，此处必须设置TRUE
		commParam.fParity = FALSE;					// 支持奇偶校验 
		commParam.ByteSize = 8;						// 数据位,范围:4-8 
		commParam.Parity = NOPARITY;				// 校验模式
		//commParam.StopBits = ONESTOPBIT;			// 停止位 

		//commParam.fOutxCtsFlow = FALSE;				// No CTS output flow control 
		//commParam.fOutxDsrFlow = FALSE;				// No DSR output flow control 
		commParam.fDtrControl = 0;
		//// DTR flow control type 
		//commParam.fDsrSensitivity = FALSE;			// DSR sensitivity 
		commParam.fTXContinueOnXoff = 0;			// XOFF continues Tx 
		commParam.fOutX = FALSE;					// No XON/XOFF out flow control 
		commParam.fInX = FALSE;						// No XON/XOFF in flow control 
		//commParam.fErrorChar = FALSE;				// Disable error replacement 
		//commParam.fNull = FALSE;					// Disable null stripping 
		commParam.fRtsControl = 0;
		// RTS flow control 
		commParam.fAbortOnError = FALSE;			// 当串口发生错误，并不终止串口读写

		if (!SetCommState(_hcomm, &commParam))
		{
			printf(" fail to setCommState!\n");
			return false;
		}
		//SetupComm(_hcomm, 0x400, 0x400);
	

		//指定端口监测的事件集
		SetCommMask(_hcomm, EV_RXCHAR);

		//分配设备缓冲区
		::SetupComm(_hcomm, 4096, 4096);

		//初始化缓冲区中的信息
		::PurgeComm(_hcomm, PURGE_TXCLEAR | PURGE_RXCLEAR);

	}
	void stop()
	{
		_running = false;
	}
	void set_msg_handle(msg_handle msg_hand)
	{
		swap(_msg_handl, msg_hand);
	}
	void send_data(u8* pbuf, DWORD dataLen)
	{
		DWORD dwBytesWrite;
		COMSTAT ComStat;
		DWORD dwErrorFlags;
		BOOL bWriteStat;
		ClearCommError(_hcomm, &dwErrorFlags, &ComStat);
		bWriteStat = WriteFile(_hcomm, pbuf, dataLen, &dwBytesWrite, NULL);
		if (!bWriteStat)
		{
			printf("fail to write data to com\n");
		}
	}
	DWORD ReadData(LPVOID lpBuf, DWORD dwToRead)
	{
		//TRACE("RRRRRRRRRRRR 00\n"); 
		if (_hcomm == INVALID_HANDLE_VALUE) return 0;
		DWORD dwRead;
		if (ReadFile(_hcomm, lpBuf, dwToRead, &dwRead,
#ifdef _OVER_LAP_OP
			&m_osReader
#else
			NULL
#endif
			))
			return dwRead;
		else
			return 0;
#ifdef _OVER_LAP_OP
		if (GetLastError() != ERROR_IO_PENDING)  return 0;

		if (WaitForSingleObject(m_osReader.hEvent, INFINITE) != WAIT_OBJECT_0)
			return 0;
		if (!GetOverlappedResult(_hcomm, &m_osReader, &dwRead, FALSE))
			return 0;
		//TRACE("RRRRRRRRRRRR 11\n");  
		return dwRead;
#endif
	}


	void thd_process()// operator()()
	{
		_running = true;
		DWORD dwBytesRead;
		
		

		uint8_t *pdata = new uint8_t[MAX_BYTE_SIZE];
		uint8_t *data_cache = new uint8_t[MAX_CACH_SIZE];
		uint8_t*data_cmd = new uint8_t[MAX_BYTE_SIZE];

		int ifront = 0, irear = 0;
		DWORD EventMask = EV_RXCHAR;
		COMSTAT ComStat;
		DWORD dwErrorFlags;
		BOOL bReadState = FALSE;
		while (_running)
		{
			if (WaitCommEvent(_hcomm, &EventMask, NULL) && (EventMask & EV_RXCHAR))
			{
				dwBytesRead = 0;
				ClearCommError(_hcomm, &dwErrorFlags, &ComStat);
				if (ComStat.cbInQue > 0)
				{
					bReadState = ::ReadFile(_hcomm, pdata, ComStat.cbInQue, &dwBytesRead, NULL);
					if (bReadState && dwBytesRead > 0)
					{
						/*printf("get frame:");

						for (int ix = 0; ix < dwBytesRead; ix++)
						{
						printf("%0x ", pdata[ix]);
						}
						printf("\n");*/
					}
					else
						continue;
				}
			}
			else
			{
				Sleep(50);
				continue;
			}

			if (ifront + dwBytesRead <= MAX_CACH_SIZE)
			{
				memcpy(data_cache + ifront, pdata, dwBytesRead);
			}
			else
			{
				int itrim = MAX_CACH_SIZE - ifront;
				memcpy(data_cache + ifront, pdata, itrim);
				memcpy(data_cache, pdata + itrim, dwBytesRead - itrim);
			}
			//
			ifront += dwBytesRead;
			ifront %= MAX_CACH_SIZE;
			//continue;
			for (; irear != ifront; ++irear, irear %= MAX_CACH_SIZE)
			{
				static uint8_t prebyte = 0, check_sum = 0;
				static bool be_picking_data = false;
				static int pick_index = 0;
				static uint16_t frame_len;
		
				if (!be_picking_data)
				{
					if (prebyte == MCU_SYNC1&&data_cache[irear] == MCU_SYNC2)
					{
						be_picking_data = true;
						pick_index = 0;
						prebyte = 0;
						check_sum = MCU_SYNC1^MCU_SYNC2;
					}
					prebyte = data_cache[irear];
				}
				else
				{
					//printf("pick_index=%d frame_len=%d\n", pick_index, frame_len);
					data_cmd[pick_index] = data_cache[irear];
					bool finish_frame = false;
					if (pick_index == en_pos_length_l)
					{
						uint16_t* plen = (uint16_t*)& data_cmd[en_pos_length_h];
						
						frame_len = *plen;// g_data_cmd[en_pos_length_h]*0x100+g_data_cmd[en_pos_length_l];
						if (frame_len>MAX_BYTE_SIZE - en_pos_length_l)
						{
							printf("invalid command length:%d\n", frame_len);
							be_picking_data = false;
							finish_frame = true;
						}
					}
					else
					if (pick_index == (frame_len + en_pos_length_l))
					{
						uint8_t fchk_sum = data_cmd[pick_index];
						if (fchk_sum == check_sum)
						{
							//RmtAdpter::GetInstance()->interpretCommand(g_data_cmd + 3);
							//printf("get a frame data\n");
							//g_msg_host.pick_valid_data(&data_cmd[en_pos_cmd_head], frame_len - 1);
							_msg_handl(&data_cmd[en_pos_cmd_head], frame_len - 1);
						}
						else
						{
							printf("fail to checksum1\n");
						}
						be_picking_data = false;
						finish_frame = true;
					}
					if (!finish_frame)
					{
						check_sum ^= data_cmd[pick_index];
						pick_index++;
					}
					
				}
			}
		}

		delete[] pdata;
		delete[] data_cmd;
		delete[] data_cache;
	}
};
DWORD WINAPI ThreadLoadApps(LPVOID lpParameter)
{
	scomm* pHost = static_cast<scomm*>(lpParameter);
	//hevent_refresh_area=CreateEvent(NULL,false,false,MAINMENU_REFRESH_AREA_EVNT_NAME);
	pHost->thd_process();
	return 0;
}