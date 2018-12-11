#include "command_element_delta.h"
namespace auto_future
{
	

	/*template<class T>
	void auto_future::commmand_manager<T>::create_command(edit_commd<T>& ecommd)
	{
	_edit_command_list.erase(_edit_command_list.begin() + _cur_command_id, _edit_command_list.end());
	_edit_command_list.emplace_back(ecommd);
	_cur_command_id++;
	}
	template<class T>
	void commmand_manager<T>::redo_command()
	{
	auto cur_cmd = _edit_command_list[_cur_command_id];
	cur_cmd.exe_cute();
	_cur_command_id++;
	}
	template<class T>
	void commmand_manager<T>::undo_command()
	{
	_cur_command_id--;
	auto cur_cmd = _edit_command_list[_cur_command_id];
	cur_cmd.exe_cute();
	}*/
	commmand_manager<base_ui_component> g_ui_edit_command_mg;
}