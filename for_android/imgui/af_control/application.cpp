#include "application.h"

#include "imgui.h"
#include "imgui_impl_gl3.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
//#include <GL/gl3w.h>
#include <chrono>
#include <thread>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "SOIL.h"
//#include "texture.h"
#include "res_output.h"
#include <functional>

//#include "Resource.h"
#include "afb_load.h"
#include "af_state_manager.h"
#include "af_primitive_object.h"

using namespace chrono;


namespace zl_future
{
	application::application()
	{
		ImGui::CreateContext();
		
        ImGui_ImplGL3_CreateDeviceObjects();
		
		ImGui::StyleColorsClassic();
		
	}


	application::~application()
	{
		
	}
	application& application::set_windows_pos(float posx, float posy)
	{
		_wposx = posx;
		_wposy = posy;
		return *this;
	}

    application& application::set_window_size(float w,float h)
	{
		set_display(w,h);
		_win_width = w;
		_win_height = h;
		return *this;
	}
    bool application::load_afb(const char* afb_file)
	{
		if (_proot)
		{
			delete _proot;
		}
		afb_load afl(_proot);
		afl.load_afb(afb_file);
		resLoaded();
		return true;
	}

    bool application::loaf_afb_from_buff(const char* buff ,unsigned int len)
    {
        if (_proot)
        {
            delete _proot;
        }

        afb_load afl(_proot);
        afl.load_afb_from_buff(buff,len);
		LOGI("afb finish:%d\n",__LINE__);
         resLoaded();
        LOGI("afb finish:%d\n",__LINE__);
		return true;
    }
	bool application::render()
	{

		ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.00f);
		LOGI("render:%d\n",__LINE__);	
		ImGui_ImplGL3_NewFrame();
		ImGui::SetNextWindowSize(ImVec2(_win_width, _win_height), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(_wposx, _wposy));
		ImGui::SetNextWindowBgAlpha(1.f);
		static bool show_app = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::Begin("edit window", &show_app, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove 
			| ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoCollapse);
		//
		if (_proot)
		{
			LOGI("render:%d\n",__LINE__);
			onUpdate();
			LOGI("render:%d\n",__LINE__);
			keep_state_trans_on();
			LOGI("render:%d\n",__LINE__);
			execute_lazy_value();
			LOGI("render:%d\n",__LINE__);
			_proot->draw_frames();
			LOGI("render:%d\n",__LINE__);
			
		}
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		// Rendering
		int scw=_win_width,sch=_win_height;
		glViewport(0, 0, scw, sch);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();
		//_pscr_ds->bind_framebuffer();
		ImGui_ImplGL3_RenderDrawData(ImGui::GetDrawData());
		//_pscr_ds->disbind_framebuffer();
		//_pscr_ds->draw();
		//glfwSwapBuffers(_window);
		return true;
	}

	void application::set_image_height(int height)
	{
		//_pscr_ds->set_height(height);
	}

	void application::set_rotate_angle( float angle )
	{
		// _pscr_ds->set_rotate_angle( angle );
	}
	void application::set_rotate_axis_pos( float px, float py )
	{
		// _pscr_ds->set_rotate_axis_pos( px, py );
	}

	void application::destroy()
	{
		 ImGui_ImplGL3_Shutdown();
		ImGui::DestroyContext();
	}
}
