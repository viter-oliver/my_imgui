#include "ft_scene3d.h"
#include "common_functions.h"

ft_scene3d::ft_scene3d()
	:ft_base(), _texture_index(0),
	_size(800.f, 600.f),
	_fboId(0), _colorTextId(0), _depthStencilTextId(0)
{
	prepareFBO1(_colorTextId, _depthStencilTextId, _fboId, _size.x, _size.y);
}
ft_scene3d::ft_scene3d(ft_scene3d& bsource)
	:ft_base(bsource)
{
	//prepareFBO1(_colorTextId, _depthStencilTextId, _fboId, screenw, screenh);
}


ft_scene3d::~ft_scene3d()
{
}

void ft_scene3d::draw()
{
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
	glViewport(0, 0, _size.x, _size.y);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_SCISSOR_TEST);
	glClearColor(0.2f, 0.2f, 0.5f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	ft_base::draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	ImVec3 abpos = absolute_coordinate_of_base_pos();
	ImVec2 winpos = ImGui::GetWindowPos();
	float sizew = _size.x;
	float sizeh = _size.y;
	ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
	ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
	ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
	ImVec2 pos4 = { pos1.x + sizew, pos1.y };
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, _colorTextId);
	ImGui::ImageQuad((ImTextureID)_colorTextId, pos1, pos2, pos3, pos4, ImVec2(0, 0), ImVec2(0, 1), ImVec2(1, 1), ImVec2(1, 0));
#if !defined(IMGUI_WAYLAND)
	if (is_selected())//draw envelope
	{
		ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
		ImVec2 editunit(edit_unit_len, edit_unit_len);
		ImVec2 pos1a = pos1 - editunit;
		ImVec2 pos1b = pos1 + editunit;
		ImGui::RenderFrame(pos1a, pos1b, col);
		ImVec2 pos2a = pos2 - editunit;
		ImVec2 pos2b = pos2 + editunit;
		ImGui::RenderFrame(pos2a, pos2b, col);
		ImVec2 pos3a = pos3 - editunit;
		ImVec2 pos3b = pos3 + editunit;
		ImGui::RenderFrame(pos3a, pos3b, col);
		ImVec2 pos4a = pos4 - editunit;
		ImVec2 pos4b = pos4 + editunit;
		ImGui::RenderFrame(pos4a, pos4b, col);
	}
#endif
}
#if !defined(IMGUI_WAYLAND)
void ft_scene3d::draw_peroperty_page()
{
	ft_base::draw_peroperty_page();
}
#endif

bool ft_scene3d::init_from_json(Value& jvalue)
{
	return true;
}

bool ft_scene3d::handle_mouse()
{
	return true;
}

