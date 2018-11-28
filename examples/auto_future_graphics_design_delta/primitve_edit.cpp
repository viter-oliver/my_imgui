#include "primitve_edit.h"
#include <string>
#include "user_control_imgui.h"
#include <windows.h>
#include <locale.h>  
#include <ShlObj.h>
#include <Commdlg.h>
#ifdef IMGUI_WAYLAND
#include "../../deps/glad/glad.h"
#else
#include <GL/gl3w.h> 
#endif

void primitve_edit::draw_primitive_list()
{
	char str_size[50] = { 0 };
	sprintf(str_size, "count of vertex:%d", _pmobj->_vertex_buf_len);
	ImGui::Text(str_size);
	if (ImGui::Button("load vertices from file..."))
	{
		OPENFILENAME ofn = { sizeof(OPENFILENAME) };
		ofn.hwndOwner = GetForegroundWindow();
		ofn.lpstrFilter = "image file:\0*.fbx;*.obj\0\0";
		char strFileName[MAX_PATH] = { 0 };
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = strFileName;
		ofn.nMaxFile = sizeof(strFileName);
		ofn.lpstrTitle = "Loading vertices...";
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		if (GetOpenFileName(&ofn))
		{
			extern string g_cureent_project_file_path;
			string strimg_file = strFileName;
			string obj_file_name = strimg_file.substr(strimg_file.find_last_of('\\') + 1);
			string obj_file_path = strimg_file.substr(0, strimg_file.find_last_of('\\') + 1);

			string extname = strimg_file.substr(strimg_file.find_last_of('.') + 1);
			if (extname=="fbx")
			{

			}

		}
	}
}

void primitve_edit::draw_primitive_item_property()
{

}
