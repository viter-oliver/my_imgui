#pragma once
#include <functional>
#include <map>
#include <string>
using namespace std;
using handle_var_set=function<void(char*) >;
using mp_var_set=map<string,handle_var_set>;
extern mp_var_set g_mp_var_set;
void init_var_set_fifo();
void cmd_update();