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
			Value screenw = jroot["screenw"];
			Value screenh = jroot["screenh"];
			if (!screenw.isNull())
			{
				base_ui_component::screenw = screenw.asDouble();
			}
			else
			{
			    base_ui_component::screenw = 2000.0f;
			}
			if (!screenh.isNull())
			{
				base_ui_component::screenh = screenh.asDouble();
			}
			else
			{
				base_ui_component::screenh = 1000.0f;
			}

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
