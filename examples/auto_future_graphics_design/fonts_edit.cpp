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
using namespace std;
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
		static float font_size = 16.f;
		ImGui::InputFloat("load font size:", &font_size, 0.1, 10.f, 32.f);
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
				ImFontAtlas* atlas = ImGui::GetIO().Fonts;
				for (size_t i = 0; i < atlas->Fonts.size();i++)
				{
					ImFont* font = atlas->Fonts[i];
					string font_name = font->ConfigData[0].Name;
					font_name = font_name.substr(0, font_name.find_first_of(','));
					if (font_name == ttf_file_name)
					{
						MessageBox(GetForegroundWindow(), "ttf file you selected is already in the fonts list!", "Error info", MB_OK);
						throw "fail to load ttf file!";
					}
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
				atlas->AddFontFromFileTTF(strFileName, font_size, NULL, atlas->GetGlyphRangesChinese());
				font_size = 16.f;
			}
		}
	}
	catch (std::exception& e)
	{
		cout << e.what() << endl;
	}
	
	
	ImGuiStyle& style = ImGui::GetStyle();
	bool fonts_opened = ImGui::TreeNode("Fonts", "Fonts (%d)", ImGui::GetIO().Fonts->Fonts.Size);
	if (fonts_opened)
	{
		ImFontAtlas* atlas = ImGui::GetIO().Fonts;
		for (int i = 0; i < atlas->ConfigData.size(); i++)
		{
			auto& cfg_data = atlas->ConfigData[i];
			ImFont* font = atlas->Fonts[i];
			//ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Pink");
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool is_current_font = ImGui::GetIO().FontDefault == font;
			if (is_current_font)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			if (ImGui::TreeNodeEx((void*)(intptr_t)i,node_flags,cfg_data.Name))
			{
				ImGui::Text("Font data size:%d", cfg_data.FontDataSize);
				ImGui::Text("Font size in pixels:%f", cfg_data.SizePixels);
				if (font->ConfigData &&!is_current_font&& ImGui::SmallButton("Set as default"))
				{
					ImGui::GetIO().FontDefault = font;
				}
				ImGui::TreePop();
			}
		}
		/*if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
		{
			ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
			ImGui::TreePop();
		}
		ImGui::PushItemWidth(100);
		for (int i = 0; i < atlas->Fonts.Size; i++)
		{
			ImFont* font = atlas->Fonts[i];
			ImGui::PushID(font);
			bool font_details_opened = ImGui::TreeNode(font, "Font %d: \'%s\', %.2f px, %d glyphs", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size);
			ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) ImGui::GetIO().FontDefault = font;
			if (font_details_opened)
			{
				ImGui::PushFont(font);
				ImGui::Text("The quick brown fox jumps over the lazy dog");
				ImGui::PopFont();
				ImGui::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
				ImGui::InputFloat("Font offset", &font->DisplayOffset.y, 1, 1, 0);
				ImGui::SameLine(); ShowHelpMarker("Note than the default embedded font is NOT meant to be scaled.\n\nFont are currently rendered into bitmaps at a given size at the time of building the atlas. You may oversample them to get some flexibility with scaling. You can also render at multiple sizes and select which one to use at runtime.\n\n(Glimmer of hope: the atlas system should hopefully be rewritten in the future to make scaling more natural and automatic.)");
				ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
				ImGui::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
				ImGui::Text("Texture surface: %d pixels (approx) ~ %dx%d", font->MetricsTotalSurface, (int)sqrtf((float)font->MetricsTotalSurface), (int)sqrtf((float)font->MetricsTotalSurface));
				for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
					if (ImFontConfig* cfg = &font->ConfigData[config_i])
						ImGui::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
				if (ImGui::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
				{
					// Display all glyphs of the fonts in separate pages of 256 characters
					for (int base = 0; base < 0x10000; base += 256)
					{
						int count = 0;
						for (int n = 0; n < 256; n++)
							count += font->FindGlyphNoFallback((ImWchar)(base + n)) ? 1 : 0;
						if (count > 0 && ImGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
						{
							float cell_size = font->FontSize * 1;
							float cell_spacing = style.ItemSpacing.y;
							ImVec2 base_pos = ImGui::GetCursorScreenPos();
							ImDrawList* draw_list = ImGui::GetWindowDrawList();
							for (int n = 0; n < 256; n++)
							{
								ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
								ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
								const ImFontGlyph* glyph = font->FindGlyphNoFallback((ImWchar)(base + n));
								draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
								font->RenderChar(draw_list, cell_size, cell_p1, ImGui::GetColorU32(ImGuiCol_Text), (ImWchar)(base + n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
								if (glyph && ImGui::IsMouseHoveringRect(cell_p1, cell_p2))
								{
									ImGui::BeginTooltip();
									ImGui::Text("Codepoint: U+%04X", base + n);
									ImGui::Separator();
									ImGui::Text("AdvanceX: %.1f", glyph->AdvanceX);
									ImGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
									ImGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
									ImGui::EndTooltip();
								}
							}
							ImGui::Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		static float window_scale = 1.0f;
		ImGui::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.1f");              // scale only this window
		ImGui::DragFloat("global scale", &ImGui::GetIO().FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.1f"); // scale everything
		ImGui::PopItemWidth();
		ImGui::SetWindowFontScale(window_scale);*/
		ImGui::TreePop();
	}
}

