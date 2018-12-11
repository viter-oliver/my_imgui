#pragma once
#include "control_common_def.h"
#include <algorithm>
namespace auto_future
{
	enum cmd_value_type
	{
		en_cmd_float,
		en_cmd_double,
		en_cmd_bool,
		en_cmd_int,
		en_cmd_str,
	};
	struct command_value
	{
		cmd_value_type _cmd_value_type;
		union
		{
			float _f;
			bool _b;
			double _d;
			int _i;
			char* _s;
		}_value;
		command_value()
		{
			command_value(0);
		}
		command_value(command_value&tar)
			:_cmd_value_type(tar._cmd_value_type)
		{
			switch (tar._cmd_value_type)
			{
			case en_cmd_str:
				{
					_value._s = 0;
					int sl = strlen(tar._value._s);
					if (sl > 0)
					{
						_value._s = static_cast<char*>(malloc(sl + 1));
						memcpy(_value._s, tar._value._s, sl);
						_value._s[sl] = 0;
					}
				}
				break;
			default:
				_value = tar._value;
			}
		}
		command_value(float fvalue)
			:_cmd_value_type(en_cmd_float)
		{
			_value._f = fvalue;
		}
		command_value(double dvalue)
			:_cmd_value_type(en_cmd_double){
			_value._d = dvalue;
		}
		command_value(bool bvalue)
			:_cmd_value_type(en_cmd_bool)
		{
			_value._b = bvalue;
		}
		command_value(int ivalue)
			:_cmd_value_type(en_cmd_int)
		{
			_value._i = ivalue;
		}
		command_value(char* svalue)
			:_cmd_value_type(en_cmd_str)
		{
			_value._s = 0;
			int sl = strlen(svalue);
			if (sl>0)
			{
				_value._s= static_cast<char*>(malloc(sl + 1));
				memcpy(_value._s, svalue, sl);
				_value._s[sl] = 0;
			}

		}
		~command_value()
		{
			if (_cmd_value_type == en_cmd_str&&_value._s)
			{
				free(_value._s);
				_value._s=0;
			}
		}
		void assign_value_to(void* var_address)
		{
			switch (_cmd_value_type)
			{
			case en_cmd_float:
				*(float*)var_address = _value._f;
				break;
			case en_cmd_double:
				*(double*)var_address = _value._f;
				break;
			case en_cmd_bool:
				*(bool*)var_address = _value._b;
				break;
			case en_cmd_int:
				*(int*)var_address = _value._i;
				break;
			case en_cmd_str:
				memcpy((char*)var_address, _value._s, strlen(_value._s));
				*((char*)var_address + strlen(_value._s)) = 0;
				break;
			default:
				break;
			}
		}
		void get_value_from(void* var_address)
		{
			switch (_cmd_value_type)
			{
			case auto_future::en_cmd_float:
				_value._f = *(float*)var_address;
				break;
			case auto_future::en_cmd_double:
				_value._d = *(double*)var_address;
				break;
			case auto_future::en_cmd_bool:
				_value._b = *(bool*)var_address;
				break;
			case auto_future::en_cmd_int:
				_value._i = *(int*)var_address;
				break;
			case auto_future::en_cmd_str:
				if (_value._s == 0 || strlen(_value._s) < strlen((char*)var_address)+1&&(free(_value._s),_value._s=0))
				{
					_value._s = (char*)malloc(strlen((char*)var_address) + 1);
				}
				
				memcpy(_value._s, var_address, strlen((char*)var_address));
				_value._s[strlen((char*)var_address)] = 0;
				break;
			default:
				break;
			}
		}
	};

	template<class T> struct edit_commd
	{
		T* _phost;
		void* _var_address;
		command_value _cmd_value;
		edit_commd(T* phost, void* var_address, command_value&cmd_value)
			:_phost(phost), _var_address(var_address), _cmd_value(cmd_value){}
		void execute()
		{
			_cmd_value.assign_value_to(_var_address);
		}
		edit_commd<T> clone_command()
		{
			command_value cmdv;
			cmdv._cmd_value_type = _cmd_value._cmd_value_type;
			cmdv.get_value_from(_var_address);
			return edit_commd<T>(_phost, _var_address,cmdv);
		}
	};
	template<class T> class commmand_manager
	{
		int _cur_command_id = {-1};
		vector<edit_commd<T>> _edit_command_list;
		bool _lack_cur_value_cmd = { false };
	public:
		commmand_manager(){}
		void create_command(edit_commd<T>& ecommd)
		{
			if (_cur_command_id<_edit_command_list.size()-1)
			{
				_edit_command_list.erase(_edit_command_list.begin() + _cur_command_id, _edit_command_list.end());
			}
			_lack_cur_value_cmd = true;
			_edit_command_list.emplace_back(ecommd);
			_cur_command_id++;
		}
		void redo_command()
		{
			_cur_command_id++;
			auto cur_cmd = _edit_command_list[_cur_command_id];
			cur_cmd.execute();
		}
		void undo_command()
		{
			if (_lack_cur_value_cmd)
			{//store last unchanged data
 				edit_commd<T> ecommd = _edit_command_list[_cur_command_id].clone_command();
				_edit_command_list.emplace_back(ecommd);
				_lack_cur_value_cmd = false;
			}
			else
				_cur_command_id--;
			auto cur_cmd = _edit_command_list[_cur_command_id];
			cur_cmd.execute();
		}
		bool redo_able(){return _cur_command_id < _edit_command_list.size() - 1; }
		bool undo_able(){ return _cur_command_id > 0 || _cur_command_id == _edit_command_list.size()-1; }
		void clear_cmds_by_component(T* target)
		{
			_cur_command_id = 0;
			_edit_command_list.erase(remove_if(_edit_command_list.begin(), _edit_command_list.end(), [target](edit_commd<T>& ecmd){
				return ecmd._phost == target;
			}), _edit_command_list.end());
		}
	};

	extern commmand_manager<base_ui_component> g_ui_edit_command_mg;

}
