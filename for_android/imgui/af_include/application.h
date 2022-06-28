#pragma once
#include "afg.h"
#include <string>
#include "screen_image_distortion.h"

namespace zl_future
{

	class AFG_EXPORT application
	{
		const float SCREEN_W=800.0,SCREEN_H=480.0;
	protected:
		float _win_width = { SCREEN_W }, _win_height = { SCREEN_H };
		int monitor_id={0};
		base_ui_component* _proot = { nullptr };
		float _wposx=0.f, _wposy=0.f;
		shared_ptr<screen_image_distortion> _pscr_ds;
	public:
		application();
		~application();
	    bool load_afb(const char* afb_file);
	    bool loaf_afb_from_buff(const char* buff ,unsigned int len);
		//virtual void init_ui_component(base_ui_component* pobj){}
		virtual void resLoaded() = 0;
		virtual bool render();
		//virtual void preRender(){}
		virtual void onUpdate() = 0;
		void destroy();
		application& set_windows_pos(float posx, float posy);
		application&  set_window_size(float w_width, float w_height);
		void set_image_height(int height);
		void set_rotate_angle( float angle );
        void set_rotate_axis_pos( float px, float py );
	};
}

