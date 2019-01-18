#pragma once
#include "imgui.h"
#include "res_output.h"
#include <functional>
#include <string>
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif
extern std::string wstringToUtf8(const std::wstring& str);
extern bool handle_file_data(const std::string& file_name, std::function<void(char*, unsigned int)>buff_handle);
extern std::wstring utf8ToWstring(const std::string& str);
extern GLfloat* get_txt_uvs(const char* data_file, int& retn_len);
extern void get_txt_uv_vector(const char* data_file, vres_txt_cd& vtxt_cd);
extern ImVec2 rotate_point_by_zaxis(ImVec2& tar, float agl, ImVec2& basePoint);
extern bool prepareFBO1(GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId, GLuint frame_width, GLuint frame_height);
extern bool af_prepareFBO1(GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId, GLuint frame_width, GLuint frame_height);
extern bool prepareFBO2(GLuint& textId, GLuint& fboId, GLuint frame_width, GLuint frame_height);
extern bool fileExist(const char* fileName);
extern bool directoryExist(const char* dir);
extern bool createDirectory(const char* pathName);
extern bool createFileWithDirectory(const char* pathName);
