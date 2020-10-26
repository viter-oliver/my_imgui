#pragma once
#include "afg.h"
#include <GLFW/glfw3.h>
#include <string>
#include "screen_image_distortion.h"

namespace auto_future
{

	class AFG_EXPORT application
	{
	protected:
#define  SCREEN_W 800
#define  SCREEN_H 534
		string _cureent_project_file_path;
          GLFWwindow* _window = { NULL };
		vector<string> _arg_list;
		int _screen_width = { SCREEN_W }, _screen_height = { SCREEN_H };
		float _win_width = { SCREEN_W }, _win_height = { SCREEN_H };
		base_ui_component* _proot = { NULL };
		float _wposx=0.f, _wposy=0.f;
          shared_ptr<screen_image_distortion> _pscr_ds;
	public:
		application(int argc, char **argv);
		~application();
		
		bool prepare();
		//virtual void init_ui_component(base_ui_component* pobj){}
		virtual void resLoaded() = 0;
		virtual bool create_run();
		//virtual void preRender(){}
		virtual void onUpdate() = 0;
		void destroy();
		void set_windows_pos(float posx, float posy)
		{
			_wposx = posx;
			_wposy = posy;
		}
		void set_window_size(float w_width, float w_height)
		{
			_win_width = w_width;
			_win_height = w_height;
		}
          void set_rotate_angle( float angle );
          void set_rotate_axis_pos( float px, float py );
	};
}

