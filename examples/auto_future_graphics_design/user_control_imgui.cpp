#include "res_internal.h"
#include "user_control_imgui.h"

//#include <functional>
bool IconTreeNode(string& icon_name,const char* label,  ImGuiTreeNodeFlags flags,const char* postfix_icon)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;

	if (window->SkipItems)
		return false;

     return IconTreeNodeBehavior( window->GetID( label ), flags, icon_name, label, postfix_icon );
}

bool IconTreeNodeBehavior( ImGuiID id, ImGuiTreeNodeFlags flags, string& icon_name, const char* label, const char* postfix_icon )
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	const ImGuiStyle& style = g.Style;
	const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
	const ImVec2 padding = (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, 0.0f);

	
	const char* label_end = ImGui::FindRenderedTextEnd(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, label_end, false);

	// We vertically grow up to current line height up the typical widget height.
	const float text_base_offset_y = ImMax(padding.y, window->DC.CurrentLineTextBaseOffset); // Latch before ItemSize changes it
	const float frame_height = ImMax(ImMin(window->DC.CurrentLineHeight, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y * 2);
	ImRect frame_bb = ImRect(window->DC.CursorPos, ImVec2(window->Pos.x + ImGui::GetContentRegionMax().x, window->DC.CursorPos.y + frame_height));
	if (display_frame)
	{
		// Framed header expand a little outside the default padding
		frame_bb.Min.x -= (float)(int)(window->WindowPadding.x*0.5f) - 1;
		frame_bb.Max.x += (float)(int)(window->WindowPadding.x*0.5f) - 1;
	}

	float text_offset_x = (g.FontSize + (display_frame ? padding.x * 3 : padding.x * 2));   // Collapser arrow width + Spacing
	const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);   // Include collapser
	ImGui::ItemSize(ImVec2(text_width, frame_height), text_base_offset_y);

	// For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
	// (Ideally we'd want to add a flag for the user to specify if we want the hit test to be done up to the right side of the content or not)
	const ImRect interact_bb = display_frame ? frame_bb : ImRect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + text_width + style.ItemSpacing.x * 2, frame_bb.Max.y);
	bool is_open = ImGui::TreeNodeBehaviorIsOpen(id, flags);

	// Store a flag for the current depth to tell if we will allow closing this node when navigating one of its child. 
	// For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
	// This is currently only support 32 level deep and we are fine with (1 << Depth) overflowing into a zero.
	if (is_open && !g.NavIdIsAlive && (flags & ImGuiTreeNodeFlags_NavLeftJumpsBackHere) && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		window->DC.TreeDepthMayJumpToParentOnPop |= (1 << window->DC.TreeDepth);

	bool item_add = ImGui::ItemAdd(interact_bb, id);
	window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
	window->DC.LastItemDisplayRect = frame_bb;

	if (!item_add)
	{
		if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			ImGui::TreePushRawID(id);
		return is_open;
	}

	// Flags that affects opening behavior:
	// - 0(default) ..................... single-click anywhere to open
	// - OpenOnDoubleClick .............. double-click anywhere to open
	// - OpenOnArrow .................... single-click on arrow to open
	// - OpenOnDoubleClick|OpenOnArrow .. single-click on arrow or double-click anywhere to open
	ImGuiButtonFlags button_flags = ImGuiButtonFlags_NoKeyModifiers | ((flags & ImGuiTreeNodeFlags_AllowItemOverlap) ? ImGuiButtonFlags_AllowItemOverlap : 0);
	if (!(flags & ImGuiTreeNodeFlags_Leaf))
		button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;
	if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
		button_flags |= ImGuiButtonFlags_PressedOnDoubleClick | ((flags & ImGuiTreeNodeFlags_OpenOnArrow) ? ImGuiButtonFlags_PressedOnClickRelease : 0);

	bool hovered, held, pressed = ImGui::ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);
	if (!(flags & ImGuiTreeNodeFlags_Leaf))
	{
		bool toggled = false;
		if (pressed)
		{
			toggled = !(flags & (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) || (g.NavActivateId == id);
			if (flags & ImGuiTreeNodeFlags_OpenOnArrow)
				toggled |= ImGui::IsMouseHoveringRect(interact_bb.Min, ImVec2(interact_bb.Min.x + text_offset_x, interact_bb.Max.y)) && (!g.NavDisableMouseHover);
			if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
				toggled |= g.IO.MouseDoubleClicked[0];
			if (g.DragDropActive && is_open) // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but never close it again.
				toggled = false;
		}

		if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Left && is_open)
		{
			toggled = true;
			ImGui::NavMoveRequestCancel();
		}
		if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Right && !is_open) // If there's something upcoming on the line we may want to give it the priority?
		{
			toggled = true;
			ImGui::NavMoveRequestCancel();
		}

		if (toggled)
		{
			is_open = !is_open;
			window->DC.StateStorage->SetInt(id, is_open);
		}
	}
	if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
		ImGui::SetItemAllowOverlap();
	
	//icon pos
	ImVec2 icon_pos = frame_bb.Min;
	icon_pos.x += 20;
	struct nestfun
	{
		ImVec2& _basepos;
		string& _iconname;
          const char*  _icon_postfix = {NULL};
		float& _txtxoffset;
		ImVec2 pos1, pos2, pos3, pos4;
		ImVec2 uv1, uv2, uv3, uv4;
		bool will_draw;
		nestfun(ImVec2& bpos,string&icname,float& txtxof,const char* icon_postfix)
			:_basepos(bpos), _iconname(icname), _txtxoffset(txtxof),_icon_postfix(icon_postfix), will_draw(false)
		{
               float w = 18.0f;
               float h = 18.0f;
			auto itxt_unit = g_mtxt_intl.find(_iconname);
			if (itxt_unit != g_mtxt_intl.end())
			{
				will_draw = true;
				auto& txt_unit = itxt_unit->second;
				pos1 = _basepos;
				pos2 = { pos1.x, pos1.y + h };
				pos3 = { pos1.x + w, pos2.y };
				pos4 = { pos3.x, pos1.y };

				uv1 = ImVec2(txt_unit._x0 / g_txt_width_intl, txt_unit._y0 / g_txt_height_intl);
				uv2 = ImVec2(txt_unit._x0 / g_txt_width_intl, (txt_unit._y1) / g_txt_height_intl);
				uv3 = ImVec2((txt_unit._x1) / g_txt_width_intl, (txt_unit._y1) / g_txt_height_intl);
				uv4 = ImVec2((txt_unit._x1) / g_txt_width_intl, (txt_unit._y0) / g_txt_height_intl);
				_txtxoffset += w;
                    
			}	
               if( icon_postfix )
               {
                    _txtxoffset += w;
               }
		}
		void operator()()
		{
			if (will_draw)
			{
				ImGui::ImageQuad((ImTextureID)g_txt_id_intl, pos1, pos2, pos3, pos4, uv1, uv2, uv3, uv4);
			}
               if( _icon_postfix )
               {
                    auto itxt_unit = g_mtxt_intl.find( _icon_postfix );
                    if( itxt_unit != g_mtxt_intl.end() )
                    {
                         auto& txt_unit = itxt_unit->second;
                         float w = 18.0f;
                         float h = 18.0f;
                         pos1 = _basepos;
                         if (will_draw)
                         {
                              pos1.x += w;
                         }
                         pos2 = { pos1.x, pos1.y + h };
                         pos3 = { pos1.x + w, pos2.y };
                         pos4 = { pos3.x, pos1.y };

                         uv1 = ImVec2( txt_unit._x0 / g_txt_width_intl, txt_unit._y0 / g_txt_height_intl );
                         uv2 = ImVec2( txt_unit._x0 / g_txt_width_intl, ( txt_unit._y1 ) / g_txt_height_intl );
                         uv3 = ImVec2( ( txt_unit._x1 ) / g_txt_width_intl, ( txt_unit._y1 ) / g_txt_height_intl );
                         uv4 = ImVec2( ( txt_unit._x1 ) / g_txt_width_intl, ( txt_unit._y0 ) / g_txt_height_intl );
                         ImGui::ImageQuad( (ImTextureID)g_txt_id_intl, pos1, pos2, pos3, pos4, uv1, uv2, uv3, uv4 );
                    }

               }
		}
     } tmpfun( icon_pos, icon_name, text_offset_x, postfix_icon );

	// Render
	const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
	const ImVec2 text_pos = frame_bb.Min + ImVec2(text_offset_x, text_base_offset_y);
	ImVec2 cursorpos=window->DC.CursorPos;
	if (display_frame)
	{
		// Framed type
		ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, col, true, style.FrameRounding);
		ImGui::RenderNavHighlight(frame_bb, id, ImGuiNavHighlightFlags_TypeThin);
		ImGui::RenderArrow(frame_bb.Min + ImVec2(padding.x, text_base_offset_y), is_open ? ImGuiDir_Down : ImGuiDir_Right, 1.0f);
		tmpfun();
		ImGui::RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
	}
	else
	{
		// Unframed typed for tree nodes
		if (hovered || (flags & ImGuiTreeNodeFlags_Selected))
		{
			ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, col, false);
			ImGui::RenderNavHighlight(frame_bb, id, ImGuiNavHighlightFlags_TypeThin);
		}

		if (flags & ImGuiTreeNodeFlags_Bullet)
			ImGui::RenderBullet(frame_bb.Min + ImVec2(text_offset_x * 0.5f, g.FontSize*0.50f + text_base_offset_y));
		else if (!(flags & ImGuiTreeNodeFlags_Leaf))
			ImGui::RenderArrow(frame_bb.Min + ImVec2(padding.x, g.FontSize*0.15f + text_base_offset_y), is_open ? ImGuiDir_Down : ImGuiDir_Right, 0.70f);
		tmpfun();
		ImGui::RenderText(text_pos, label, label_end, false);
	}
	window->DC.CursorPos = cursorpos;
	window->DC.CursorPos.y += 5;
	if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		ImGui::TreePushRawID(id);
	return is_open;
}
