#pragma once
#include <string>
#include <vector>
struct  var_unit
{
	std::string _type;
	void* _value_addr;
	var_unit(std::string& tp, void* vaddr)
		:_type(tp), _value_addr(vaddr)
	{}
};
using variable_list = std::vector<var_unit>;