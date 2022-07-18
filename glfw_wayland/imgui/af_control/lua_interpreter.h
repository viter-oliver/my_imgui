#pragma once
#include "interpreter_vardef.h"
namespace lua_interpreter
{
	extern const char* lua_func_head;
	extern const char* lua_func_tail;
	bool call_lua_fun(std::string& func_exp, var_unit& fun_retn, variable_list& vlist);
}
