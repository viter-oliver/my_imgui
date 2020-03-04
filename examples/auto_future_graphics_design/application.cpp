#include "application.h"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#else
#include "../../deps/glad/glad.h"
#endif

#include "SOIL.h"
#include "texture.h"
#include "res_output.h"
#include "ft_image.h"
#include <functional>

#include "Resource.h"
#include "afb_load.h"
#include "af_state_manager.h"
#include "af_primitive_object.h"
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
		
		/*if (argc > 3)
		{
			_screen_width = atoi(argv[2]);
			_screen_height = atoi(argv[3]);
		}*/
	}


	application::~application()
	{
	}

	bool application::prepare()
	{
		glfwSetErrorCallback(error_callback);
		if (!glfwInit())
			return false;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		/*glfwWindowHint(GLFW_RED_BITS, 0);
		glfwWindowHint(GLFW_GREEN_BITS, 0);
		glfwWindowHint(GLFW_BLUE_BITS, 0);*/

#if __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
		
#if !defined(IMGUI_WAYLAND)
		_window = glfwCreateWindow(_screen_width, _screen_height, "Graphics app", NULL, NULL);
#else
		_window = glfwCreateWindow(_screen_width, _screen_height, "Graphics app", glfwGetPrimaryMonitor(), NULL);
#endif
		glfwMakeContextCurrent(_window);
		glfwSwapInterval(1); // Enable vsync
		gl3wInit();

		// Setup ImGui binding
		ImGui::CreateContext();
		//ImGuiIO& io = ImGui::GetIO(); //(void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
		ImGui_ImplGlfwGL3_Init(_window, true);
		
		ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.00f); //ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		//base_ui_component* _proot = NULL;
		if (!_cureent_project_file_path.empty())
		{
			//_proot = new ft_base;
			auto currentTime = std::chrono::high_resolution_clock::now();
			afb_load afl(_proot);
			//std::bind(&play_back_node::btnclk_##x, this, std::placeholders::_1)
			//afl.set_impl(std::bind(&application::init_ui_component, this, std::placeholders::_1));
			afl.load_afb(_cureent_project_file_path.c_str());
			auto afterLoadTime = std::chrono::high_resolution_clock::now();
			auto tmspan = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(afterLoadTime-currentTime).count();
			printf("afb load consume %d milli seconds\n", tmspan);
			resLoaded();
		}
		//init_internal_primitive_list();
		// Setup style
		//ImGui::StyleColorsLight();
		ImGui::StyleColorsDark();
#if 0
		printf("externsions=%s\n", glGetString(GL_EXTENSIONS));
		GLint n;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);
		if (n > 0)
		{
			GLint i;
			for (i = 0; i < n; i++)
			{
				printf("%s\n", glGetStringi(GL_EXTENSIONS, i));
			}
		}
#endif
		while (!glfwWindowShouldClose(_window))
		{
			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			glfwPollEvents();
			ImGui_ImplGlfwGL3_NewFrame();
			ImGui::SetNextWindowSize(ImVec2(_win_width, _win_height), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowPos(ImVec2(_wposx, _wposy));
			ImGui::SetNextWindowBgAlpha(1.f);
			static bool show_app = true;
			ImGuiStyle& style = ImGui::GetStyle();
			style.Alpha = 1.f;
			style.WindowRounding = 0.f;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
			ImGui::Begin("edit window", &show_app, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			//
			if (_proot)
			{
				onUpdate();
				keep_state_trans_on();
                    _proot->draw_frames();
			}
               ImGuiContext& g = *GImGui;
               ImGuiWindow* cur_window = ImGui::GetCurrentWindow();
               ImGuiWindow* front_window = g.Windows.back();
               ImRect wrect( cur_window->Pos, cur_window->Pos + cur_window->Size );
               if( cur_window == front_window && wrect.Contains( ImGui::GetIO().MousePos ) )
               {
                    //printf("mouse_click_pos(%f,%f)\n", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
                    base_ui_component* psel_ui = _proot->get_hit_ui_object( ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y );
                    if( psel_ui )
                    {
                         if( ImGui::IsMouseClicked( 0 ) )
                         {
                              psel_ui->trigger_click();
                              psel_ui->mouse_clicked();
                         }
                         if( ImGui::IsMouseDown( 0 ) )
                         {
                              psel_ui->trigger_mouse_down();
                              psel_ui->mouse_down();
                         }
                         if( ImGui::IsMouseReleased( 0 ) )
                         {
                              psel_ui->trigger_mouse_release();
                              psel_ui->mouse_relese();
                         }
                         if( ImGui::IsMouseDragging( 0 ) )
                         {
                              auto ms_delta = ImGui::GetIO().MouseDelta;
                              psel_ui->trigger_mouse_drag( ms_delta.x, ms_delta.y );
                              psel_ui->mouse_drag( ms_delta.x, ms_delta.y );
                         }
                    }
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