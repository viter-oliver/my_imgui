#pragma once
#include "afg.h"
#include <string>

namespace auto_future
{

	class AFG_EXPORT application
	{
	protected:
#define  SCREEN_W 1920
#define  SCREEN_H 720
		string _cureent_project_file_path;
		vector<string> _arg_list;
		int _screen_width = { SCREEN_W }, _screen_height = { SCREEN_H };
		base_ui_component* _proot = { NULL };
		
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
	};
}

