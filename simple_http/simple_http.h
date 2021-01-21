/*
DiabloHorn http://diablohorn.wordpress.com
*/
#ifndef _SIMPLE_HTTP
#define _SIMPLE_HTTP
#include <Windows.h>
#ifdef __cplusplus
extern "C"
{
     DWORD http_get_content(char *target, unsigned char **response_data);
     DWORD http_post_binary(char *target, unsigned char *post_data, int post_data_length, unsigned char **response_data);
     /* SSL RELATED FUNCS */
     DWORD https_get_content(char *target, unsigned char **response_data);
     DWORD https_post_binary(char *target, unsigned char *post_data, int post_data_length, unsigned char **response_data);
}
#endif // _cplusplus


#endif
