#pragma once
#include<functional>
#define AFG_EXPORT
namespace auto_future_utilities
{
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned int u32;
    typedef char s8;
    typedef short s16;
    typedef int s32;
    typedef std::function<void(u8*, int)> msg_handle;

}