#pragma once
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
extern bool IconTreeNode(string& icon_name, const char* label, ImGuiTreeNodeFlags flags);
extern bool IconTreeNodeBehavior(ImGuiID id, ImGuiTreeNodeFlags flags,string& icon_name, const char* label, const char* label_end);
