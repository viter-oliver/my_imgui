#pragma once
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
bool MyTreeNodeBehavior(ImGuiID id, ImGuiTreeNodeFlags flags,const char* icon_name, const char* label, const char* label_end);
