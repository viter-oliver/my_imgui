#include "af_state_manager.h"

mp_state_manager g_mstate_manager;

void state_trans_player::play_state_trans(ps_state_manager pstate, int from, int to)
{
	_psel = pstate;
	_cur_trans_play_key = {from,to};
	_cur_trans_play_start = steady_clock::now();
	_be_playing = true;
}
template< class T> void step_value(void* pfrom,void* pto,char* value_address,double scale)
{
	T* ptfrom = (T*)pfrom;
	T* ptto = (T*)pto;
	T* ptprop = (T*)value_address;
	T tvalue_span = *ptto - *ptfrom;
	T tdelta_value = tvalue_span*scale;
	*ptprop = *ptfrom + tdelta_value;
}
void state_trans_player::keep_state_trans_on()
{
	if (_be_playing)
	{
		assert(_psel);
		auto cur_clk = steady_clock::now();
		auto dur_mills = duration_cast<milliseconds>(cur_clk - _cur_trans_play_start);
		auto& icur_trans = _psel->_mtrans.find(_cur_trans_play_key);
		auto& cur_trans = *icur_trans->second;
		auto delta_tm = dur_mills.count() - cur_trans._start_time;
		if (delta_tm > 0)
		{
			double tm_pt_mill = (double)delta_tm / cur_trans._duration;
			auto& easing_fun = easingFun[cur_trans._easing_func];
			double value_scale = easing_fun(tm_pt_mill);
			if (delta_tm < cur_trans._duration)
			{
				_psel->_mstate = en_state_moving;
			}
			else
			{
				_be_playing = false;
				_psel->_mstate = en_state_pause;
				value_scale = 1.0;
				_psel->_state_idx = _cur_trans_play_key._to;
				if (_psel->_trans_finish)
				{
					_psel->_trans_finish(_cur_trans_play_key._from, _cur_trans_play_key._to);
				}
			}
			auto& prop_list = _psel->_prop_list;
			auto& pp_vl_lst_from = _psel->_prop_value_list[_cur_trans_play_key._from];
			auto& pp_vl_lst_to = _psel->_prop_value_list[_cur_trans_play_key._to];
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
				if (fel._type == "bool"&& value_scale==1.0)
				{
					ppt_addr[0] = ppt_blk_to[0];
				}
				idx++;
			}
		}
	}
}
state_trans_player g_state_trans_player;
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
bool play_trans(string trans_name, int from, int to)
{
	const auto& itrans = g_mstate_manager.find(trans_name);
	if (itrans == g_mstate_manager.end())
	{
		return false;
	}
	g_state_trans_player.play_state_trans(itrans->second, from, to);
	return true;
}