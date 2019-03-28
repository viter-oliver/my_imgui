#pragma once
#include "control_common_def.h"
#ifdef IMGUI_WAYLAND
#include "../../deps/glad/glad.h"
#else
#include <GL/gl3w.h> 
#endif
/**
* @brief ui_assembler is responsible for transferring project file to ui components
*        or transferring ui components to project file\n
*/
using namespace auto_future;
extern void init_controls_res_constrained();
class ui_assembler
{
	base_ui_component& _root;
public:
	ui_assembler(base_ui_component& root):_root(root){}
	~ui_assembler(){}
	/**
	*@brief transferring project file to ui components
	*@param file_path project file path
	*@return true if success,false if failure
	*/
	bool load_ui_component_from_file(const char* file_path);
	/**
	*@brief transferring ui components to ui project file
	*@param file_path project file path
	*@return true if success,false if failure
	*/
	bool output_ui_component_to_file(const char* file_path);
	/**
	*@brief update texture resource referenced by controls
	*@return true if success,false if failure
	*/
	bool update_texture_res();
private:
	void output_primitive_to_file();

	void load_primitive_from_file(string &kname, vector<GLubyte> ele_format, GLuint vbo_len, GLuint ebo_len);
};

