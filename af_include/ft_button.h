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
		DEF_STRUCT_WITH_INIT(intl_pt, _pt,
			(af_vi2, _image_normal_txt),
               ( af_vi2, _image_press_txt ),
               ( af_vi2, _image_select_txt ) )
		state _state = { en_normal };
	public:

		ft_button();
		//ft_button(int tid) :base_ui_component(tid){}
		void draw();
          void mouse_down()
          {
               _state = en_pressed;
          }
          void mouse_relese()
          {
               _state = en_normal;
          }
		void set_state(state st)
		{
			_state = st;
		}
		state get_state()
		{
			return _state;
		}

          af_vi2 get_cur_txt_id()
		{
			switch (_state)
			{
			case auto_future::ft_button::en_normal:
				return _pt._image_normal_txt;
			case auto_future::ft_button::en_pressed:
				return _pt._image_press_txt;
			case auto_future::ft_button::en_selected:
				return _pt._image_select_txt;
			}
               return {0,0};
		}


		bool handle_mouse();
	};
	REGISTER_CONTROL(ft_button)
}