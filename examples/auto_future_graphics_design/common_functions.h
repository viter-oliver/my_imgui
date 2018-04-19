#pragma once
#include "imgui.h"
#include <string>
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif
extern ImVec2 rotate_point_by_zaxis(ImVec2& tar, float agl, ImVec2& basePoint);
extern bool prepareFBO1(GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId, GLuint frame_width, GLuint frame_height);
extern bool af_prepareFBO1(GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId, GLuint frame_width, GLuint frame_height);
extern bool prepareFBO2(GLuint& textId, GLuint& fboId, GLuint frame_width, GLuint frame_height);