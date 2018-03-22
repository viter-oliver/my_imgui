#include "ui_assembler.h"
#include <fstream>
#include "texture_res_load.h"

bool ui_assembler::load_ui_component_from_file(const char* file_path)
{
	ifstream fin;
	fin.open(file_path);
	if (fin.is_open())
	{
		Reader reader;
		Value jroot;
		if (reader.parse(fin, jroot, false))
		{
			texture_res_load tresload(g_vres_texture_list);
			tresload.load_res_from_json(jroot);
			_root.init_from_json(jroot);
		}
		fin.close();
	}
	else
	{
		printf("invalid file_path:%s\n", file_path);
		return false;
	}
	return true;
}
