#include "fonts_edit.h"
#include <string>
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <windows.h>
#include <locale.h>  
#include <ShlObj.h>
#include <Commdlg.h>
#ifdef IMGUI_WAYLAND
#include "../../deps/glad/glad.h"
#else
#include <GL/gl3w.h> 
#endif
#include "SOIL.h"
#include <stdio.h>
#include <iostream>  
#include "dir_output.h"
#include "af_font_res_set.h"
using namespace std;
using namespace auto_future;
fonts_edit::fonts_edit()
{
}


fonts_edit::~fonts_edit()
{
}
static void ShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
void fonts_edit::draw_fonts_list()
{
	
	try
	{
		if (ImGui::Button("Load new font from ttf file..."))
		{
			OPENFILENAME ofn = { sizeof(OPENFILENAME) };
			ofn.hwndOwner = GetForegroundWindow();
			ofn.lpstrFilter = "ttf file:\0*.ttf\0\0";
			char strFileName[MAX_PATH] = { 0 };
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = strFileName;
			ofn.nMaxFile = sizeof(strFileName);
			ofn.lpstrTitle = "Loading ttf file...";
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			if (GetOpenFileName(&ofn))
			{
				string ttf_file = strFileName;
				string ttf_file_name = ttf_file.substr(ttf_file.find_last_of('\\') + 1);
				if (!g_pfont_face_manager->load_font(ttf_file_name, ttf_file))
				{
					string err_msg = "fail to load file:";
					err_msg += ttf_file;
					err_msg += "!";
					MessageBox(GetForegroundWindow(), err_msg.c_str(), "Error info", MB_OK);
					throw "fail to load ttf file!";
				}

				string ttf_file_path = ttf_file.substr(0, ttf_file.find_last_of('\\') + 1);
			
				extern string g_cureent_directory;
				string str_ttf_path = g_cureent_directory+font_fold;
				if (ttf_file_path != str_ttf_path)
				{
					string str_cmd = "copy ";
					str_cmd += ttf_file;
					str_cmd += " ";
					str_cmd += str_ttf_path;
					system(str_cmd.c_str());
				}
			}
		}
	}
	catch (std::exception& e)
	{
		cout << e.what() << endl;
	}
	vfont_face_name& font_name_list = g_pfont_face_manager->get_font_name_list();
	
	bool fonts_opened = ImGui::TreeNode("Fonts", "Fonts (%d)", font_name_list.size());
	if (fonts_opened)
	{
		int ix = 0;
		for (auto& font_face_item:font_name_list)
		{
			ImGui::TreeNodeEx((void*)(intptr_t)ix, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, font_face_item.c_str());
			ix++;
		}
		ImGui::TreePop();
	}
}

