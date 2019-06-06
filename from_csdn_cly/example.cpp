#include <iostream>
#include "HttpBoost.h"
 
int main() {
	std::string str("http://yunhq.sse.com.cn:32041/v1/sh1/snap/204001?callback=jQuery_test&select=name%2Clast%2Cchg_rate%2Cchange%2Camount%2Cvolume%2Copen%2Cprev_close%2Cask%2Cbid%2Chigh%2Clow%2Ctradephase");
	str = cly::get(str);
	std::cout << str.c_str() << std::endl;
 
	str = "http://service.winic.org:8009/sys_port/gateway/[id=13695800360&pwd=13645411460&to=13695800360&content=infomation&time=]";
	str = cly::post(str);
	std::cout << str.c_str() << std::endl;
	
	return 0;
}
