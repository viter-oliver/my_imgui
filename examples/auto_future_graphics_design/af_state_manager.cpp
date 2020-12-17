#include "af_state_manager.h"

mp_state_manager g_mstate_manager;
/**
void state_trans_player::play_state_trans(ps_state_manager pstate, int from, int to)
{
	_psel = pstate;
	_cur_trans_play_key = {from,to};
	_cur_trans_play_start = steady_clock::now();
	_be_playing = true;
}*/
static char* str_play_state[en_play_state_cnt] =
{
	"stopped",
	"playing a trans",
	"playing a playlist"
};
AFG_EXPORT bool trans_is_playing( string trans_name )
{
     const auto& istm = g_mstate_manager.find( trans_name );
     if( istm == g_mstate_manager.end() )
     {
          printf( "invalid state manager name:%s\n", trans_name.c_str() );
          return false;
     }
     auto& stm = *istm->second;
     if( stm._play_state != en_play_stop )
     {
          printf( "state manager:%s is running in state:%s\n", trans_name.c_str(), str_play_state[ stm._play_state ] );
          return true;
     }
     return false;
}
AFG_EXPORT bool play_tran(string stm_name, int from, int to)
{
	const auto& istm = g_mstate_manager.find(stm_name);
	if (istm == g_mstate_manager.end())
	{
		printf("invalid state manager name:%s\n", stm_name.c_str());
		return false;
	}
	auto& stm = *istm->second;
	if (stm._play_state!=en_play_stop)
	{
		printf("state manager:%s is running in state:%s\n", stm_name.c_str(), str_play_state[stm._play_state]);
		return false;
	}
	const auto& itran = stm._mtrans.find({ from, to });
	if (itran == stm._mtrans.end())
	{
		return false;
	}
	stm._pcur_tran = itran->second;
	stm._trans_start = steady_clock::now();
	stm._play_state = en_play_tran;
	stm._cur_from = from;
	stm._cur_to = to;
	return true;
}
AFG_EXPORT bool play_tran_playlist(string stm_name, int playlist_id)
{
	const auto& istm = g_mstate_manager.find(stm_name);
	if (istm == g_mstate_manager.end())
	{
		printf("invalid state manager name:%s\n", stm_name.c_str());
		return false;
	}
	auto& stm = *istm->second;
	if (stm._play_state != en_play_stop)
	{
		printf("state manager:%s is running in state:%s\n", stm_name.c_str(), str_play_state[stm._play_state]);
		return false;
	}
	if (playlist_id>=stm._playlist_list.size())
	{
		printf("for state manager:%s,%d is invalid\n", stm_name.c_str(), playlist_id);
		return false;
	}
	stm._cur_playlist_id = playlist_id;
	stm._play_state = en_play_tran_playlist;
	stm._trans_start = steady_clock::now();
	stm._cur_play_trans_id = 0;
	return true;
}

template< class T> void step_value( void* pfrom, void* pto, char* value_address, double scale )
{
     auto sizeT = sizeof( T );
     T from, to, tprop;
     memcpy( &from, pfrom, sizeT );
     memcpy( &to, pto, sizeT );
     T valueSp = to - from;
     tprop = from + (valueSp)*scale;
     memcpy( value_address, &tprop, sizeT );
}

void keep_state_trans_on()
{
     for( auto& istm : g_mstate_manager )
     {
		auto& stm = *istm.second;
		if (!stm._play_state)
		{
			continue;
		}
		if (stm._play_state==en_play_tran)
		{
			if (!stm._pcur_tran)
			{
				continue;
			}
			assert(stm._cur_from != stm._cur_to&&"invalid from or to");
			auto cur_clk = steady_clock::now();
			auto& play_clk = stm._trans_start;
			auto dur_mills = duration_cast<milliseconds>(cur_clk - play_clk);
			auto& cur_trans = *stm._pcur_tran;
			auto delta_tm = dur_mills.count() - cur_trans._start_time;//note㏒ostart_time2⊿2?那?那㊣??米?㏒???那??角谷迄o芍??o車?a那?
			if (delta_tm>0)
			{
				double tm_pt_mill = (double)delta_tm / cur_trans._duration;
				auto& easing_fun = easingFun[cur_trans._easing_func];
				double value_scale = easing_fun(tm_pt_mill);
				if (delta_tm>= cur_trans._duration)
				{
					stm._play_state=en_play_stop;
					value_scale = 1.0;
					if (stm._trans_finish)
					{
						stm._trans_finish(stm._cur_from, stm._cur_to);
					}
				}
				auto& prop_list = stm._prop_list;
				auto& pp_vl_lst_from = stm._prop_value_list[stm._cur_from];
				auto& pp_vl_lst_to = stm._prop_value_list[stm._cur_to];
				int idx = 0;
				for (auto& prop : prop_list)
				{
					auto& pgidx = prop._page_index;
					auto& fdidx = prop._field_index;
					auto& pobj = prop._pobj;
					auto& fel = pobj->get_filed_ele(pgidx, fdidx);
					char* ppt_addr = fel._address;
					auto& ppt_blk_from = pp_vl_lst_from[idx];
					auto& ppt_blk_to = pp_vl_lst_to[idx];
					if (fel._type == "int")
					{
						step_value<int>(&ppt_blk_from[0], &ppt_blk_to[0], ppt_addr, value_scale);
					}
					else
					if (fel._type == "float")
					{
						step_value<float>(&ppt_blk_from[0], &ppt_blk_to[0], ppt_addr, value_scale);
					}
					else
					if (fel._type == "double")
					{
						step_value<double>(&ppt_blk_from[0], &ppt_blk_to[0], ppt_addr, value_scale);
					}
					else
					if (fel._type == "bool"&& value_scale == 1.0)
					{
						ppt_addr[0] = ppt_blk_to[0];
					}
                         calcu_bind_node( prop );
					idx++;
				}
			}
		}
		else
		{
			assert(stm._cur_playlist_id < stm._playlist_list.size() && "invalid playlist id?");
			auto& playlist = stm._playlist_list[stm._cur_playlist_id];
			auto& cur_pid = stm._cur_play_trans_id;
			assert(cur_pid < playlist.size() && "invalid play id?");
			auto cur_clk = steady_clock::now();
			auto& play_clk = stm._trans_start;
			auto dur_mills = duration_cast<milliseconds>(cur_clk - play_clk);
			auto& cur_trankey = playlist[cur_pid];
			auto& itrans = stm._mtrans.find(cur_trankey);
			assert(itrans != stm._mtrans.end() && "invalid trans key?");
			auto& cur_trans = *itrans->second;
			auto delta_tm = dur_mills.count() - cur_trans._start_time;
			if (delta_tm > 0)
			{
				double tm_pt_mill = (double)delta_tm / cur_trans._duration;
				auto& easing_fun = easingFun[cur_trans._easing_func];
				double value_scale = easing_fun(tm_pt_mill);
				if (delta_tm >= cur_trans._duration)
				{
					value_scale = 1.0;
					auto last_id = playlist.size() - 1;
					if (cur_pid == last_id)
					{
						stm._play_state = en_play_stop;
						cur_pid = 0;
						stm._state_idx = cur_trankey._to;
						if (stm._trans_finish)
						{
							stm._trans_finish(cur_trankey._from, cur_trankey._to);
						}
						//stm._playlist.clear();
					}
					else
					{
						cur_pid++;
						stm._trans_start = steady_clock::now();
						//auto& next_transkey = playlist[ cur_pid ];
					}

				}
				auto& prop_list = stm._prop_list;
				auto& pp_vl_lst_from = stm._prop_value_list[cur_trankey._from];
				auto& pp_vl_lst_to = stm._prop_value_list[cur_trankey._to];
				int idx = 0;
				for (auto& prop : prop_list)
				{
					auto& pgidx = prop._page_index;
					auto& fdidx = prop._field_index;
					auto& pobj = prop._pobj;
					auto& fel = pobj->get_filed_ele(pgidx, fdidx);
					char* ppt_addr = fel._address;
					auto& ppt_blk_from = pp_vl_lst_from[idx];
					auto& ppt_blk_to = pp_vl_lst_to[idx];
					if (fel._type == "int")
					{
						step_value<int>(&ppt_blk_from[0], &ppt_blk_to[0], ppt_addr, value_scale);
					}
					else
					if (fel._type == "float")
					{
						step_value<float>(&ppt_blk_from[0], &ppt_blk_to[0], ppt_addr, value_scale);
					}
					else
					if (fel._type == "double")
					{
						step_value<double>(&ppt_blk_from[0], &ppt_blk_to[0], ppt_addr, value_scale);
					}
					else
					if (fel._type == "bool"&& value_scale == 1.0)
					{
						ppt_addr[0] = ppt_blk_to[0];
					}
                         calcu_bind_node( prop );
					idx++;
				}
			}
		}
     }
}
AFG_EXPORT bool save_property_to_trans_state( string trans_name,
                                              prop_ele_position& prp_pos,
                                              int base_id )
{
     const auto& itrans = g_mstate_manager.find( trans_name );
     if( itrans == g_mstate_manager.end() )
     {
          printf( "invalid trans name:%s\n", trans_name.c_str() );
          return false;
     }
     auto& pobj = prp_pos._pobj;
     auto& pgidx = prp_pos._page_index;
     auto& fidx = prp_pos._field_index;
     field_ele& fel = pobj->get_filed_ele( pgidx, fidx );

     auto& trans = *itrans->second;
     auto& prp_list = trans._prop_list;
     int pos_id = 0;
     for( ; pos_id < prp_list.size(); pos_id++ )
     {
          if( prp_list[ pos_id ] == prp_pos )
          {
               break;
          }
     }
     if( pos_id == prp_list.size() )
     {
          printf( "invalid property element position\n" );
          return false;
     }
     auto& prp_value_list = trans._prop_value_list;
     auto& pp_vl_list_target = prp_value_list[ base_id ];
     auto& value_target = pp_vl_list_target[ pos_id ];
     //T tar_value;
     memcpy( &value_target[ 0 ], fel._address, fel._tpsz );

     return true;
}
AFG_EXPORT bool cancel_play_tran( string stm_name )
{
     const auto& istm = g_mstate_manager.find( stm_name );
     if( istm == g_mstate_manager.end() )
     {
          printf( "invalid state manager name:%s\n", stm_name.c_str() );
          return false;
     }
     auto& stm = *istm->second;
     stm._play_state = en_play_stop;
     return true;

}

bool save_trans_value( string trans_name, int sid )
{
     const auto& itrans = g_mstate_manager.find( trans_name );
     if (itrans==g_mstate_manager.end())
     {
          return false;
     }
     auto& trans = *itrans->second;
     auto& pp_value_list = trans._prop_value_list;
     if (sid>=pp_value_list.size())
     {
          return false;
     }
     auto& cur_pp_value = pp_value_list[ sid ];
     
     auto& pplist = trans._prop_list;
     int iidx = 0;
     for( auto&p_pos : pplist )
     {
          auto& pgidx = p_pos._page_index;
          auto& fidx = p_pos._field_index;
          field_ele &fel = p_pos._pobj->get_filed_ele( pgidx, fidx );
          auto& pp_block = cur_pp_value[ iidx ];
          char* ppt_addr = fel._address;
          int tp_sz = fel._tpsz;
          pp_block.resize( tp_sz );
          memcpy( &pp_block[ 0 ], ppt_addr, tp_sz );
          iidx++;
     }
     return true;
}
bool restore_trans_value( string trans_name, int sid )
{
     const auto& itrans = g_mstate_manager.find( trans_name );
     if( itrans == g_mstate_manager.end() )
     {
          return false;
     }
     auto& trans = *itrans->second;
     auto& pp_value_list = trans._prop_value_list;
     if( sid >= pp_value_list.size() )
     {
          return false;
     }
     auto& cur_pp_value = pp_value_list[ sid ];

     auto& pplist = trans._prop_list;
     int iidx = 0;
     for( auto&p_pos : pplist )
     {
          auto& pgidx = p_pos._page_index;
          auto& fidx = p_pos._field_index;
          field_ele &fel = p_pos._pobj->get_filed_ele( pgidx, fidx );
          auto& pp_block = cur_pp_value[ iidx ];
          char* ppt_addr = fel._address;
          int tp_sz = fel._tpsz;
          pp_block.resize( tp_sz );
          memcpy(ppt_addr, &pp_block[ 0 ], tp_sz );
          iidx++;
     }
     return true;
}
bool reg_trans_handle(string trans_name, trans_finish_handle trans_handle)
{
	const auto& itrans = g_mstate_manager.find(trans_name);
	if (itrans==g_mstate_manager.end())
	{
		return false;
	}
	auto& trans = *itrans->second;
	trans._trans_finish = trans_handle;
	return true;
}

