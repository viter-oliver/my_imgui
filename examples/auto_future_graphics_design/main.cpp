// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#else
#include "../../deps/glad/glad.h"
#endif
#include <GLFW/glfw3.h>
#include "SOIL.h"
#include "texture.h"
#include "ft_base.h"
#include "ui_assembler.h"
#include "res_output.h"
#include "ft_image.h"
#include "project_edit.h"
#include "res_internal.h"
#include "texture_res_load.h"
#include <functional>
#if !defined(IMGUI_WAYLAND)
#include <windows.h>
#include <locale.h>  
#include <ShlObj.h>
#include <Commdlg.h>
#include "imguidock.h"
#endif
#include "Resource.h"
#include "res_edit.h"
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}
extern void instantiating_internal_shader();
string g_cureent_project_file_path;
string g_current_run_path;
#include <windows.h>
enum en_short_cut_item
{
	en_ctrl_n,
	en_ctrl_o,
	en_ctrl_s,
	an_alt_f4,
};
int main(int argc, char* argv[])
{
    // Setup window
	if (argc>1)
	{
		g_cureent_project_file_path = argv[1];
	}
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWmonitor*  pmornitor = glfwGetPrimaryMonitor();
	const GLFWvidmode * mode = glfwGetVideoMode(pmornitor);
	int iw, ih;
	iw = mode->width;
	ih = mode->height;

    GLFWwindow* window = glfwCreateWindow(iw, ih, "Auto Future Graphics Designer", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    gl3wInit();

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); //(void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    //ImGui::StyleColorsLight();
    ImGui::StyleColorsClassic();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesChinese());

    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesChinese());
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	io.Fonts->AddFontFromFileTTF("D:\\Qt\\Qt5.6.2\\5.6\\Src\\qtbase\\lib\\fonts\\DejaVuSerif-BoldOblique.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesChinese());

    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
	char buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	g_current_run_path = buffer;
	g_current_run_path += "\\";
	load_internal_texture_res(g_mtxt_intl, IDB_INTERNAL_TXT_RES, IDR_INTERNAL_TXT_FMT);
	instantiating_internal_shader();

    bool show_demo_window = true;
    bool show_another_window = false;
	bool show_edit_window = true;
	//ImVec2 edit_window_size = ImVec2()
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	//g_res_texture_list[0].texture_id = TextureHelper::load2DTexture(g_res_texture_list[0].texture_path, g_res_texture_list[0].texture_width, g_res_texture_list[0].texture_height);
	//g_vres_texture_list[0].texture_id = \
	//	TextureHelper::load2DTexture(g_vres_texture_list[0].texture_path, g_vres_texture_list[0].texture_width, g_vres_texture_list[0].texture_height,\
	//	GL_RGBA,GL_RGBA,SOIL_LOAD_RGBA);
	ft_base* _proot = NULL;	
	base_ui_component* _pselect = NULL;
	res_edit* _pres_mg;
	if (!g_cureent_project_file_path.empty())
	{
		_proot = new ft_base;
		ui_assembler _ui_as(*_proot);
		_ui_as.load_ui_component_from_file(g_cureent_project_file_path.c_str());//note:this call must be executed after TextureHelper::load2DTexture 
		_pres_mg = new res_edit();


	//class rotate_pointer
	//{
	//	ft_image* _pointer_image_left;
	//	ft_image* _pointer_image_right;
	//public:
	//	rotate_pointer(ft_base&root)
	//	{
	//		_pointer_image_left = static_cast<ft_image*>(find_a_uc_from_uc(root, "pointer_left"));
	//		_pointer_image_right = static_cast<ft_image*>(find_a_uc_from_uc(root, "pointer_right"));

	//	}
	//	void rotate()
	//	{
	//		static float ia = 0.0f;
	//		ia += 0.01f;
	//		_pointer_image_left->rotate(ia);
	//	}
	//}_rt_pointer(*_proot);
	//_app.register_update_fun("rotate_pointer", bind(&rotate_pointer::rotate, &_rt_pointer));
	}
	project_edit pjedit(*_proot);
	function<void(en_short_cut_item)> fun_shortct = [_proot, pjedit, &fun_shortct](en_short_cut_item enshort) mutable {
		if (!_proot)
		{
			return;
		}
		switch (enshort)
		{
		case en_ctrl_n:
		{
			if (_proot)
			{
				int result = MessageBox(GetForegroundWindow(), "Save changes to the current project?", "auto future graphics designer", MB_YESNOCANCEL);
				if (result == IDCANCEL)
				{
					return;
				}
				else
				if (result == IDYES)
				{
					fun_shortct(en_ctrl_s);
				}
				pjedit.clear_sel_item();
				delete _proot;
				_proot = NULL;

			}
			g_cureent_project_file_path = "";
			_proot = new ft_base;
			_proot->set_name(string("screen"));

		}
		break;
		case en_ctrl_o:
		{
			OPENFILENAME ofn = { sizeof(OPENFILENAME) };
			ofn.hwndOwner = GetForegroundWindow();
			ofn.lpstrFilter = "valid file:\0*.afproj\0\0";
			char strFileName[MAX_PATH] = { 0 };
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = strFileName;
			ofn.nMaxFile = sizeof(strFileName);
			ofn.lpstrTitle = "select a auto-future graphics design project file(*.afproj) please!";
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			if (GetOpenFileName(&ofn))
			{
				printf("open file%s\n", strFileName);
				if (_proot)
				{
					int result = MessageBox(GetForegroundWindow(), "Save changes to the current project?", "auto future graphics designer", MB_YESNOCANCEL);
					if (result==IDCANCEL)
					{
						return;
					}
					else
					if (result==IDYES)
					{
						fun_shortct(en_ctrl_s);
					}
					pjedit.clear_sel_item();
					delete _proot;
					_proot = NULL;
				}
				g_cureent_project_file_path = strFileName;
				_proot = new ft_base;
				ui_assembler _ui_as(*_proot);
				_ui_as.load_ui_component_from_file(g_cureent_project_file_path.c_str());//note:this call must be executed after TextureHelper::load2DTexture 

			}
		}
		break;
		case en_ctrl_s:
		{
			if (g_cureent_project_file_path.empty())
			{
				OPENFILENAME sfn = { sizeof(OPENFILENAME) };
				sfn.hwndOwner = GetForegroundWindow();
				sfn.lpstrFilter = "afd project file:\0*.afproj\0\0";
				sfn.lpstrDefExt = "afproj";
				char strFileName[MAX_PATH] = { 0 };
				sfn.nFilterIndex = 1;
				sfn.lpstrFile = strFileName;
				sfn.nMaxFile = sizeof(strFileName);
				sfn.lpstrTitle = "Save to";
				sfn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
				sfn.FlagsEx = OFN_EX_NOPLACESBAR;
				if (GetSaveFileName(&sfn))
				{
					g_cureent_project_file_path = strFileName;
				}
				else
				{
					return;
				}
			}
			
			ui_assembler _ui_as(*_proot);
			_ui_as.output_ui_component_to_file(g_cureent_project_file_path.c_str());
		}
		break;

		}
	};

	// Main loop
    while (!glfwWindowShouldClose(window))
    {
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();
		//socketpair
#define _MY_IMGUI__
#if defined(_Dockable_)

		if (ImGui::Begin("imguidock window (= lumix engine's dock system)",NULL,ImVec2(500, 500),0.95f,ImGuiWindowFlags_NoScrollbar)) {
			ImGui::BeginDockspace();
			static char tmp[128];
			for (int i=0;i<10;i++)  {
				sprintf(tmp,"Dock %d",i);
				if (i==9) ImGui::SetNextDock(ImGuiDockSlot_Bottom);// optional
				if(ImGui::BeginDock(tmp))  {
					ImGui::Text("Content of dock window %d goes here",i);
				}
				ImGui::EndDock();
			}
			ImGui::EndDockspace();
		}
		ImGui::End();
		//ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		//const ImGuiWindowFlags flags =  (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
		//const float oldWindowRounding = ImGui::GetStyle().WindowRounding;ImGui::GetStyle().WindowRounding = 0;
		//const bool visible = ImGui::Begin("imguidock window (= lumix engine's dock system)",NULL,ImVec2(0, 0),1.0f,flags);
		//ImGui::GetStyle().WindowRounding = oldWindowRounding;
		//if (visible) {
		//	ImGui::BeginDockspace();
		//	static char tmp[128];
		//	for (int i=0;i<10;i++)  {
		//		sprintf(tmp,"Dock %d",i);
		//		if (i==9) ImGui::SetNextDock(ImGuiDockSlot_Bottom);// optional
		//		if(ImGui::BeginDock(tmp))  {
		//			ImGui::Text("Content of dock window %d goes here",i);
		//		}
		//		ImGui::EndDock();
		//	}
		//	ImGui::EndDockspace();
		//}
		//ImGui::End();
#elif defined(_DEMO_)
        // 1. Show a simple window.
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
        {
            static float f = 0.0f;
            static int counter = 0;
            ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
        if (show_demo_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowDemoWindow(&show_demo_window);
        }
#elif defined(_MY_IMGUI__)
		//ImGui::SetNextWindowPos(ImVec2(0, 0));
		static bool show_project_window=true,show_edit_window=true,show_property_window=true,show_resource_manager=true;
		if (ImGui::GetIO().KeyCtrl)
		{
			if (ImGui::GetIO().KeysDown[GLFW_KEY_S])
			{
				fun_shortct(en_ctrl_s);
			}
			else
			if (ImGui::GetIO().KeysDown[GLFW_KEY_O])
			{
				fun_shortct(en_ctrl_o);
			}
			else
			if (ImGui::GetIO().KeysDown[GLFW_KEY_N])
			{
				fun_shortct(en_ctrl_n);
			}
		}

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				//ImGui::MenuItem("(dummy menu)", NULL, false, false);
				if (ImGui::MenuItem("New Project", "Ctrl+N")) 
				{
					fun_shortct(en_ctrl_n);
				}
				if (ImGui::MenuItem("Open Project...", "Ctrl+O")) 
				{
					fun_shortct(en_ctrl_o);
				}
				if (ImGui::BeginMenu("Open Recent"))
				{
					ImGui::MenuItem("haima.afproj");
					ImGui::EndMenu();
				}
				if (ImGui::GetIO().KeyCtrl)
				{
					printf("ctrl is pressed\n");
				}
				if (ImGui::MenuItem("Save", "Ctrl+S") )
				{
					fun_shortct(en_ctrl_s);
				}
				if (ImGui::MenuItem("Save As..")) 
				{
				}
				ImGui::Separator();
				
				if (ImGui::MenuItem("Quit", "Alt+F4")) 
				{
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) 
				{
				}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) 
				{
				}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) 
				{
				}
				if (ImGui::MenuItem("Copy", "CTRL+C")) 
				{
				}
				if (ImGui::MenuItem("Paste", "CTRL+V")) 
				{
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				ImGui::MenuItem("Close Active Window", NULL, false, false);
				ImGui::Separator();

				if (ImGui::MenuItem("Project Window", NULL, show_project_window)) { show_project_window = !show_project_window; }
				if (ImGui::MenuItem("Edit Window", NULL, show_edit_window)) { show_edit_window = !show_edit_window; }
				if (ImGui::MenuItem("Property Window", NULL, show_property_window)) { show_property_window = !show_property_window; }
				if (ImGui::MenuItem("Resource Manager", NULL, show_resource_manager)) { show_resource_manager = !show_resource_manager; }


				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		if (show_project_window)
		{
			ImGui::Begin("project");
			if (_proot)
			{
				pjedit.objects_view();
				pjedit.popup_context_menu();
			}
			ImGui::End();
		}
		if (show_property_window)
		{
			ImGui::Begin("property");
			_pselect = pjedit.current_object();
			if (_pselect)
			{
				_pselect->draw_peroperty_page();
			}
			ImGui::End();
		}
		if (show_edit_window)
		{
			ImGui::SetNextWindowSize(ImVec2(1920, 720), ImGuiCond_FirstUseEver);
			ImGui::Begin("edit window", &show_edit_window);
			//
			if (_proot)
			{
				_proot->draw();
			}
			ImGui::End();
		}
		if (show_resource_manager)
		{
			ImGui::Begin("resources manager", &show_resource_manager, ImVec2(200, 500));
			ImGui::BeginDockspace();

			ImGui::BeginDock("resource list:");
			if (_pres_mg)
			{
				_pres_mg->draw_res_list();
			}
			ImGui::EndDock();

			ImGui::BeginDock("resource item property:");
			if (_pres_mg)
			{
				_pres_mg->draw_res_item_property();
			}
			ImGui::EndDock();

			ImGui::EndDockspace();
			ImGui::End();
		}
		
#endif
        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}
