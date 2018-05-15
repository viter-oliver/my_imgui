#pragma once
#include<functional>
namespace auto_future_utilities
{
	typedef unsigned char u8;
	typedef unsigned short u16;
	typedef std::function<void(u8*, int)> msg_handle;

}