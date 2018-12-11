#pragma once
#include "control_common_def.h"
#include <algorithm>
namespace auto_future
{
	
	typedef vector<unsigned char> cmd_value_block;
	
	template<class T> struct edit_commd
	{
		T* _phost;
		void* _var_address;
		cmd_value_block _cmd_value;
		edit_commd(T* phost, void* var_address, void* value_address,size_t var_sz)
			:_phost(phost), _var_address(var_address){
			_cmd_value.reserve(var_sz);
			_cmd_value.resize(var_sz);
			memcpy(&_cmd_value[0], value_address, var_sz);
		}
		void execute()
		{
			memcpy(_var_address, &_cmd_value[0], _cmd_value.size());
		}
		edit_commd<T> clone_command()
		{
			return edit_commd<T>(_phost, _var_address,_var_address,_cmd_value.size());
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
			_cur_command_id = _cur_command_id<_edit_command_list.size() - 1;
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
