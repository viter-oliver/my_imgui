#include "application.h"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#if defined(IMGUI_WAYLAND)
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#endif
//#include <GL/gl3w.h>

#include <GLFW/glfw3.h>
#include "SOIL.h"
//#include "texture.h"
#include "res_output.h"
#include "ft_image.h"
#include <functional>

//#include "Resource.h"
#include "afb_load.h"
#include "primitive_object.h"
#include <chrono>
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

	bool application::prepare()
	{
		glfwSetErrorCallback(error_callback);
		if (!glfwInit())
			return false;

              glfwWindowHint(GLFW_CLIENT_API,GLFW_OPENGL_ES_API);
		#ifdef GLFW_INCLUDE_ES2
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
              #else
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		#endif
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
		 
              #if defined(_GLFW_SCREEN)
		for(auto& arg:_arg_list)
		{
			if(strncmp(arg.c_str(), "-zorder=", strlen("-zorder=")) == 0)
			{
				string str_zorder=arg.substr(sizeof("-zorder="));
				int zorder=atoi(str_zorder.c_str());
				glfwWindowHint(GLFW_SCREEN_WINDOW_ZORDER,zorder);
			}
			else
			if(strncmp(arg.c_str(), "-display=", strlen("-display=")) == 0){
				string str_display=arg.substr(sizeof("-display="));
				glfwWindowHintString(GLFW_SCREEN_DISPLAY_NAME,str_display.c_str());
			}
		}
	       #endif
		if (!_screen_width || !_screen_height)
		{
			GLFWmonitor*  pmornitor = glfwGetPrimaryMonitor();
			const GLFWvidmode * mode = glfwGetVideoMode(pmornitor);
			_screen_width = mode->width;
			_screen_height = mode->height;
		}
		//instantiating_internal_shader();
		
		//ImVec2 edit_window_size = ImVec2()
		

		return true;
	}

	bool application::create_run()
	{
		GLFWwindow* _window = { NULL };
		_window = glfwCreateWindow(_screen_width, _screen_height, "Graphics app", NULL, NULL);

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
		ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.00f); //ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		//base_ui_component* _proot = NULL;
		if (!_cureent_project_file_path.empty())
		{
			//_proot = new ft_base;
			auto currentTime = std::chrono::high_resolution_clock::now();
			afb_load afl(_proot);
			afl.load_afb(_cureent_project_file_path.c_str());
			auto afterLoadTime = std::chrono::high_resolution_clock::now();
			auto tmspan = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(afterLoadTime-currentTime).count();
			printf("afb load consume %d milli seconds\n", tmspan);
			resLoaded();
		}
                init_internal_primitive_list();
	// Setup style
		//ImGui::StyleColorsLight();
		ImGui::StyleColorsDark();
		while (!glfwWindowShouldClose(_window))
		{
			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			glfwPollEvents();
			ImGui_ImplGlfwGL3_NewFrame();
			ImGui::SetNextWindowSize(ImVec2(1920, 720), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(1.f);
			static bool show_app = true;
			ImGuiStyle& style = ImGui::GetStyle();
			style.Alpha = 1.f;
			style.WindowRounding = 0.f;
			ImGui::Begin("edit window", &show_app, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			//
			onUpdate();
			if (_proot)
			{
				_proot->draw();
			}
			ImGui::End();
			// Rendering
			int display_w, display_h;
			glfwGetFramebufferSize(_window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(_window);
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
