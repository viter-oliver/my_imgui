#include "bind_edit.h"
#include "user_control_imgui.h"
#include "res_output.h"
#include <GLFW/glfw3.h>


void bind_edit::bind_source_view()
{
	static char text[1024 * 16] =
		"/*\n"
		" The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
		" the hexadecimal encoding of one offending instruction,\n"
		" more formally, the invalid operand with locked CMPXCHG8B\n"
		" instruction bug, is a design flaw in the majority of\n"
		" Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
		" processors (all in the P5 microarchitecture).\n"
		"*/\n\n"
		"label:\n"
		"\tlock cmpxchg8b eax\n";
	ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
}