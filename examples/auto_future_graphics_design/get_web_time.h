#pragma once
#include <string>
#include <vector>
using namespace std;
extern DWORD GetTimeFromServer(char *ip_addr);
extern SYSTEMTIME FormatServerTime(DWORD serverTime);
extern string get_mac_address( vector<BYTE>& vbyte_mac );