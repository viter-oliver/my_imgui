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
//add fbximport include
//#include "./fbx_save_info.h"
#endif
#include "Resource.h"
#include "res_edit.h"
#include "afb_output.h"
#include "primitive_object.h"
#include "af_model.h"
#include "material_shader_edit.h"
#include "texture_edit.h"
#include "fonts_edit.h"
#include "file_res_edit.h"
#include "model_edit.h"
#include "bind_edit.h"
#include "common_functions.h"
#include "dir_output.h"
#include "command_element_delta.h"
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}
string g_cureent_project_file_path;
string g_cureent_directory;
bind_edit g_bind_edit;
//string g_current_run_path;
#include <windows.h>
enum en_short_cut_item
{
	en_ctrl_n,
	en_ctrl_o,
	en_ctrl_s,
	en_ctrl_b,
	en_ctrl_f2,
	an_alt_f4,
};
bool show_project_window = true, show_edit_window = true, \
show_property_window = true, show_resource_manager = true,\
show_fonts_manager=true,show_file_manager=true,\
show_output_format=false,show_model_list=false,show_world_space=false,\
show_bind_edit=false;
#define _MY_IMGUI__
//#define _DEMO_
int main(int argc, char* argv[])
{
    // Setup window
	if (argc>1)
	{
		g_cureent_project_file_path = argv[1];
		g_cureent_directory=g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
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
	if (1)//!g_cureent_project_file_path.empty())
	{
		string str_font_path = g_cureent_directory;
		str_font_path += "fonts\\";
		string FZLanTingHeiS= str_font_path + "FZLanTingHeiS-R-GB.ttf";
		//string arial = str_font_path +"arialuni.ttf";

		io.Fonts->AddFontFromFileTTF(FZLanTingHeiS.c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesChinese());
	}
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
	//char buffer[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH, buffer);
	//g_current_run_path = buffer;
	//g_current_run_path += "\\";

	
	//instantiating_internal_shader();
    bool show_demo_window = true;
    bool show_another_window = false;
	bool show_edit_window = true;
	//ImVec2 edit_window_size = ImVec2()
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
#ifdef _MY_IMGUI__
	load_internal_texture_res(g_mtxt_intl, IDB_INTERNAL_TXT_RES, IDR_INTERNAL_TXT_FMT);

	//g_res_texture_list[0].texture_id = TextureHelper::load2DTexture(g_res_texture_list[0].texture_path, g_res_texture_list[0].texture_width, g_res_texture_list[0].texture_height);
	//g_vres_texture_list[0].texture_id = \
	//	TextureHelper::load2DTexture(g_vres_texture_list[0].texture_path, g_vres_texture_list[0].texture_width, g_vres_texture_list[0].texture_height,\
	//	GL_RGBA,GL_RGBA,SOIL_LOAD_RGBA);
	int max_txt_size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_txt_size);
	printf("max texture size=%d\n", max_txt_size);
	base_ui_component* _proot = NULL;
	base_ui_component* _pselect = NULL;
	shared_ptr<res_edit> _pres_mg;
	shared_ptr<material_shader_edit> _pml_shd_mg;

	//if (!g_cureent_project_file_path.empty())
	//{
	//	_proot = new ft_base;
	//	ui_assembler _ui_as(*_proot);
	//	_ui_as.load_ui_component_from_file(g_cureent_project_file_path.c_str());//note:this call must be executed after TextureHelper::load2DTexture 
	//	_pres_mg = make_shared< res_edit>();
	//	_pml_shd_mg = make_shared<material_shader_edit>();

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
	//}

	//load_fbx_file();

	_proot = new ft_base;
	ui_assembler _ui_as(*_proot);
	if (!g_cureent_project_file_path.empty())
	{
		_ui_as.load_ui_component_from_file(g_cureent_project_file_path.c_str());//note:this call must be executed after TextureHelper::load2DTexture 
	}
	//init_internal_primitive_list();
	init_common_type_property_handles();
	_pres_mg = make_shared< res_edit>();
	_pml_shd_mg = make_shared<material_shader_edit>();
	auto ptexture = make_shared<texture_edit>();
	//project_edit pjedit(*_proot);
	auto pjedit = make_shared<project_edit>(*_proot);

	auto pfonts_edit = make_shared<fonts_edit>();
	auto pfiles_edit = make_shared<file_res_edit>();
	auto pmodel_edit = make_shared<model_edit>();
	function<void(en_short_cut_item)> fun_shortct = [&](en_short_cut_item enshort) mutable {
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
				pjedit->clear_sel_item();
				delete _proot;
				_proot = NULL;

			}
			g_cureent_project_file_path = "";
			_proot = new ft_base;
			_proot->set_name(string("screen"));
			pjedit.reset(new project_edit(*_proot));
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
					pjedit->clear_sel_item();				
					delete _proot;
					_proot = NULL;
				}
				g_cureent_project_file_path = strFileName;

				_proot = new ft_base;
				ui_assembler _ui_as(*_proot);
				_ui_as.load_ui_component_from_file(g_cureent_project_file_path.c_str());//note:this call must be executed after TextureHelper::load2DTexture 
				
				pjedit.reset(new project_edit(*_proot));
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
					g_cureent_directory = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
					string mesh_path=g_cureent_directory + mesh_fold;
					string font_path = g_cureent_directory + font_fold;
					string files_path = g_cureent_directory + files_fold;
					string image_path = g_cureent_directory + image_fold;
					string shader_path = g_cureent_directory + shaders_fold;
					string text_res_path = g_cureent_directory + text_res_fold;
					string afb_path = g_cureent_directory + afb_fold; 
					createDirectory(mesh_path.c_str());
					createDirectory(font_path.c_str());
					createDirectory(files_path.c_str());
					createDirectory(image_path.c_str());
					createDirectory(shader_path.c_str());
					createDirectory(text_res_path.c_str());
					createDirectory(afb_path.c_str());

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
		case en_ctrl_b:
		{
			if (!g_cureent_project_file_path.empty())
			{
				string str_proj_file_path = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
				string str_proj_file_name = g_cureent_project_file_path.substr(g_cureent_project_file_path.find_last_of('\\') + 1);
				str_proj_file_name = str_proj_file_name.substr(0, str_proj_file_name.find('.'));
				string str_afb_file = str_proj_file_path + afb_fold;
				str_afb_file += str_proj_file_name;
				str_afb_file += ".AFB";
				printf("afbfile:%s\n", str_afb_file.c_str());
				afb_output afbop(*_proot);
				afbop.output_afb(str_afb_file.c_str());
			}
		}
		break;
		case en_ctrl_f2:
		{
			OPENFILENAME ofn = { sizeof(OPENFILENAME) };
			ofn.hwndOwner = GetForegroundWindow();
			ofn.lpstrFilter = assimp_support_format;
			char strFileName[MAX_PATH] = { 0 };
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = strFileName;
			ofn.nMaxFile = sizeof(strFileName);
			ofn.lpstrTitle = "select a 3d file please!";
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			if (GetOpenFileName(&ofn))
			{
				printf("open file:%s\n", strFileName);
				int result = MessageBox(GetForegroundWindow(), "Import this model?", "Import", MB_YESNOCANCEL);
				if (result == IDCANCEL)
				{
					return;
				}
				else
				if (result == IDYES)
				{
					//import_fbx_info(strFileName);
					import_models(strFileName);
				}
			}
		}
		break;
		case an_alt_f4:
		exit(0);
		break;
		}
	};
#endif
	//glShaderBinary GL_NUM_SHADER_BINARY_FORMATS GL_SHADER_BINARY_FORMATS
	//GL_NUM_PROGRAM_BINARY_FORMATS,GL_PROGRAM_BINARY_FORMATS GL_PROGRAM_BINARY_LENGTH
//#define CHECK_SHADER_FORMAT
#ifdef CHECK_SHADER_FORMAT
	//GLboolean shaderCompiler;
	//glGetBooleanv(GL_SHADER_COMPILER, &shaderCompiler);
	GLint numFormats=0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
	GLint* formats = new GLint[numFormats];
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, formats);
	for (int i = 0; i < numFormats; i++)
	{
		printf("shader fmt:0x%x\n", formats[i]);
	}
	
	//GL_MALI_SHADER_BINARY_ARM
	delete[] formats;
#endif // CHECK_SHADER_FORMAT

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
			char str_chn[] = "我是中文";
			ImGui::Text(str_chn);
			ImGui::Text("Kanjis: \xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e (nihongo)");
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
			else
			if (ImGui::GetIO().KeysDown[GLFW_KEY_B])
			{
				fun_shortct(en_ctrl_b);
			}
			else
			if (ImGui::GetIO().KeysDown[GLFW_KEY_F2])
			{
				fun_shortct(en_ctrl_f2);
			}
			else
			if (ImGui::GetIO().KeysDown[GLFW_KEY_Z]&&g_ui_edit_command_mg.undo_able())
			{
				g_ui_edit_command_mg.undo_command();
			}
			else
			if (ImGui::GetIO().KeysDown[GLFW_KEY_Y]&&g_ui_edit_command_mg.redo_able())
			{
				g_ui_edit_command_mg.redo_command();
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
				if (ImGui::BeginMenu("Import"))
				{
					if (ImGui::MenuItem("Import 3D Assert", "Alt+F2"))
					{
						fun_shortct(en_ctrl_f2);
					}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Export AFB", "Ctrl+B"))
				{
					fun_shortct(en_ctrl_b);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Update texture resources"))
				{
					ui_assembler _ui_as(*_proot);
					_ui_as.update_texture_res();
					_ui_as.output_ui_component_to_file(g_cureent_project_file_path.c_str());
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Quit", "Alt+F4")) 
				{
					fun_shortct(an_alt_f4);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z",false,g_ui_edit_command_mg.undo_able())) 
				{
					g_ui_edit_command_mg.undo_command();
				}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, g_ui_edit_command_mg.redo_able())) 
				{
					g_ui_edit_command_mg.redo_command();
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
				if (ImGui::MenuItem("Fonts Manager", NULL, show_fonts_manager)) { show_fonts_manager = !show_fonts_manager; }
				if (ImGui::MenuItem("Files Manager", NULL, show_file_manager)) { show_file_manager = !show_file_manager; }
				if (ImGui::MenuItem("Models Manager", NULL, show_model_list))
				{
					show_model_list = !show_model_list;
				}
				if (ImGui::MenuItem("World space", NULL, show_world_space))
				{
					show_world_space = !show_world_space;
				}
				if (ImGui::MenuItem("Bind edit", NULL, show_bind_edit))
				{
					show_bind_edit = !show_bind_edit;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Setup"))
			{
				if (ImGui::MenuItem("OutputBinaryFormat", NULL, show_output_format)){
					show_output_format = !show_output_format;
				}
				ImGui::EndMenu();
			}
			
			//auto itxt_unit = g_mtxt_intl.find("ft_undo");
			//if (itxt_unit != g_mtxt_intl.end())
			//{
			//	auto& txt_unit = itxt_unit->second;
			//	ImVec2 uv0(txt_unit._x0 / g_txt_width_intl, txt_unit._y0 / g_txt_height_intl);
			//	ImVec2 uv1(txt_unit._x1 / g_txt_width_intl, txt_unit._y1 / g_txt_height_intl);
			//	int flags = g_ui_edit_command_mg.undo_able() ? 0:ImGuiButtonFlags_Disabled;
			//	//printf("flags=%d\n", flags);
			//	if (ImGui::ImageButton((ImTextureID)g_txt_id_intl, ImVec2(25.f, 23.f), uv0, uv1,flags))
			//	{
			//		g_ui_edit_command_mg.undo_command();
			//	}
			//}
			//itxt_unit = g_mtxt_intl.find("ft_redo");
			//if (itxt_unit != g_mtxt_intl.end())
			//{
			//	auto& txt_unit = itxt_unit->second;
			//	ImVec2 uv0(txt_unit._x0 / g_txt_width_intl, txt_unit._y0 / g_txt_height_intl);
			//	ImVec2 uv1(txt_unit._x1 / g_txt_width_intl, txt_unit._y1 / g_txt_height_intl);
			//	int flags = g_ui_edit_command_mg.redo_able() ? 0 : ImGuiButtonFlags_Disabled;
			//	//printf("flags1=%d\n", flags);
			//	if (ImGui::ImageButton((ImTextureID)g_txt_id_intl, ImVec2(25.f, 23.f), uv0, uv1, flags))
			//	{
			//		g_ui_edit_command_mg.redo_command();
			//	}
			//}
			
			ImGui::EndMainMenuBar();
		}
		if (show_output_format)
		{
			ImGui::Begin("output binary format", &show_output_format, ImVec2(400, 100));
			static const char* str_txt_fmts[] = { "uncompressed texture", "dxt5", "atc", "etc2", "pvrtc", "bptc", "palleted texture" };
			ImGui::Combo("texture format", (int*)&g_output_bin_format._txt_fmt, str_txt_fmts, IM_ARRAYSIZE(str_txt_fmts));
			static const char* str_progrm_fmts[] = { "shader code", "general binary", "vivante binary" };
			ImGui::Combo("program format", (int*)&g_output_bin_format._pgm_fmt, str_progrm_fmts, IM_ARRAYSIZE(str_progrm_fmts));
			

			ImGui::End();
		}
		if (show_project_window)
		{
			ImGui::Begin("project");
			static char str_ctrl_name[name_len] = {0};
			ImGui::InputText("####", str_ctrl_name, name_len);
			ImGui::SameLine();
			if (ImGui::Button("search..."))
			{
				auto search_ctrl = find_a_uc_from_uc(*_proot, str_ctrl_name);
				if (search_ctrl)
				{
					pjedit->sel_ui_component(search_ctrl);
				}
			}
			//ImGui::Text("\xE4\xBD\xA0\xE5\xA5\xBD");
			if (_proot)
			{
				pjedit->objects_view();
				pjedit->popup_context_menu();
			}
			ImGui::End();
		}
		if (show_property_window)
		{
			ImGui::Begin("property");
			_pselect = pjedit->current_object();
			if (_pselect)
			{
				//_pselect->draw_peroperty_page();
				_pselect->draw_peropertys();
			}
			ImGui::End();
		}
		if (show_edit_window)
		{
			ImGui::SetNextWindowSize(ImVec2(1920, 720), ImGuiCond_FirstUseEver);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
			
			ImGui::Begin("edit window", &show_edit_window, ImGuiWindowFlags_NoTitleBar );
			//
			if (_proot)
			{
				_proot->draw();
			}			
			ImGuiContext& g = *GImGui;
			ImGuiWindow* cur_window = ImGui::GetCurrentWindow();
			ImGuiWindow* front_window = g.Windows.back();
			ImRect wrect(cur_window->Pos, cur_window->Pos + cur_window->Size);
			
			if (cur_window == front_window&&ImGui::IsMouseClicked(0) && wrect.Contains(ImGui::GetIO().MousePos))
			{
				printf("mouse_click_pos(%f,%f)\n", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
				base_ui_component* psel_ui = _proot->get_hit_ui_object(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
				if (psel_ui)
				{
					pjedit->sel_ui_component(psel_ui);
				}
			}
			ImGui::End();
			ImGui::PopStyleVar(2);
		}
		if (show_world_space)
		{
			ImGui::Begin("World space", &show_world_space, ImVec2(400, 500));
			_pselect = pjedit->current_object();
			if (_pselect)
			{
				//_pselect->draw_peroperty_page();
				_pselect->view_components_in_world_space();
			}
			ImGui::End();
		}
		if (show_bind_edit)
		{
			ImGui::Begin("Bind edit", &show_bind_edit, ImVec2(400, 500));
			g_bind_edit.bind_source_view();
			ImGui::End();
		}
		if (show_resource_manager)
		{
			//ImGui::SetNextWindowBgAlpha(1.0f); // Transparent background
			ImGui::Begin("resources manager", &show_resource_manager, ImVec2(200, 500));
			//ImGui::BeginDockspace();

			//ImGui::BeginDock("resource list:");
			ImGui::BeginChild("res_list",ImVec2(1000, 160), true);
			ImGui::Columns(2);
			ImGui::Text("Spliced texture list:");
			if (_pres_mg)
			{
				_pres_mg->draw_res_list();
				ImGui::NextColumn();
				_pres_mg->draw_res_item_property();
			}
			ImGui::NextColumn();
			ImGui::Spacing();
			ImGui::EndChild();

			
			if(_pml_shd_mg)
			{
				ImGui::BeginChild("shaders", ImVec2(1000, 500), true);
			//ImGui::Separator();
				ImGui::Columns(2);
				ImGui::Text("shaders:");
				_pml_shd_mg->load_shader();
				_pml_shd_mg->draw_shader();
				ImGui::NextColumn();
				_pml_shd_mg->load_shader_info();
				_pml_shd_mg->draw_shader_item_property();
				ImGui::NextColumn();
				ImGui::EndChild();
				ImGui::BeginChild("materials", ImVec2(1000, 500), true);
				ImGui::Columns(2);
				ImGui::Text("materials:");
				_pml_shd_mg->draw_material();
				ImGui::NextColumn();
				_pml_shd_mg->draw_material_item_property();
				ImGui::NextColumn();
			//ImGui::Separator();
				ImGui::EndChild();
				ImGui::BeginChild("textures", ImVec2(1000, 500), true);
				ImGui::Columns(2);
				ptexture->draw_texture_list();
				ImGui::NextColumn();
				ptexture->draw_texture_item_property();
				ImGui::NextColumn();
				ImGui::EndChild();

			}
			/**/
			ImGui::End();
		}
		if (show_fonts_manager)
		{
			ImGui::Begin("Fonts Manager", &show_fonts_manager);
			//
			pfonts_edit->draw_fonts_list();
			ImGui::End();
		}
		if (show_file_manager)
		{
			ImGui::Begin("Files Manager", &show_file_manager);
			ImGui::Columns(2);
			pfiles_edit->draw_file_res_list();
			ImGui::NextColumn();
			pfiles_edit->draw_file_res_item_property();
			ImGui::End();
		}
		if (show_model_list)
		{
			ImGui::Begin("Models Manager", &show_model_list);
			ImGui::Columns(2);
			pmodel_edit->draw_model_list();
			ImGui::NextColumn();
			pmodel_edit->draw_model_item_property();
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
