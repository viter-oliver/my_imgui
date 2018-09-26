#pragma once
#include "control_common_def.h"
#include <msgpack.hpp>
#include <functional>
using namespace auto_future;
class afb_load
{
	base_ui_component*& _pj;
	function<void(base_ui_component*)> _impl;
	void init_ui_component_by_mgo(base_ui_component*&ptar, msgpack::v2::object& obj);
public:
	afb_load(base_ui_component*&pr_root) :_pj(pr_root){}
	~afb_load(){}
	void set_impl(function<void(base_ui_component*)> impl)
	{
		_impl = impl;
	}
	void load_afb(const char* afb_file);
};

