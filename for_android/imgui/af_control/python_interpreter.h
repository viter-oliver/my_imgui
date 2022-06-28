#pragma once

#include <string>
#include <vector>
using namespace std;
extern const string python_pre_define;
extern const string python_fun_head;
extern const string python_fun_name;
struct  var_unit
{
	string _type;
	void* _value_addr;
	var_unit(string& tp,void* vaddr)
		:_type(tp), _value_addr(vaddr)
	{}
};
using variable_list = vector<var_unit>;
class python_interpreter
{

public:
	python_interpreter();
	~python_interpreter();
	bool call_python_fun(string& fun_exp,string fun_name, var_unit& fun_retn, variable_list& vlist);
};
extern python_interpreter g_python_intp;

