#include "afg.h"
#include "af_bind.h"
#include "media_info.h"
#include "msg_host_n.h"
#include "enum_txt_name0.h"

using namespace auto_future_utilities;
extern msg_host_n g_msg_host;
#define MAX_CONTENT_LEN 0x100
static char str_show[MAX_CONTENT_LEN] = { 0 };
static bool bvalue = false;
static int media_icon_txt = en_media_fm_png;
MSC_PACK_HEAD
void register_media_cmd_handl()
{
	g_msg_host.attach_monitor("entertainment info",[&](u8*pbuff,int len){
		enum en_sound_type
		{
			en_radio,
			en_usb,
			en_bluetooth,
			en_sound_type_num
		};
		int media_icons[en_sound_type_num] = {
			en_media_fm_png,
			en_media_music_png,
			en_media_bt_png,
		};
		u8 sound_type=*pbuff++;
		if (sound_type>2)
		{
			sound_type = 2;
		}
		media_icon_txt = media_icons[sound_type];
		set_property_aliase_value("control_media_icon", &media_icon_txt);
		u16* pplay_progress=(u16*)pbuff;
		pbuff+=2;
		u16* ptrack_num=(u16*)pbuff;
		pbuff+=2;
		u16* ptrack_list_num=(u16*)pbuff;
		pbuff+=2;
		u8 track_name_len=*pbuff++;
		memcpy(str_show, pbuff, track_name_len);
		set_property_aliase_value("media_content", str_show);
	});
	g_msg_host.attach_monitor("phone call info",[&](u8*pbuff,int len){
		static char numb[10]={'0','1','2','3','4','5','6','7','8','9'};
		//string phone_num;
		enum phone_status
		{
			en_incoming_call,
			en_outgoing_call,
			en_call_active,
			en_calling,
			en_call_inactive,
			en_muticall_active
		};
		u8 phone_type=*pbuff++;
		
		if (phone_type!=en_call_inactive)
		{
			int icon_txt = en_media_phone_call_png;
			set_property_aliase_value("control_media_icon", &icon_txt);
			bvalue = phone_type == en_muticall_active;
			set_property_aliase_value("show_third_call", &bvalue);
		}
		else
		{
			set_property_aliase_value("control_media_icon", &media_icon_txt);
			bvalue = false;
			set_property_aliase_value("show_third_call", &bvalue);
		}
		for(int ix=0;ix<12;ix++)
		{
			str_show[ix]= numb[*pbuff++];
		}
		str_show[12] = '\0';
		u8 user_name_len=*pbuff++;
		char* user_name=(char*)pbuff;
	});
	
}
