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
extern void align_expression(string& exp, string&expo);
extern void trim_align_expression(string& exp, string&expo);
extern void convert_binary_to_string(char* pbin, int len, string& out_str);
extern void convert_string_to_binary(string& in_str, string& out_bin);
extern unsigned int conver_track_buff_to_pair(char* pbuff,unsigned int buff_len, vector<ImVec2>& vtrack0, vector<ImVec2>& vtrack1);
//5点3次平滑法
extern bool smooth_algorithm_5_points_3_times(vector<ImVec2>& point_list, bool x_direction = true);
//直线3点平均平滑法
extern bool smooth_algoritm_3_points_average(vector<ImVec2>& point_list, bool x_direction = true);
//直线5点平均平滑法
extern bool smooth_algorithm_5_points_average(vector<ImVec2>& point_list, bool x_direction = true);
extern void FromYUY2ToRGB32(LPVOID lpDest, LPVOID lpSource, LONG lWidth, LONG lHeight);

template<class T> string find_a_key_from_mp(T& mp, string& ref_key)
{
	string chk_key = ref_key;
	while (mp.find(chk_key) != mp.end())
	{
		chk_key += "#";

	}
	return chk_key;
}
