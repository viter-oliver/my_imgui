#pragma once
#include <Python.h>
#include "interpreter_vardef.h"
using namespace std;
extern const string python_pre_define;
extern const string python_fun_head;
extern const string python_fun_name;

class python_interpreter
{
	PyObject *_pModule, *_pGlobal, *_pLocal;
public:
	python_interpreter();
	~python_interpreter();
	bool call_python_fun(string& fun_exp,string fun_name, var_unit& fun_retn, variable_list& vlist);
};
extern python_interpreter g_python_intp;

