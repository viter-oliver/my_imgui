#include "res_edit.h"
#include "res_output.h"
#include "user_control_imgui.h"
#include <windows.h>
#include <locale.h>  
#include <ShlObj.h>
#include <Commdlg.h>
#include "texture_res_load.h"
res_edit::res_edit()
{
	_tmp_current_txt_id_index = g_cur_texture_id_index;
	
}


res_edit::~res_edit()
{

}

void res_edit::draw_res_list()
{
	int isize=g_vres_texture_list.size();
	static const char* texture_res_show[]{"texture0", "texture1", "texture2", "texture3", "texture4",};
	ImGui::Combo("Spliced texture list", &_tmp_current_txt_id_index, texture_res_show, isize);
	if (_tmp_current_txt_id_index != g_cur_texture_id_index)
	{
		g_cur_texture_id_index = _tmp_current_txt_id_index;
	}
	ImGui::Text("Add new spliced texture:");
	static char texture_pack_name_str[FILE_NAME_LEN] = "";
	static char texture_data_name_str[FILE_NAME_LEN] = "";
	if (isize < sizeof(texture_res_show))
	{
		ImGui::InputText("texture pack file:", texture_pack_name_str, FILE_NAME_LEN);
		ImGui::InputText("texture data file:", texture_data_name_str, FILE_NAME_LEN);
		if (ImGui::Button("new texture resource"))
		{
			g_vres_texture_list.push_back(res_texture_list());
			int isize = g_vres_texture_list.size();
			res_texture_list& retxt = g_vres_texture_list[isize - 1];

			retxt.texture_pack_file = texture_pack_name_str;
			retxt.texture_data_file = texture_data_name_str;
			if (!load_texture_info(retxt, retxt.texture_pack_file, retxt.texture_data_file))
			{
				MessageBox(GetForegroundWindow(), "fail to loading texture information! ", "Error info", MB_OK);
				g_vres_texture_list.erase(g_vres_texture_list.begin()+ (isize - 1));
			}
			memset(texture_pack_name_str, 0, FILE_NAME_LEN);
			memset(texture_data_name_str, 0, FILE_NAME_LEN);
		
		}
	}


}

void res_edit::draw_res_item_property()
{
	if (g_vres_texture_list.size()>0)
	{
		ImGui::Text("texture pack file:");
		ImGui::SameLine();
		ImGui::Text(g_vres_texture_list[g_cur_texture_id_index].texture_pack_file.c_str());
		ImGui::Text("texture data file:");
		ImGui::SameLine();
		ImGui::Text(g_vres_texture_list[g_cur_texture_id_index].texture_data_file.c_str());
	}
	else
	{
		ImGui::Text("texture file is empty");
	}

}
