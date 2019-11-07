#pragma once
#include "ft_video.h"
extern void usb_device_state(int state, wchar_t* dev_name);
extern bool video_link(ft_video* pfv, int& vwidth, int& vheight, char* dev_name);
extern void video_capture_excute();
extern void video_dev_view(void);
extern void video_item_edit(void);