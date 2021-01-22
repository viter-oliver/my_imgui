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
#include <Shlwapi.h>
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
#include "af_font_res_set.h"
#include "af_primitive_object.h"
#include "af_model.h"
#include "material_shader_edit.h"
#include "texture_edit.h"
#include "fonts_edit.h"
#include "file_res_edit.h"
#include "model_edit.h"
#include "bind_edit.h"
#include "aliase_edit.h"
#include "state_manager_edit.h"
#include "base_prp_type_edit.h"
#include "playlist_group_edit.h"
#include "slider_path_picker.h"
#include "primitve_edit.h"
#include "feedback_edit.h"
#include "common_functions.h"
#include "dir_output.h"
#include "command_element_delta.h"
#include "unreferenced_items.h"
#include "get_web_time.h"
#include "aes.h"
#include "video_capture.h"
#include "main_version.h"
#include "svn_version.h"
#include "HtmlHelp.h"
#include <fstream>
#include <thread>
#include <atomic>
#include "simple_http.h"
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}
string g_cureent_project_file_path;
string g_cureent_directory;
string g_current_running_directory;
string g_afb_output_path;
bind_edit g_bind_edit;
aliase_edit g_aliase_edit;
state_manager_edit g_state_manager_edit;
base_prp_type_edit g_common_value_edit;
playlist_group_edit g_playlist_group_edit;
slider_path_picker g_slider_path_picker;
unreferenced_items g_unreferenced_items;
primitve_edit g_primitive_edit;
feedback_edit g_feedback_edit;
ft_base* _proot = NULL;
shared_ptr<project_edit> prj_edit;
HCURSOR g_hcursor_wait;
//string g_current_run_path;
enum en_short_cut_item
{
	en_ctrl_n,
	en_ctrl_o,
	en_ctrl_s,
	en_ctrl_b,
	en_ctrl_f2,
	an_alt_f4,
};
function<void(en_short_cut_item)> fun_shortct;
bool show_project_window = true, show_edit_window = true, \
show_property_window = true, show_texture_res_manager = true,show_shader_material_manager=false, \
show_fonts_manager = true, show_file_manager = true, \
show_output_format = false, show_model_list = false, show_world_space = false, \
show_bind_edit = false, show_state_manager_edit = false, show_aliase_edit = false, \
show_slider_path_picker = false, show_prm_edit = false, show_video_dev_mg = false,\
show_common_value_edit = false,show_feedback_edit=false,show_playlist_group=false;

enum en_editing_wd_state
{
	en_editing,
	en_preview,
};

int g_editing_state = en_editing;
bool is_editing()
{
     return g_editing_state == en_editing;
}
static string front_part_license_valid_time_key = "fplvt";
static string rear_part_file_name = "rplvt";
static string reg_path = "afg_ide";
static string ikey = "autofuture&afgER";// "max&maj20190815x";

string iv = "123456qwaszx0000";

void drag_dop_callback(GLFWwindow*wh, int cnt, const char** fpaths)
{
	int last_id = cnt - 1;
	g_cureent_project_file_path = fpaths[last_id];
	g_cureent_directory = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
	if (g_ui_edit_command_mg.undo_able() || g_ui_edit_command_mg.redo_able())
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
	}
	prj_edit->clear_sel_item();
	delete _proot;

	_proot = new ft_base;
	_proot->set_name("screen");
	ui_assembler _ui_as(*_proot);
	_ui_as.load_afg_from_file(g_cureent_project_file_path.c_str());//note:this call must be executed after TextureHelper::load2DTexture 

	prj_edit.reset(new project_edit(*_proot));
}
//#define _MY_IMGUI__
//#define _DEMO_
#pragma data_seg("afg_seg")
uint32_t s_user_count=0;
#pragma data_seg()
#pragma comment(linker,"/section:afg_seg,RWS")
static DWORD fp_lc_len = 16;
static DWORD lc_ld_len = 32;
static DWORD app_valid_min = 0;
atomic<bool> counting_thread_is_running( false );
DWORD get_valid_mins( vector<BYTE>& plainText )
{
     if( plainText.size() != lc_ld_len )
     {
          return 0;
     }
     if( plainText[ 0 ] != 'a' || plainText[ 1 ] != 'f' || plainText[ 2] != 'g' )
     {
          return 0;
     }
     DWORD valid_mins;
     memcpy( &valid_mins, &plainText[ 8 ], 4 );
     return valid_mins;
}
void pack_value_to_plainText( vector<BYTE>& plainText, DWORD valid_mis )
{
     plainText[ 0 ] = 'a';
     plainText[ 1 ] = 'f';
     plainText[ 2] =  'g';

     DWORD* pvalid_pos = (DWORD*)&plainText[  8 ];
     *pvalid_pos = valid_mis;
}
#define _request_authentication

int main( int argc, char* argv[] )
{

	s_user_count++;
	/*HWND hwnd = GetConsoleWindow();
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));*/
	     
	//register_app_and_icon(g_app_path);
	printf("%s__%d\n", __FILE__, s_user_count);
     string running_app = argv[ 0 ];
     g_current_running_directory = running_app.substr( 0, running_app.find_last_of( '\\' ) + 1 );
     printf( "cur_app:%s\n", argv[ 0 ] );

#ifdef _request_authentication
     string rear_part_lv = g_current_running_directory + rear_part_file_name;
	AESModeOfOperation moo;
	moo.set_key((unsigned char*)ikey.c_str());
	moo.set_mode(MODE_CBC);
	moo.set_iv((BYTE*)iv.c_str());
	HKEY hKey;
     vector<BYTE> encrpText;
     encrpText.resize( fp_lc_len * 2 );
	vector<BYTE> plainText;
     plainText.resize( fp_lc_len * 2 );
     DWORD data_type = REG_BINARY;
     
     auto lret = RegGetValue( HKEY_CURRENT_USER, "afg_ide\\fplvt", NULL, RRF_RT_REG_BINARY, &data_type, &encrpText[ 0 ], &fp_lc_len );
     if (lret==ERROR_SUCCESS)
     {
          //RegCloseKey( hKey );
          ifstream ifs;
          ifs.open( rear_part_lv, ios::binary );
          if( ifs.is_open() )
          {
               ifs.read( (char*)&encrpText[ fp_lc_len ], fp_lc_len );
               moo.Decrypt( &encrpText[ 0 ], fp_lc_len * 2, &plainText[ 0 ] );
               app_valid_min = get_valid_mins( plainText );
               ifs.close();
          }
     }
     /**
	auto lRet = RegOpenKeyEx(HKEY_CURRENT_USER, "afg_ide\\fplvt", 0, KEY_READ, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		//读取键值
          //string reg_path = front_part_license_valid_time_key + "\\";
          auto get_rt = RegQueryValueEx( hKey, NULL, 0, &data_type, &encrpText[ 0 ], &fp_lc_len );
          if( get_rt == ERROR_SUCCESS )
		{
		     //关闭键
		     RegCloseKey(hKey);
               ifstream ifs;
               ifs.open( rear_part_lv, ios::binary );
               if (ifs.is_open())
               {
                    ifs.read( (char*)&encrpText[ fp_lc_len ], fp_lc_len );
                    moo.Decrypt( &encrpText[0], fp_lc_len*2,&plainText[0] );
                    app_valid_min = get_valid_mins( plainText );
                    ifs.close();
               }
		}
          else
          {

          }
	}*/
     auto save_value = [&]()
     {
          pack_value_to_plainText( plainText, app_valid_min );
          moo.Encrypt( &plainText[ 0 ], fp_lc_len * 2, &encrpText[ 0 ] );
          ofstream ofs;
          ofs.open( rear_part_lv, ios::binary );
          ofs.write( (const char*)&encrpText[ fp_lc_len ], fp_lc_len );
          ofs.flush();
          SetFileAttributesA( rear_part_lv.c_str(), FILE_ATTRIBUTE_HIDDEN );
          ofs.close();
          HKEY hKey;
          if( RegOpenKeyEx( HKEY_CURRENT_USER, reg_path.c_str(), 0, KEY_WRITE, &hKey ) != ERROR_SUCCESS )
          {
               DWORD state;
               RegCreateKeyEx( HKEY_CURRENT_USER, reg_path.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hKey, &state );
          }
          //RegSetValueEx
          RegSetKeyValue( hKey, front_part_license_valid_time_key.c_str(), 0, REG_BINARY, &encrpText[ 0 ], fp_lc_len );
          RegCloseKey( hKey );
     };
     thread td_counting( [&]()
     {
          while( true )
          {
               if( app_valid_min > 0 )
               {
                    save_value();
                    app_valid_min--;
                    this_thread::sleep_for( chrono::seconds( 60 ) );
               }
               else
               {
                    this_thread::sleep_for( chrono::seconds( 1 ) );
               }
          }

     } );
     td_counting.detach();
#endif
     
    // Setup window
	if (argc>1)
	{
		g_cureent_project_file_path = argv[1];
		g_cureent_directory=g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
	}

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
	g_hcursor_wait = LoadCursor(NULL, IDC_WAIT);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);

	GLFWmonitor*  pmornitor = glfwGetPrimaryMonitor();
	const GLFWvidmode * mode = glfwGetVideoMode(pmornitor);
	int iw, ih;
	iw = mode->width;
	ih = mode->height;
	//printf("x%", this);
     GLFWwindow* window = glfwCreateWindow(iw, ih, "Auto Future Graphics Designer", NULL, NULL);
	HWND hwnd_main = glfwGetWin32Window(window);
	SendMessage(hwnd_main, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));

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
	if (0)//!g_cureent_project_file_path.empty())
	{
		string str_font_path = g_cureent_directory;
		str_font_path += "fonts\\";
		string FZLanTingHeiS= str_font_path + "FZLanTingHeiS-R-GB.ttf";
		//string arial = str_font_path +"arialuni.ttf";

		io.Fonts->AddFontFromFileTTF(FZLanTingHeiS.c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesChinese());
	}

     bool show_demo_window = true;
     bool show_another_window = false;
     bool show_edit_window = true;
	//ImVec2 edit_window_size = ImVec2()
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
#ifdef _MY_IMGUI__
	load_internal_texture_res(g_mtxt_intl, IDR_FONTS1,IDB_INTERNAL_TXT_RES, IDR_INTERNAL_TXT_FMT);

	int max_txt_size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_txt_size);

	base_ui_component* _pselect = NULL;
	shared_ptr<res_edit> _pres_mg;
	shared_ptr<material_shader_edit> _pml_shd_mg;

	_proot = new ft_base;
	_proot->set_name("screen");
	g_pfont_face_manager = make_shared<font_face_manager>();
	init_controls_res_constrained();
	if (!g_cureent_project_file_path.empty())
	{
		ui_assembler _ui_as(*_proot);
		_ui_as.load_afg_from_file(g_cureent_project_file_path.c_str());//note:this call must be executed after TextureHelper::load2DTexture 
	}
	//init_internal_primitive_list();
	init_common_type_property_handles();
	_pres_mg = make_shared< res_edit>();
	_pml_shd_mg = make_shared<material_shader_edit>();
	auto ptexture = make_shared<texture_edit>();
	//project_edit pjedit(*_proot);
	prj_edit = make_shared<project_edit>(*_proot);

	auto pfonts_edit = make_shared<fonts_edit>();
	auto pfiles_edit = make_shared<file_res_edit>();
	auto pmodel_edit = make_shared<model_edit>();
	fun_shortct = [&](en_short_cut_item enshort) mutable {
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
				prj_edit->clear_sel_item();
				delete _proot;
				_proot = NULL;

			}
			g_cureent_project_file_path = "";
			_proot = new ft_base;
			_proot->set_name("screen");
			prj_edit.reset(new project_edit(*_proot));
		}
		break;
		case en_ctrl_o:
		{
			OPENFILENAME ofn = { sizeof(OPENFILENAME) };
			ofn.hwndOwner = GetForegroundWindow();
			ofn.lpstrFilter = "valid file:\0*.afg\0\0";
			char strFileName[MAX_PATH] = { 0 };
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = strFileName;
			ofn.nMaxFile = sizeof(strFileName);
			ofn.lpstrTitle = "select a auto-future graphics design project file(*.afg) please!";
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			if (GetOpenFileName(&ofn))
			{
				printf("open file%s\n", strFileName);
				if (_proot && (g_ui_edit_command_mg.undo_able() || g_ui_edit_command_mg.redo_able()))
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
					prj_edit->clear_sel_item();				
					delete _proot;
					_proot = NULL;
				}
				g_cureent_project_file_path = strFileName;
				g_cureent_directory = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
				_proot = new ft_base;
				_proot->set_name("screen");
				ui_assembler _ui_as(*_proot);
				_ui_as.load_afg_from_file(g_cureent_project_file_path.c_str());//note:this call must be executed after TextureHelper::load2DTexture 
				
				prj_edit.reset(new project_edit(*_proot));
			}
		}
		break;
		case en_ctrl_s:
		{
			if (g_cureent_project_file_path.empty())
			{
				OPENFILENAME sfn = { sizeof(OPENFILENAME) };
				sfn.hwndOwner = GetForegroundWindow();
				sfn.lpstrFilter = "afg project file:\0*.afg\0\0";
				sfn.lpstrDefExt = "afg";
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
                         g_afb_output_path = g_cureent_directory + afb_fold;
					createDirectory(mesh_path.c_str());
					createDirectory(font_path.c_str());
					createDirectory(files_path.c_str());
					createDirectory(image_path.c_str());
					createDirectory(shader_path.c_str());
					createDirectory(text_res_path.c_str());
                         createDirectory( g_afb_output_path.c_str() );

				}
				else
				{
					return;
				}
			}
			
			ui_assembler _ui_as(*_proot);
			_ui_as.output_ui_component_to_file(g_cureent_project_file_path.c_str());
               g_ui_edit_command_mg.resume();
		}
		break;
		case en_ctrl_b:
		{
			if (!g_cureent_project_file_path.empty())
			{
                    string str_afb_file;
                    string str_proj_file_path = g_cureent_project_file_path.substr( 0, g_cureent_project_file_path.find_last_of( '\\' ) + 1 );
                    string str_proj_file_name = g_cureent_project_file_path.substr( g_cureent_project_file_path.find_last_of( '\\' ) + 1 );
                    str_proj_file_name = str_proj_file_name.substr( 0, str_proj_file_name.find( '.' ) );
                    if( PathFileExists( g_afb_output_path.c_str() ) )
                    {
                         str_afb_file = g_afb_output_path + str_proj_file_name+".AFB";
                    }
                    else
                    {
                         str_afb_file = str_proj_file_path + afb_fold;
                         g_afb_output_path = str_afb_file;
                         if( !PathFileExists( g_afb_output_path.c_str() ) )
                         {
                              createDirectory( g_afb_output_path.c_str() );
                         }
                         str_afb_file += str_proj_file_name;
                         str_afb_file += ".AFB";
                    }
                    printf( "afbfile:%s\n", str_afb_file.c_str() );
				
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
	glfwSetDropCallback(window, drag_dop_callback);
	glfwUsbDevicStateCallback(usb_device_state);
	usb_device_state(1,NULL);
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
		if (glfwGetWindowAttrib(window,GLFW_ICONIFIED))
		{
			glfwWaitEvents();
			continue;
		}
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
#ifdef _request_authentication
        if(app_valid_min ==0)
		{
			ImGui::OpenPopup("get license");
		}
          
		if (ImGui::BeginPopupModal("get license"))
		{
#define LICENSE_LEN 0x100
			static char str_license_num[LICENSE_LEN] = "";
			static char str_state[LICENSE_LEN] = "";
			ImGui::InputText("enter license number here.", str_license_num, LICENSE_LEN);
			ImGui::Text(str_state);
			if (ImGui::Button("Ok"))
			{
				string slicense = str_license_num;
				if (slicense.size() != 64)
				{
					auto state = "invalid license number!";
					strcpy(str_state, state);
				}
				else
				{
                         //string afg_mg_path = "http://10.0.0.101:8080/afg_manager/afg_authentification";
                         Value jpost( objectValue );
                         vector<BYTE> mac_address_bt;
                         string str_mac = get_mac_address( mac_address_bt );
                         jpost[ "mac_adrress" ] = str_mac;
                         jpost[ "license" ] = slicense;
                         string str_post =jpost.toStyledString();
                     
                         DWORD http_response_code = -1;
                         unsigned char *http_response_content = NULL;
                         http_response_code = https_post_binary( "http://202.105.144.245:8080/afg_manager/afg_authentification", \
                                                                 (unsigned char*)str_post.c_str(), str_post.length(), &http_response_content );
                         printf( "%d\n", http_response_code );
                         printf( "%s\n", http_response_content );
                         string str_encrp_text( (char*)http_response_content );
                         string str_enc_txt = str_encrp_text.substr( str_encrp_text.size() - 69,64 );
                         vector<BYTE> encrp_text;
                         char_to_hex( str_enc_txt, encrp_text );
                         vector<BYTE>plain_text;
                         plain_text.resize( encrp_text.size() );
                         moo.Decrypt( &encrp_text[ 0 ], encrp_text.size(), &plain_text[ 0 ] );
                        
                         if( mac_address_bt[ 0 ] != plain_text[ 8 ] 
                             || mac_address_bt[ 1 ] != plain_text[ 9 ] 
                             || mac_address_bt[ 2 ] != plain_text[ 10 ] 
                             || mac_address_bt[ 3 ] != plain_text[ 11 ] 
                             || mac_address_bt[ 4 ] != plain_text[ 12 ]
                             || mac_address_bt[ 5 ] != plain_text[ 13 ]
                             )
                         {
                              printf( "this license is occupied,you must cancel the authorization of host:%2x-%2x-%2x-%2x-%2x-%2x\n",\
                                      plain_text[ 8 ], plain_text[ 9 ], plain_text[ 10 ], plain_text[ 11 ], plain_text[ 12], plain_text[ 13 ] );
                              app_valid_min = 0;
                         }
                         else
                         {
                              UINT* pvalid_time = (UINT*)&plain_text[ 4 ];
                              app_valid_min = *pvalid_time;
                              if( app_valid_min >0)
                              {
                                   ImGui::CloseCurrentPopup();
                              }
                              
                         }
                         
                         //string str_rt = cly::post( afg_mg_path );
                         //printf( "%s\n", str_rt.c_str() );
				}
				
			}
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
				exit(0);
			}
			ImGui::EndPopup();
			goto Rendering;
		}
#endif
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
               if( ImGui::GetIO().KeysDown[ GLFW_KEY_H ] )
               {
                    auto cur_ui_obj=prj_edit->current_object();
                    if (cur_ui_obj)
                    {
                         bool be_visible = cur_ui_obj->is_visible();
                         cur_ui_obj->set_visible( !be_visible );
                         std::this_thread::sleep_for( std::chrono::milliseconds( 300 ) );
                    }
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
               else
               if( ImGui::IsKeyReleased(GLFW_KEY_C) )
               {
                    prj_edit->copy_item();
               }
               else
               if( ImGui::IsKeyReleased( GLFW_KEY_X ))
               {
                    prj_edit->cut_item();
               }
               else
               if( ImGui::IsKeyReleased( GLFW_KEY_V ))
               {
                    prj_edit->past_item();
               }               
               else
               if( ImGui::IsKeyReleased( GLFW_KEY_A ))
               {
                    prj_edit->add_sibling();
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
					ImGui::MenuItem("haima.afg");
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
				/*if (ImGui::MenuItem("Save As..")) 
				{
				}*/
				if (ImGui::BeginMenu("Import"))
				{
					if (ImGui::MenuItem("Import 3D Assert", "Alt+F2"))
					{
						fun_shortct(en_ctrl_f2);
					}
					ImGui::EndMenu();
				}
				ImGui::Separator();
                    ImGui::Text( "AFB output path:%s", g_afb_output_path.c_str() );
                    if (ImGui::MenuItem("Set AFB output path"))
                    {
                         char szBuff[ MAX_PATH ];
                         BROWSEINFO bi = { 0 };
                         bi.hwndOwner = GetForegroundWindow();
                         bi.lpszTitle = "Select a directory as AFB output path";
                         bi.ulFlags = BIF_NEWDIALOGSTYLE;
                         LPITEMIDLIST idl = SHBrowseForFolder( &bi );
                         if( SHGetPathFromIDList(idl,szBuff))
                         {
                              g_afb_output_path = szBuff;
                              g_afb_output_path += '\\';
                         }
                    }
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

                    auto psel_ui=prj_edit->current_object();
                    if( psel_ui&&ImGui::MenuItem( "Insert ui node from afg project..." ) )
                    {
                         int result = MessageBox( GetForegroundWindow(), "Be careful about this operation! \nObjects in this project that have the same name as objects in the imported project will be overwritten.", "Import", MB_YESNOCANCEL );
                         if( result == IDYES )
                         {
                              OPENFILENAME ofn = { sizeof( OPENFILENAME ) };
                              ofn.hwndOwner = GetForegroundWindow();
                              ofn.lpstrFilter = "valid file:\0*.afg\0\0";
                              char strFileName[ MAX_PATH ] = { 0 };
                              ofn.nFilterIndex = 1;
                              ofn.lpstrFile = strFileName;
                              ofn.nMaxFile = sizeof( strFileName );
                              ofn.lpstrTitle = "select a auto-future graphics design project file(*.afg) please!";
                              ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                              if( GetOpenFileName( &ofn ) )
                              {
                                   printf( "open file%s\n", strFileName );
                                   ui_assembler _ui_as( *_proot );
                                   _ui_as.load_ui_component_from_file( *psel_ui, strFileName );
                              }
                         }     
                    }
                    ImGui::Separator();
                    if( ImGui::MenuItem( "Quit", "Alt+F4" ) )
                    {
                         fun_shortct( an_alt_f4 );
                    }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				//int addr;
				//__asm
				//{
				//	call _here
				//	_here : pop eax
				//			; eax now holds the PC.
				//			mov[addr], eax
				//}
				//printf("%x\n", addr);
				bool uable = g_ui_edit_command_mg.undo_able();
				if (ImGui::MenuItem("Undo", "CTRL+Z",false,uable)) 
				{
					g_ui_edit_command_mg.undo_command();
				}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, g_ui_edit_command_mg.redo_able())) 
				{
					g_ui_edit_command_mg.redo_command();
				}
				if (ImGui::Button("Delete unreferenced items"))
				{
                         _pml_shd_mg->clear_states();
                         ptexture->clear_states();
                         pfiles_edit->clear_states();
                         pmodel_edit->clear_states();
                         g_primitive_edit.clear_states();
					ImGui::OpenPopup("Delete unreferenced Items");
					g_unreferenced_items.search_unreferenced_items();
				}
				if (ImGui::BeginPopupModal("Delete unreferenced Items"))
				{
					g_unreferenced_items.show_delete_unreferenced_items();
					ImGui::EndPopup();
				}

				// Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Copy", "CTRL+C")) 
				{
                         prj_edit->copy_item();
				}				
                    if (ImGui::MenuItem("Cut", "CTRL+X")) 
				{
                         prj_edit->cut_item();
				}
				if (ImGui::MenuItem("Paste", "CTRL+V")) 
				{
                         prj_edit->past_item();
				}
                    if (ImGui::MenuItem("Add sibling","CTRL+A"))
                    {
                         prj_edit->add_sibling();
                    }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

				ImGui::MenuItem("Close Active Window", NULL, false, false);
				ImGui::Separator();

				if (ImGui::MenuItem("Project Window", NULL, show_project_window))
                    { 
                         show_project_window = !show_project_window; 
                    }
				if (ImGui::MenuItem("Edit Window", NULL, show_edit_window)) 
                    { 
                         show_edit_window = !show_edit_window; 
                    }
				if (ImGui::MenuItem("Property Window", NULL, show_property_window)) 
                    { 
                         show_property_window = !show_property_window; 
                    }
				if (ImGui::MenuItem("Texture Resource Manager", NULL, show_texture_res_manager)) 
                    { 
                         show_texture_res_manager = !show_texture_res_manager; 
                    }
                    if( ImGui::MenuItem( "Shader&Material Manager", NULL, show_shader_material_manager ) )
                    {
                         show_shader_material_manager = !show_shader_material_manager;
                    }
				if (ImGui::MenuItem("Fonts Manager", NULL, show_fonts_manager)) 
                    { 
                         show_fonts_manager = !show_fonts_manager; 
                    }
				if (ImGui::MenuItem("Files Manager", NULL, show_file_manager)) 
                    { 
                         show_file_manager = !show_file_manager; 
                    }
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
				if (ImGui::MenuItem("State manager edit", NULL, show_state_manager_edit))
				{
					show_state_manager_edit = !show_state_manager_edit;
				}
                    if( ImGui::MenuItem( "Common value edit", NULL, show_common_value_edit ) )
                    {
                         show_common_value_edit = !show_common_value_edit;
                    }
				if (ImGui::MenuItem("Playlist group edit", NULL, show_playlist_group))
				{
					show_playlist_group = !show_playlist_group;
				}
				if (ImGui::MenuItem("Aliases edit", NULL, show_aliase_edit))
				{
					show_aliase_edit = !show_aliase_edit;
				}
				if (ImGui::MenuItem("Slider path pick", NULL, show_slider_path_picker))
				{
					show_slider_path_picker = !show_slider_path_picker;
				}
				if (ImGui::MenuItem("Primitive objects", NULL, show_prm_edit))
				{
					show_prm_edit = !show_prm_edit;
				}
				if (ImGui::MenuItem("Feedback objects", NULL, show_feedback_edit))
				{
					show_feedback_edit = !show_feedback_edit;
				}
				
				if (ImGui::MenuItem("Video device manager", NULL, show_video_dev_mg))
				{
					show_video_dev_mg = !show_video_dev_mg;
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
               if (ImGui::BeginMenu("Help"))
               {
                    if (ImGui::MenuItem("ViewHelp"))
                    {
                         char szFilePath[ MAX_PATH ], szPath[ MAX_PATH ];
                         GetModuleFileName( NULL, szFilePath, MAX_PATH ); //获得当前程序目录
                         string sfp( szFilePath );
                         string sdp = sfp.substr( 0, sfp.find_last_of( '\\' ) + 1 );
                         sdp += "help.chm";
                         HtmlHelp( NULL, sdp.c_str(), HH_DISPLAY_TOPIC, NULL );
                         
                    }
                    if( ImGui::Button( "About" ) )
                    {
                         ImGui::OpenPopup( "About" );
                    }
                    if( ImGui::BeginPopupModal( "About", NULL, ImGuiWindowFlags_NoTitleBar ) )
                    {
                         ImGui::Text( "Auto-future graphics designer" );
                         ImGui::Text( "Copyright 2020 by viter/Auto-future company" );
                         ImGui::Text( "Version:" MAINVERSION SOFTWARE_VERSION );
                         if( ImGui::Button( "Ok" ) )
                         {
                              ImGui::CloseCurrentPopup();
                         }
                         ImGui::EndPopup();
                    }
                    ImGui::EndMenu();
               }
			ImGui::RadioButton("Editing", &g_editing_state, en_editing);
			if (ImGui::RadioButton("Preview", &g_editing_state, en_preview))
			{
				prj_edit->clear_sel_item();
			}

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
               static vector<base_ui_component*> find_resut_list;
               auto de_search = [&]
               {
                    if (strlen(str_ctrl_name)==0)
                    {
                         return false;
                    }
                    auto search_ctrl = find_a_uc_from_uc( *_proot, str_ctrl_name );
                    if( search_ctrl )
                    {
                         prj_edit->sel_ui_component( search_ctrl );
                         prj_edit->trigger_focus_switch();
                         return true;
                    }
                    return false;
               };			
               if( ImGui::InputText( "####", str_ctrl_name, name_len, ImGuiInputTextFlags_EnterReturnsTrue ) )
               {
                       de_search();
               }
               ImGui::SameLine();
               if (ImGui::Button("X"))
               {
                    str_ctrl_name[ 0 ] = '\0';
               }
			ImGui::SameLine();
               if( ImGui::Button( "search..." ) && strlen( str_ctrl_name )>0)
			{
                    //de_search();
                    find_resut_list.clear();
                    find_by_un_from_the_node( *_proot, str_ctrl_name, find_resut_list );
                    /*if (find_resut_list.size()>0)
                    {

                    }*/
			}
			//ImGui::Text("\xE4\xBD\xA0\xE5\xA5\xBD");
               ImGui::BeginChild( "project_members", ImVec2( 0, 0 ), true );
			if (_proot)
			{
                    if (find_resut_list.size()>0)
                    {
                         int ix = 0;
                         for (const auto& ui_item:find_resut_list)
                         {
                              char cc[ 50 ] = { 0 };
                              itoa( ix, cc, 10 );
                              string btn_cap = ui_item->get_name();
                              btn_cap += "##";
                              btn_cap += cc;
                              if (ImGui::Button(btn_cap.c_str()))
                              {
                                   prj_edit->sel_ui_component( ui_item );
                                   prj_edit->trigger_focus_switch();
                                   find_resut_list.clear();
                                   break;
                              }
                              ix++;
                         }
                    }
                    else
                    {
                         prj_edit->objects_view();
                         prj_edit->popup_context_menu();
                    }
			}
               ImGui::EndChild();
			ImGui::End();
		}
		if (show_property_window)
		{
			ImGui::Begin("property");
			_pselect = prj_edit->current_object();
			if (_pselect)
			{
				//_pselect->draw_peroperty_page();
				_pselect->draw_peropertys();
			}
			ImGui::End();
		}
		video_capture_excute();

		if (show_edit_window)
		{
			ImGui::SetNextWindowSize(ImVec2(base_ui_component::screenw, base_ui_component::screenh), ImGuiCond_FirstUseEver);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
			
			ImGui::Begin("edit window", &show_edit_window, ImGuiWindowFlags_NoTitleBar );
			//
			if (_proot)
			{
				_proot->draw_frames();
			}			
			
			ImGuiContext& g = *GImGui;
			ImGuiWindow* cur_window = ImGui::GetCurrentWindow();
			ImGuiWindow* front_window = g.Windows.back();
			ImRect wrect(cur_window->Pos, cur_window->Pos + cur_window->Size);
			if (cur_window == front_window && wrect.Contains(ImGui::GetIO().MousePos))
			{
				//printf("mouse_click_pos(%f,%f)\n", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
				base_ui_component* psel_ui = _proot->get_hit_ui_object(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
				if (psel_ui)
				{
					if (is_editing())
					{
						if (ImGui::IsMouseClicked(0))
						{
							prj_edit->sel_ui_component(psel_ui);
						}
					}
					else
					{
						if (ImGui::IsMouseClicked(0))
						{
                                   psel_ui->trigger_click();
							psel_ui->mouse_clicked();
						}
						if (ImGui::IsMouseDown(0))
						{
                                   psel_ui->trigger_mouse_down();
							psel_ui->mouse_down();
						}
						if (ImGui::IsMouseReleased(0))
						{
                                   psel_ui->trigger_mouse_release();
							psel_ui->mouse_relese();
						}
						if (ImGui::IsMouseDragging(0))
						{
							auto ms_delta = ImGui::GetIO().MouseDelta;
                                   psel_ui->trigger_mouse_drag( ms_delta.x, ms_delta.y );
							psel_ui->mouse_drag(ms_delta.x, ms_delta.y);
						}
					}
				}
			}
		

			ImGui::End();
			ImGui::PopStyleVar(2);
		}
		if (show_world_space)
		{
			ImGui::Begin("World space", &show_world_space, ImVec2(400, 500));
			_pselect = prj_edit->current_object();
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
          auto be_trans_playing = [&]()->bool
          {
               for (auto& istm:g_mstate_manager)
               {
                    auto& stm = istm.second;
                    if( stm->_play_state != en_play_stop )
                    {
                         return true;
                    }
               }
               return false;
          };
          if( be_trans_playing())
		{
			//g_state_trans_player.keep_state_trans_on();
               keep_state_trans_on();
		}
		else
		if (show_state_manager_edit)
		{
			ImGui::Begin("State manager edit", &show_state_manager_edit, ImVec2(500, 600));
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 300);
			g_state_manager_edit.view_state_managers();
			ImGui::NextColumn();
			g_state_manager_edit.view_state_manager_item_property();
			ImGui::End();
		}
		  if (show_common_value_edit)
		  {
			  ImGui::Begin("Common value edit", &show_common_value_edit, ImVec2(500, 600));
			  ImGui::Columns(2);
			  g_common_value_edit.view_base_prp_list();
			  ImGui::NextColumn();
			  g_common_value_edit.base_prp_item();
			  ImGui::End();
		  }
		  if (show_playlist_group)
		  {
			  ImGui::Begin("Playlist group edit", &show_playlist_group, ImVec2(500, 600));
			  ImGui::Columns(2);
			  g_playlist_group_edit.view_playlit_group_list();
			  ImGui::NextColumn();
			  g_playlist_group_edit.playlist_group_item();
			  ImGui::End();
		  }
		if (show_slider_path_picker)
		{
			ImGui::Begin("Slider path picker", &show_slider_path_picker, ImVec2(500, 600),0.5);
			g_slider_path_picker.view();
			ImGui::End();
		}
		
		if (show_aliase_edit)
		{
			ImGui::Begin("Aliases edit", &show_aliase_edit, ImVec2(400, 400));
			//ImGui::Columns(2);
			//ImGui::SetColumnWidth(0, 200);
               ImGui::Text( "Current Alias:" );
               g_aliase_edit.aliase_item_propoerty();
               ImGui::Separator();
               ImGui::BeginChild("aliase list");
			g_aliase_edit.aliase_dic_view();
			//ImGui::NextColumn();
			g_aliase_edit.popup_new_aliase();
               ImGui::EndChild();
			ImGui::End();
		}
		if (show_prm_edit)
		{
			ImGui::Begin("Primitive objects", &show_prm_edit, ImVec2(600, 500));
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 200);
			g_primitive_edit.draw_primitive_list();
			ImGui::NextColumn();
			g_primitive_edit.draw_primitive_item_property();
			ImGui::End();
		}
		if (show_feedback_edit)
		{
			ImGui::Begin("Feedback objects", &show_feedback_edit, ImVec2(600, 500));
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 200);
			g_feedback_edit.draw_feedback_list();
			ImGui::NextColumn();
			g_feedback_edit.draw_feedback_item_property();
			ImGui::End();
		}
		if (show_texture_res_manager)
		{
			//ImGui::SetNextWindowBgAlpha(1.0f); // Transparent background
			ImGui::Begin("Texture resources manager", &show_texture_res_manager, ImVec2(200, 500));
			ImGui::BeginChild("Combined texture res list",ImVec2(1000, 220), true);
			ImGui::Columns(2);
			ImGui::Text("Spliced texture list:");
               _pres_mg->draw_res_list();
               ImGui::NextColumn();
               _pres_mg->draw_res_item_property();
			
			ImGui::NextColumn();
			ImGui::Spacing();
			ImGui::EndChild();
               ImGui::BeginChild( "Separated textures list", ImVec2( 1000, 500 ), true );
               ImGui::Columns( 2 );
               ImGui::BeginChild( "Separated_textures list", ImVec2( 0, 0 ), true );
               ImGui::Text( "Separated texture list" );
               ptexture->draw_texture_list();
               ImGui::EndChild();
               ImGui::NextColumn();
               ptexture->draw_texture_item_property();
               ImGui::NextColumn();
               ImGui::EndChild();
			ImGui::End();
		}
          if( show_shader_material_manager )
          {
               ImGui::Begin( "Shader&Material manager", &show_shader_material_manager );
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
               ImGui::End();
          }
          /**
          else
          {
               _pml_shd_mg->clear_states();
               ptexture->clear_states();
          }*/
		if (show_fonts_manager)
		{
			ImGui::Begin("Fonts Manager", &show_fonts_manager);
			ImGui::Columns(2);
			pfonts_edit->draw_fonts_list();
			ImGui::NextColumn();
			pfonts_edit->draw_font_item_pty();
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
		if (show_video_dev_mg)
		{
			ImGui::Begin("Video device Manager", &show_video_dev_mg);
			ImGui::Columns(2);
			video_dev_view();
			ImGui::NextColumn();
			video_item_edit();
			ImGui::End();
		}
#endif
        // Rendering
Rendering:
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
	if (g_ui_edit_command_mg.suspending())//g_ui_edit_command_mg.undo_able() || g_ui_edit_command_mg.redo_able())
	{
		int result = MessageBox(GetForegroundWindow(), "Save changes to the current project?", "auto future graphics designer", MB_YESNOCANCEL);
		if (result == IDYES)
		{
			fun_shortct(en_ctrl_s);
		}
	}
    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}
