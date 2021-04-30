
#pragma once

#define IMG_NAVI_SIZE 10 * 1024 * 1024 / 8

#ifdef IMGUI_WAYLAND
void socket_init(void);
void delete_socket(void);
int socket_navi();

#endif
