#include "ft_scene.h"
#include "common_functions.h"
namespace auto_future
{
	ft_scene::ft_scene()
		:ft_base()
		, _fboId(0), _colorTextId(0), _depthStencilTextId(0)
	{
		_sn_pt._bk_cl = { 0.2f, 0.2f, 0.5f, 0.5f };
		prepareFBO1(_colorTextId, _depthStencilTextId, _fboId, _sn_pt._sizew, _sn_pt._sizeh);
	}

	ft_scene::~ft_scene()
	{
	}

	void ft_scene::draw()
	{
		GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
		glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
		glViewport(0, 0, _sn_pt._sizew, _sn_pt._sizeh);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glEnable(GL_CULL_FACE);
		//glDisable(GL_SCISSOR_TEST);
		glClearColor(_sn_pt._bk_cl.x, _sn_pt._bk_cl.y, _sn_pt._bk_cl.z, _sn_pt._bk_cl.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		ft_base::draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		float sizew = _sn_pt._sizew;
		float sizeh = _sn_pt._sizeh;
		ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, _colorTextId);
		ImGui::ImageQuad((ImTextureID)_colorTextId, pos1, pos2, pos3, pos4, ImVec2(0, 0), ImVec2(0, 1), ImVec2(1, 1), ImVec2(1, 0));
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
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
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void ft_scene::draw_peroperty_page(int property_part)
	{
		ft_base::draw_peroperty_page();
		ImGui::Text("Size:");
		ImGui::SliderFloat("###w", &_sn_pt._sizew, 0.f, base_ui_component::screenw);
		ImGui::SliderFloat("###h", &_sn_pt._sizeh, 0.f, base_ui_component::screenw);
		ImGui::Text("Background color:");
		ImGui::ColorEdit4("text color:", (float*)&_sn_pt._bk_cl);

	}

	bool ft_scene::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		Value& jsize = jvalue["size"];
		_sn_pt._sizew = jsize["w"].asDouble();
		_sn_pt._sizeh = jsize["h"].asDouble();
		Value& jbgcolor = jvalue["background color"];
		_sn_pt._bk_cl.x = jbgcolor["x"].asDouble();
		_sn_pt._bk_cl.y = jbgcolor["y"].asDouble();
		_sn_pt._bk_cl.z = jbgcolor["z"].asDouble();
		_sn_pt._bk_cl.w = jbgcolor["w"].asDouble();

		return true;
	}

	bool ft_scene::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		Value jsize(objectValue);
		jsize["w"] = _sn_pt._sizew;
		jsize["h"] = _sn_pt._sizeh;
		junit["size"] = jsize;
		Value jbgcolor(objectValue);
		jbgcolor["x"] = _sn_pt._bk_cl.x;
		jbgcolor["y"] = _sn_pt._bk_cl.y;
		jbgcolor["z"] = _sn_pt._bk_cl.z;
		jbgcolor["w"] = _sn_pt._bk_cl.w;
		junit["background color"] = jbgcolor;
		return true;
	}
#endif

	bool ft_scene::handle_mouse()
	{
		return true;
	}
}
