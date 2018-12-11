#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_button :
		public ft_base
	{
	public:
		enum state
		{
			en_normal,
			en_pressed,
			en_selected,
			en_state_cnt
		};
	private:
		DEF_STRUCT(intl_pt,_pt,
			(float,_sizew),
            (float, _sizeh),
			(int, _texture_indices_nm_tt),
			(int, _texture_indices_dn_tt),
			(int, _texture_indices_sl_tt))
		state _state = { en_normal };

	public:

		ft_button();
		//ft_button(int tid) :base_ui_component(tid){}
		void draw();
	
		void set_state(state st)
		{
			_state = st;
		}
		state get_state()
		{
			return _state;
		}
		int get_txt_id_by_state(state st)
		{
			return *(&_pt._texture_indices_nm_tt+st);
		}
		int get_cur_txt_id()
		{
			return get_txt_id_by_state(_state);
		}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif

		bool handle_mouse();
	};
	REGISTER_CONTROL(ft_button)
}