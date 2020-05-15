#include "fonts_edit.h"
#include "dir_output.h"
#include "af_font_res_set.h"
using namespace std;
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
#include "SOIL.h"
#include <stdio.h>
#include <iostream>  
#include "res_internal.h"

//
using namespace auto_future;
ps_font_unit _font_unit;
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
			//ofn.lpstrFilter = "ttf file:\0*.ttf\0\0";
			char strFileName[MAX_PATH] = { 0 };
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = strFileName;
			ofn.nMaxFile = sizeof(strFileName);
			ofn.lpstrTitle = "Loading font file...";
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			if (GetOpenFileName(&ofn))
			{
				string ttf_file = strFileName;
				string ttf_file_name = ttf_file.substr(ttf_file.find_last_of('\\') + 1);
				auto ft_u = g_pfont_face_manager->load_font(ttf_file_name, ttf_file);
				if (!ft_u)
				{
					string err_msg = "fail to load file:";
					err_msg += ttf_file;
					err_msg += "!";
					MessageBox(GetForegroundWindow(), err_msg.c_str(), "Error info", MB_OK);
					throw "fail to load ttf file!";
				}
				ft_u->_char_count_c = sqrt(ft_u->_ft_face->num_glyphs);
				auto ch_rows = ft_u->_char_count_c;
				for (;;ch_rows++)
				{
					auto txt_num = ch_rows*ft_u->_char_count_c;
					if (txt_num>=ft_u->_ft_face->num_glyphs)
					{
						break;
					}
				}
				ft_u->_char_count_r = ch_rows;
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
	//vfont_face_name& font_name_list = g_pfont_face_manager->get_font_name_list();
	auto& dic_fonts = g_pfont_face_manager->get_dic_fonts();
	bool fonts_opened = ImGui::TreeNode("Fonts", "Fonts (%d)", dic_fonts.size());
	
	if (fonts_opened)
	{
		string icon_str = icn_font;
		for (auto& f_item:dic_fonts)
		{
			auto& keynm = f_item->_name;
			auto& font_u = f_item;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (font_u->_sel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, keynm.c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				if (_font_unit)
				{
					_font_unit->_sel = false;
				}
				_font_unit = font_u;
				_font_unit->_sel = true;
			}
			ImGui::TreePop();
		}
		/*int ix = 0;
		for (auto& font_face_item:font_name_list)
		{
		ImGui::TreeNodeEx((void*)(intptr_t)ix, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, font_face_item.c_str());
		ix++;
		}*/
		ImGui::TreePop();
	}
}
void fonts_edit::draw_font_item_pty()
{
	if (_font_unit)
	{
		ImGui::Text("number of glyphs:%d", _font_unit->_ft_face->num_glyphs);
		ImGui::InputInt("cols of texture dic", &_font_unit->_char_count_c);
		ImGui::InputInt("rows of texture dic", &_font_unit->_char_count_r);
	}
}

