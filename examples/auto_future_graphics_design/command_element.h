#pragma once
#include "control_common_def.h"
namespace auto_future
{
	enum cmd_value_type
	{
		en_cmd_float,
		en_cmd_bool,
		en_cmd_int,
		en_cmd_str,
	};
	struct command_value
	{
		cmd_value_type _cmd_value_type;
		union
		{
			float _fvalue;
			bool _bvalue;
			int _ivalue;
			char* _svalue;
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
					_value._svalue = 0;
					int sl = strlen(tar._value._svalue);
					if (sl > 0)
					{
						_value._svalue = static_cast<char*>(malloc(sl + 1));
						memcpy(_value._svalue, tar._value._svalue, sl);
						_value._svalue[sl] = 0;
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
			_value._fvalue = fvalue;
		}
		command_value(bool bvalue)
			:_cmd_value_type(en_cmd_bool)
		{
			_value._bvalue = bvalue;
		}
		command_value(int ivalue)
			:_cmd_value_type(en_cmd_int)
		{
			_value._ivalue = ivalue;
		}
		command_value(char* svalue)
			:_cmd_value_type(en_cmd_str)
		{
			
			_value._svalue = 0;
			int sl = strlen(svalue);
			if (sl>0)
			{
				_value._svalue = static_cast<char*>(malloc(sl + 1));
				memcpy(_value._svalue, svalue, sl);
				_value._svalue[sl] = 0;
			}

		}
		~command_value()
		{
			if (_cmd_value_type == en_cmd_str&&_value._svalue)
			{
				free(_value._svalue);
				_value._svalue=0;
			}
		}
	};
	struct command_elemment 
	{
		string _cmd_type;
		unsigned char _cmd_id;
		command_value _cmd_value;
		command_elemment()
			:_cmd_value(0)
		{

		}
		command_elemment(string&cmd_type, unsigned char cmd_id, command_value& cmd_value)
			:_cmd_type(cmd_type),_cmd_id(cmd_id), _cmd_value(cmd_value){}
		command_elemment(command_elemment& tar)
			:_cmd_type(tar._cmd_type)
			, _cmd_id(tar._cmd_id)
			, _cmd_value(tar._cmd_value)
		{

		}
	};
	template<class T> struct edit_commd
	{
		T* _phost;
		command_elemment _cmd_ele;
		edit_commd(T* phost, command_elemment& cmd_ele)
			:_phost(phost), _cmd_ele(cmd_ele){}
		void exe_cute()
		{
			_phost->execute_command(_cmd_ele);
		}
		edit_commd<T> clone_command()
		{
			command_elemment clone_cmd_ele=_phost->clone_cmd_ele(_cmd_ele);
			return edit_commd<T>(_phost, clone_cmd_ele);
		}
	};
	template<class T> class commmand_manager
	{
		int _cur_command_id = {-1};
		vector<edit_commd<T>> _edit_command_list;
	public:
		commmand_manager(){}
		void create_command(edit_commd<T>& ecommd)
		{
			if (_cur_command_id<_edit_command_list.size()-1)
			{
				_edit_command_list.erase(_edit_command_list.begin() + _cur_command_id, _edit_command_list.end());
			}
			_edit_command_list.emplace_back(ecommd);
			_cur_command_id++;
		}
		void redo_command()
		{
			_cur_command_id++;
			auto cur_cmd = _edit_command_list[_cur_command_id];
			cur_cmd.exe_cute();
		}
		void undo_command()
		{
			if (_cur_command_id == _edit_command_list.size() - 1)
			{//store last unchanged data
				edit_commd<T> ecommd = _edit_command_list[_cur_command_id].clone_command();
				_edit_command_list.emplace_back(ecommd);
			}
			else
				_cur_command_id--;
			auto cur_cmd = _edit_command_list[_cur_command_id];
			cur_cmd.exe_cute();
		}
		bool redo_able(){return _cur_command_id < _edit_command_list.size() - 1; }
		bool undo_able(){ return _cur_command_id > 0; }
	};

	extern commmand_manager<base_ui_component> g_ui_edit_command_mg;

}
