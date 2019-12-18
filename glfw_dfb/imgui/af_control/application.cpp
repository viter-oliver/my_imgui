#include "application.h"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
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
#include <GLFW/glfw3.h>
#include "SOIL.h"
//#include "texture.h"
#include "res_output.h"
#include <functional>

//#include "Resource.h"
#include "afb_load.h"
#include "af_state_manager.h"
#include "af_primitive_object.h"
using namespace chrono;
//extern void instantiating_internal_shader();
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

namespace auto_future
{
	application::application(int argc, char **argv)
	{
		if (argc > 1)
		{
			_cureent_project_file_path = argv[1];//afb
		}
		for (int ix = 2; ix < argc;ix++)
		{
			_arg_list.emplace_back(argv[ix]);
		}
	}


	application::~application()
	{
	}

int get_resolution(int* width,int* height)
{
	struct fb_var_screeninfo sc_info;
	int fd=open("/dev/fb0",O_RDWR);
	ioctl(fd,FBIOGET_VSCREENINFO,&sc_info);
	printf("%d*%d\n",sc_info.xres,sc_info.yres);
	*width=sc_info.xres;
	*height=sc_info.yres;
	close(fd);
	return 0;
}
	bool application::prepare()
	{
		glfwSetErrorCallback(error_callback);
		if (!glfwInit())
			return false;
        glfwWindowHint(GLFW_CLIENT_API,GLFW_OPENGL_ES_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#if __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		if (!_screen_width || !_screen_height)
		{
			//GLFWmonitor*  pmornitor = glfwGetPrimaryMonitor();
			//const GLFWvidmode * mode = glfwGetVideoMode(pmornitor);
			//_screen_width = mode->width;
			//_screen_height = mode->height;
                      get_resolution(&_screen_width,&_screen_height);
		}
		//instantiating_internal_shader();

		//ImVec2 edit_window_size = ImVec2()


		return true;
	}

	bool application::create_run()
	{
		GLFWwindow* _window = { NULL };
#if !defined(IMGUI_WAYLAND)
		_window = glfwCreateWindow(_screen_width, _screen_height, "Graphics app", NULL, NULL);
#else
		_window = glfwCreateWindow(_screen_width, _screen_height, "Graphics app", glfwGetPrimaryMonitor(), NULL);
#endif
              //glfwSetWindowPos(_window,100,100);

		glfwMakeContextCurrent(_window);

		glfwSwapInterval(1); // Enable vsync

		//gl3wInit();

		// Setup ImGui binding
		ImGui::CreateContext();
		//ImGuiIO& io = ImGui::GetIO(); //(void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

              ImGui_ImplGlfwGL3_Init(_window, true);
		//ImGui::StyleColorsClassic();
		ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.00f);

		//base_ui_component* _proot = NULL;
		if (!_cureent_project_file_path.empty())
		{
			//_proot = new ft_base;
			afb_load afl(_proot);
			afl.load_afb(_cureent_project_file_path.c_str());
			resLoaded();
		}
		int max_tex_size=0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max_tex_size);
		printf("max texture size=%d\n",max_tex_size);
		//init_internal_primitive_list();
	// Setup style
		//ImGui::StyleColorsLight();
		ImGui::StyleColorsClassic();
		int tar_fps=60;
		int iper_sc=1000.f/tar_fps;
		steady_clock::time_point  t_bf_render,t_af_render;
		while (!glfwWindowShouldClose(_window))
		{
			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			//glfwPollEvents();
			t_bf_render=steady_clock::now();
			ImGui_ImplGlfwGL3_NewFrame();
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
				onUpdate();
				g_state_trans_player.keep_state_trans_on();
                    		_proot->draw_frames();
			}
			ImGui::End();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			// Rendering
			int display_w, display_h;
			glfwGetFramebufferSize(_window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(_window);
			t_af_render=steady_clock::now();
			int delta = chrono::duration_cast<chrono::duration<int,std::milli>>(t_af_render - t_bf_render).count();
			if(delta<iper_sc)
			{
			    auto sp_pd=iper_sc-delta-10;
			    this_thread::sleep_for(chrono::milliseconds(sp_pd));
			}
			
		}
		return true;
	}


	void application::destroy()
	{
		ImGui_ImplGlfwGL3_Shutdown();
		ImGui::DestroyContext();
		glfwTerminate();
	}
}
