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
			string strimg_file = strFileName;
			string img_file_name = strimg_file.substr(strimg_file.find_last_of('\\') + 1);
			string img_file_path = strimg_file.substr(0,strimg_file.find_last_of('\\') + 1);

			string extname = strimg_file.substr(strimg_file.find_last_of('.') + 1);
			GLubyte* imgdata = NULL;
			int width, height,channels;
			if (extname=="hdr")
			{
			}
			imgdata = SOIL_load_image(strimg_file.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
			if (imgdata == NULL)
			{
				printf("Fail to load texture file:%s\n", strFileName);
				return ;
			}
			extern string g_cureent_project_file_path;
			string str_img_path = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
			str_img_path += "images\\";
			if (img_file_path != str_img_path)
			{
				string str_cmd = "copy ";
				str_cmd += strimg_file;
				str_cmd += " ";
				str_cmd += str_img_path;
				system(str_cmd.c_str());

			}
			GLuint textureId = 0;
			glGenTextures(1, &textureId);
			glBindTexture(GL_TEXTURE_2D, textureId);
			if (channels == SOIL_LOAD_RGBA)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			// Step2 设定wrap参数
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// Step3 设定filter参数
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_LINEAR); // 为MipMap设定filter方法
			// Step4 加载纹理
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
				0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);
			glGenerateMipmap(GL_TEXTURE_2D);
			// Step5 释放纹理图片资源
			SOIL_free_image_data(imgdata);
			auto pimge = make_shared<af_texture>();
			pimge->_txt_id = textureId;
			pimge->_width = width;
			pimge->_height = height;
			g_mtexture_list[img_file_name] = pimge;

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
		float imw = _ptexture->_width, imh = _ptexture->_height;
		if (imw > 400.f)
		{
			imh = 400*imh / imw;
			imw = 400;
			
		}
		ImGui::Image((ImTextureID)_ptexture->_txt_id, ImVec2(imw,imh), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));

	}
}
