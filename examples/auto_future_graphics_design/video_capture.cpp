
#include "video_capture.h"
#include "common_functions.h"
#include <stdio.h>
#include <chrono>
#include <shlwapi.h>

//#include <semaphore.h>


void check_dev_lost(wchar_t* systemlink)
{
     wstring wlink = wtoLower( systemlink );
	for (auto& it = g_map_dev_units.begin(); it !=g_map_dev_units.end();)
	{
          string dev_name = it->first;
          auto& dv_unit = *it->second;
          if( dv_unit.unlink_self( (wchar_t*)wlink.c_str() ) )
          {
               dv_unit.delink_cltrl_list();
               it = g_map_dev_units.erase( it );
               return;
          }
		else
		{
			it++;
		}
	}
}




sdev_unit _vdev_sel = nullptr;
#include "user_control_imgui.h"
void usb_device_state(int state, wchar_t* dev_name)
{
	printf("usb state=%d\n",state);
	if (state)
	{
          this_thread::sleep_for( chrono::milliseconds( 200 ) );
		EnumDeviceVideoDevices();
	}
	else
	{
		check_dev_lost(dev_name);
		_vdev_sel = nullptr;
	}
}
bool video_link(ft_video* pfv,int& vwidth, int& vheight, char* dev_name)
{
	auto& it_vdev = g_map_dev_units.find(dev_name);
	if (it_vdev!=g_map_dev_units.end())
	{
          return it_vdev->second->add_video_ctrl( pfv, vwidth, vheight );
	}
	return false;
}

void video_capture_excute()
{
	for (auto& it_dev_unit : g_map_dev_units)
	{
		auto& dev_unit = it_dev_unit.second;
		dev_unit->pick_frame_data();
	}
}


void video_dev_view(void)
{
	string icon_str = "ft_video";
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;
	if (IconTreeNode(icon_str, "Video devices", node_flags_root))
	{
		for (auto& it_vdev : g_map_dev_units)
		{
			auto& keyname = it_vdev.first;
			auto& ps_prp_pos = it_vdev.second;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (_vdev_sel&&ps_prp_pos ==_vdev_sel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, keyname.c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				_vdev_sel = ps_prp_pos;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void video_item_edit(void)
{
	if (_vdev_sel)
	{
          _vdev_sel->view_edit();
	}
}
