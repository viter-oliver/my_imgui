#include "res_edit.h"
#include "res_output.h"
#include "user_control_imgui.h"
#include <windows.h>
#include <locale.h>  
#include <ShlObj.h>
#include <Commdlg.h>
#include "texture_res_load.h"
#include "control_common_def.h"

res_edit::res_edit()
{
	
}


res_edit::~res_edit()
{

}

void res_edit::draw_res_list()
{
	ImGui::SliderFloat("Width of screen", &base_ui_component::screenw,100.,2000.);
	ImGui::SliderFloat("Height of screen", &base_ui_component::screenh, 100., 2000.);

	int isize=g_vres_texture_list.size();
     ImGui::Combo( "Texture group list", &_cur_texture_id_index, get_texture_group_name, &g_vres_texture_list, isize );
	ImGui::Text("Add new texture group:");
	static char texture_pack_name_str[FILE_NAME_LEN] = "";
	static char texture_data_name_str[FILE_NAME_LEN] = "";
     ImGui::InputText( "texture pack file:", texture_pack_name_str, FILE_NAME_LEN );
     ImGui::InputText( "texture data file:", texture_data_name_str, FILE_NAME_LEN );
     if( ImGui::Button( "new texture group" ) )
     {
          g_vres_texture_list.emplace_back( make_shared<res_texture_list>() );
          res_texture_list& retxt = *g_vres_texture_list.back();
          retxt.texture_pack_file = texture_pack_name_str;
          retxt.texture_data_file = texture_data_name_str;
          if( !load_texture_info( retxt, retxt.texture_pack_file, retxt.texture_data_file ) )
          {
               MessageBox( GetForegroundWindow(), "fail to loading texture information! ", "Error info", MB_OK );
               g_vres_texture_list.erase( g_vres_texture_list.begin() + ( isize - 1 ) );
          }
          memset( texture_pack_name_str, 0, FILE_NAME_LEN );
          memset( texture_data_name_str, 0, FILE_NAME_LEN );
     }
}

void res_edit::draw_res_item_property()
{
	if (g_vres_texture_list.size()>0)
	{
		ImGui::Text("texture data file:");
		ImGui::SameLine();
          auto& res_gp = *g_vres_texture_list[ _cur_texture_id_index ];
          ImGui::Text( res_gp.texture_data_file.c_str() );
          ImGui::Text( "Texture size:%d*%d", res_gp.texture_width, res_gp.texture_height );

          ImGui::Checkbox( "is separated", &res_gp._is_separated );
	}
	else
	{
		ImGui::Text("texture file is empty");
	}

}
