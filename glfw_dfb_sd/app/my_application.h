#pragma once
#include "application.h"

#ifdef IMGUI_WAYLAND
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "InterProcComm.h"
#include "MsgInterface.h"
#include "TypedefSerialize.h"
#include "UartTypedefSerialize.h"
#include "RpcDataDefine.h"
#include "RpcMsgInterface.h"
//#include "MCUMsgCallback.h"
#endif

class my_application :
	public auto_future::application
{
	bool _be_uart_comm;
public:
	my_application(int argc, char **argv);
	void resLoaded();
	void onUpdate();
	void UpdateUartComm(std::string str);

#ifdef IMGUI_WAYLAND
	void UpdateMusicInfor(StuMusicInformation data);
	void UpdateRadioInfor(StuRadioInformation data);
	void UpdatePhoneInfor(StuPhoneInformation data);

#endif

	virtual ~my_application();

#ifdef IMGUI_WAYLAND
public:
	CInterProcComm  ojIPC;
#endif
};

