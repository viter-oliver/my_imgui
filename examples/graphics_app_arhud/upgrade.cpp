
#include "upgrade.h"
#include "msg_host_n.h"
using namespace auto_future_utilities;
extern msg_host_n g_msg_host;
void register_upgrade_cmd_handl()
{
	g_msg_host.attach_monitor("upgrade state",[&](u8*pbuff,int len){
		u8 upgrade_state=*pbuff++;
	});
	g_msg_host.attach_monitor("request next package",[&](u8*pbuff,int len){
		u16* ppackage_index=(u16*)pbuff;
	});
	g_msg_host.attach_monitor("request update",[&](u8*pbuff,int len){
		struct request_update
		{
			u8 update_bootloader:1;
			u8 update_app:1;
			u8 reserved:6;
		};
		request_update* prequest_update=(request_update*)pbuff;
	});
	
	
}
