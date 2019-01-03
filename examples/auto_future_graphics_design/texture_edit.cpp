#include "texture_edit.h"
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
#include "SOIL.h"
shared_ptr<af_texture> _ptexture{ nullptr };
void texture_edit::draw_texture_list()
{
	if (ImGui::Button("Load new image..."))
	{
		OPENFILENAME ofn = { sizeof(OPENFILENAME) };
		ofn.hwndOwner = GetForegroundWindow();
		ofn.lpstrFilter = "image file:\0*.png;*.bmp;*.jpg;*.jpeg;*.gif;*.dds;*.tga;*.psd;*.hdr\0\0";
		char strFileName[MAX_PATH] = { 0 };
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = strFileName;
		ofn.nMaxFile = sizeof(strFileName);
		ofn.lpstrTitle = "Loading image...";
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		if (GetOpenFileName(&ofn))
		{
			add_image_to_mtexure_list(string(strFileName));
		}
	}
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = "texture_list";
	if (IconTreeNode(icon_str, "texture list", node_flags_root))
	{
		for (auto& mtt_ut : g_mtexture_list)
		{
			icon_str = "image";
			auto& keyname = mtt_ut.first;
			auto& ttl = mtt_ut.second;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (ttl->_sel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, keyname.c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				if (_ptexture)
				{
					_ptexture->_sel = false;
				}
				ttl->_sel = true;
				_ptexture = ttl;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}
void texture_edit::draw_texture_item_property()
{
	if (_ptexture)
	{
		ImGui::Checkbox("is separated", &_ptexture->_is_separated);
		float imw = _ptexture->_width, imh = _ptexture->_height;
		if (imw > 400.f)
		{
			imh = 400*imh / imw;
			imw = 400;
			
		}
		ImGui::Image((ImTextureID)_ptexture->_txt_id(), ImVec2(imw,imh), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));

	}
}
