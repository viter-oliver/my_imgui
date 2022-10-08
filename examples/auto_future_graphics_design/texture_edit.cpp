#include "texture_edit.h"
#include <string>
#include "user_control_imgui.h"
#include <windows.h>
#include <locale.h>  
#include <ShlObj.h>
#include <Commdlg.h>
#include "resource.h"
#include "res_internal.h"
#include <WinUser.h>
#include <mmsystem.h>
#ifdef IMGUI_WAYLAND
#include "../../deps/glad/glad.h"
#else
#include <GL/gl3w.h> 
#endif
#include "SOIL.h"
extern void store_to_clipboard(string& str_content);
static BOOL isChked;
UINT CALLBACK ofnHookProc(HWND hDlg, UINT uMsg, UINT wParam, LONG lParam)
{
	OFNOTIFY* pofNotify;
	switch (uMsg)
	{
	case WM_NOTIFY:
		pofNotify = (OFNOTIFY*)lParam;
		switch (pofNotify->hdr.code)
		{
		case CDN_FILEOK:
			isChked = IsDlgButtonChecked(hDlg, IDC_CHK_MIPMAP);
			
		}
	default:
		break;
	}
	return FALSE;
}
shared_ptr<af_texture> _ptexture{ nullptr };
string _key_name;
void texture_edit::clear_states()
{
     _ptexture = nullptr;
}
void texture_edit::draw_texture_list()
{
	if (ImGui::Button("Load new image..."))
	{
		OPENFILENAME ofn = { sizeof(OPENFILENAME) };
		//ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = GetForegroundWindow();
		ofn.lpstrFilter = "image file:\0*.png;*.bmp;*.jpg;*.jpeg;*.gif;*.dds;*.tga;*.psd;*.hdr\0\0";
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY|OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;
		ofn.lpTemplateName = MAKEINTRESOURCE(IDD_DLG_MIPMAP);
		ofn.hInstance = GetModuleHandle(NULL);
		ofn.lpfnHook = ofnHookProc;
		char strFileName[MAX_PATH] = { 0 };
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = strFileName;
		ofn.nMaxFile = sizeof(strFileName);
		ofn.lpstrTitle = "Loading image...";
		if (GetOpenFileName(&ofn))
		{
			if (isChked == BST_CHECKED)
			{
				printf("is mipmap!\n");
			}
			else
			{
				printf("is not mipmap\n");
			}
			add_image_to_mtexure_list(string(strFileName), isChked == BST_CHECKED);
		}
	}
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = icn_nm_texture;
	if (IconTreeNode(icon_str, "texture list", node_flags_root))
	{
		for (auto& mtt_ut : g_mtexture_list)
		{
			//icon_str =icn_nm_image;
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
				_key_name = keyname;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	if (_ptexture&&ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("delete",NULL,false,_ptexture.use_count()==2))
		{
			auto& item_del = g_mtexture_list.find(_key_name);
			g_mtexture_list.erase(item_del);
			_ptexture = nullptr;
			_key_name = "";
		}
		if (ImGui::MenuItem("copy name", "CTRL+C", false)){
			store_to_clipboard(_key_name);
		}
		ImGui::EndPopup();
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
		ImGui::Text("Size:%u,%u", _ptexture->_width, _ptexture->_height);
		ImGui::Checkbox("mipmap", &_ptexture->_mip_map);
		ImGui::Image((ImTextureID)_ptexture->_txt_id(), ImVec2(imw,imh), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));

	}
}
std::string texture_edit::get_cur_key(){
	return _key_name;
}
