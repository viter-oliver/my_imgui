#include "af_playlist_group.h"
#include "af_state_manager.h"
playlist_group_list g_playlist_group_list;
bool play_playlist_group(string gp_name)
{
	const auto& ipg = g_playlist_group_list.find(gp_name);
	if (ipg == g_playlist_group_list.end())
	{
		return false;
	}
	auto& tar_playlist_group_list = *ipg->second;
	for (auto playlist_group : tar_playlist_group_list)
	{
		if (trans_is_playing(playlist_group._st_name))
		{
			printf("trans_name:%s is playing for playlist_group:%s\n", playlist_group._st_name.c_str(), gp_name.c_str());
			return false;
		}
	}
	for (auto playlist_group : tar_playlist_group_list)
	{
		play_tran_playlist(playlist_group._st_name, playlist_group._playlist_id);
	}
	return true;
}