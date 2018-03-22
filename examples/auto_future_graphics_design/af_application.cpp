#include "af_application.h"




bool af_application::register_update_fun(const char* fun_name, function<void(void)> update_fun)
{
	auto itf = _mp_update_funs.find(fun_name);
	if (itf != _mp_update_funs.end())
	{
		printf("fail to register update function:%s!\n", fun_name);
		return false;
	}
	_mp_update_funs[fun_name] = update_fun;
	return true;
}

void af_application::update()
{
	for (auto itf:_mp_update_funs)
	{
		itf.second();
	}
}
