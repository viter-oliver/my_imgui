#pragma once
#include "afg.h"
#include <string>

namespace auto_future
{
	
	class AFG_EXPORT application
	{
	protected:
		std::string _cureent_project_file_path;
		int _screen_width = { 0 }, _screen_height = { 0 };
		base_ui_component* _proot = { NULL };
		
	public:
		application(int argc, char **argv);
		~application();
		bool prepare();
		virtual void resLoaded(){}
		bool create_run();
		virtual void preRender(){}
		virtual void onUpdate(){}
		void destroy();
	};
}

