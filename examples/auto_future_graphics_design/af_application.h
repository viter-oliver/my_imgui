#pragma once
#include <map>
#include <functional>
#include <string>
using namespace std;
class af_application
{
	map<string, function<void(void)>> _mp_update_funs;
public:
	bool register_update_fun(const char* fun_name, function<void(void)> update_fun);
	void update();
};

