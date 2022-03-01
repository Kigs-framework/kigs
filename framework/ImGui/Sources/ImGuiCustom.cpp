#include "ImGuiCustom.h"

#include "ImGuiLayer.h"
#include "IconsForkAwesome.h"

#include "imgui_internal.h"

namespace ImGui
{
	bool TextureButton(const std::string& texture_name, ImVec2 size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
	{
		auto tex = ((ImGuiLayer*)ImGui::GetIO().UserData)->GetTexture(texture_name);
		tex->LoadtoGPUIfNeeded();
		v2f tex_size = tex->GetSize();
		u32 w, h;
		tex->GetPow2Size(w, h);
		auto mult = v2f(tex_size.x / w, tex_size.y / h);
		if (size.x == 0) size.x = tex_size.x;
		if (size.y == 0) size.y = tex_size.y;
		return ImGui::ImageButton((ImTextureID)tex, size, v2f(uv0)*mult, v2f(uv1)*mult, frame_padding, bg_col, tint_col);
	}

	void Texture(const std::string& texture_name, ImVec2 size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		auto tex = ((ImGuiLayer*)ImGui::GetIO().UserData)->GetTexture(texture_name);
		tex->LoadtoGPUIfNeeded();
		v2f tex_size = tex->GetSize();
		u32 w, h;
		tex->GetPow2Size(w, h);
		auto mult = v2f(tex_size.x / w, tex_size.y / h);
		if (size.x == 0 && size.y == 0)
		{
			size.x = tex_size.x;
			size.y = tex_size.y;
		}
		else if (size.x == 0)
		{
			size.x = tex_size.x * size.y / tex_size.y;
		}
		else if (size.y == 0)
		{
			size.y = tex_size.y * size.x / tex_size.x;
		}
		ImGui::Image((ImTextureID)tex, size, v2f(uv0)*mult, v2f(uv1)*mult, tint_col, border_col);
	}

	void CenterWidget(float widget_width)
	{
		auto centered_x = (ImGui::GetWindowWidth() - widget_width) / 2;
		ImGui::Dummy(v2f(0, 0)); ImGui::SameLine(centered_x);
	}

	void CenterText(const std::string& txt, bool wrapped)
	{
		const float wrap_width = wrapped ? ImGui::CalcWrapWidthForPos(ImGui::GetCursorScreenPos(), 0.0f) : -1.0f;
		CenterWidget(ImGui::CalcTextSize(txt.c_str(), 0, false, wrap_width).x);
		if(!wrapped) ImGui::Text(txt.c_str());
		else ImGui::TextWrapped(txt.c_str());
	}
	float SameLineAlignRight(float sum_of_elements_width, u32 nb_of_elements)
	{
		auto w = ImGui::GetWindowWidth() - sum_of_elements_width - (nb_of_elements-1) * ImGui::GetStyle().ItemSpacing.x - ImGui::GetColumnOffset();
		ImGui::SameLine(w);
		return w - ImGui::GetStyle().ItemSpacing.x;
	}
	float GetRemainingWidth(float sum_of_elements_width, u32 nb_of_elements)
	{
		auto w = ImGui::GetWindowWidth() - sum_of_elements_width - (nb_of_elements - 1) * ImGui::GetStyle().ItemSpacing.x - ImGui::GetColumnOffset();
		return w - ImGui::GetStyle().ItemSpacing.x;
	}
	float GetElementWidthForSubdivision(int count)
	{
		if (count <= 0) count = 1;
		return (ImGui::GetWindowWidth() - ImGui::GetColumnOffset() - ImGui::GetCurrentWindow()->ScrollbarSizes.x) / count - ImGui::GetStyle().ItemSpacing.x;
	}
	void Label(const std::string& txt)
	{
		ImGui::GetWindowDrawList()->AddText(v2f(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x, ImGui::GetCursorPosY() /*+ ImGui::GetStyle().FramePadding.y*/ + ImGui::GetStyle().ItemSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), txt.c_str());
		ImGui::Dummy(ImGui::CalcTextSize(txt.c_str()));
	}
	void ButtonLabel(const std::string& txt, v2f size)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
		ImGui::PushDisabled();
		ImGui::ButtonEx(txt.c_str(), size);
		ImGui::PopDisabled();
		ImGui::PopStyleVar();
	}
	bool ButtonWithLabel(const std::string& label, const std::string txt, float label_width, v2f size)
	{
		ImGui::PushDisabled();
		bool b = ImGui::ButtonEx(label.c_str(), v2f(label_width, size.y));
		ImGui::PopDisabled();
		ImGui::SameLine(0, 0);
		b = ImGui::Button(txt.c_str(), size) || b;
		return b;
	}
	bool ButtonCenteredSTD(const std::string& label, v2f size)
	{
		auto label_size = ImGui::CalcTextSize(label.c_str());
		ImVec2 item_size = CalcItemSize(size, label_size.x + ImGui::GetStyle().FramePadding.x * 2.0f, label_size.y + ImGui::GetStyle().FramePadding.y * 2.0f);
		ImGui::CenterWidget(item_size.x);
		return ImGui::Button(label.c_str(), size);
	}

	void Strikethrough(float offset_before, float offset_after, ImColor color, int width)
	{
		auto rect_min = ImGui::GetItemRectMin();
		auto rect_max = ImGui::GetItemRectMax();
		v2f from{ rect_min.x + offset_before, (rect_min.y + rect_max.y) / 2 };
		v2f to{ rect_max.x + offset_after, from.y };
		for (int i = 0; i < width; ++i)
		{
			ImGui::GetWindowDrawList()->AddLine(from + v2f(0, i - width/2), to + v2f(0, i - width/2), color);
		}
	}

	bool ToggleButton(const char* str_id, bool* v)
	{
		bool changed = false;

		v2f p = ImGui::GetCursorScreenPos();
		p.y += 1;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		float border_size = ImGui::GetStyle().ChildBorderSize / 2;
		float height = ImGui::GetFrameHeight() - 2 * border_size;
		float width = height * 1.55f;
		float radius = height * 0.50f;

		changed = ImGui::InvisibleButton(str_id, ImVec2(width, height));
		if(changed)
			*v = !*v;
		
		ImColor col_bg = ImGui::IsItemHovered() ? ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered) : ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
		ImColor circle_color = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
		ImColor border_color = ImGui::GetStyleColorVec4(ImGuiCol_Separator);
		
		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), border_color, height * 0.5f);
		draw_list->AddRectFilled(p + v2f(border_size, border_size), ImVec2(p.x + width - border_size, p.y + height - border_size), ImColor(ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg)), (height - 2 * border_size) * 0.5f);
		draw_list->AddRectFilled(p + v2f(border_size, border_size), ImVec2(p.x + width - border_size, p.y + height - border_size), col_bg, (height - 2 * border_size) * 0.5f);
		draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, border_color);
		draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f - border_size, circle_color);
		
		return changed;
	}

	void ScrollWhenDragging(const ImVec2& mult, float threshold, ImGuiMouseButton mouse_button)
	{
		ImVec2 delta = -ImGui::GetIO().MouseDelta;

		ImGuiContext& g = *ImGui::GetCurrentContext();
		ImGuiWindow* window = g.CurrentWindow;
		//if (g.HoveredId == 0) // If nothing hovered so far in the frame (not same as IsAnyItemHovered()!)
		//static float cumulative_delta = 0.0f;

		bool clicked_in_scrollbar = (window->ScrollbarY && ImGui::GetWindowScrollbarRect(window, ImGuiAxis_Y).Contains(ImGui::GetIO().MouseClickedPos[mouse_button]))
			|| (window->ScrollbarX && ImGui::GetWindowScrollbarRect(window, ImGuiAxis_X).Contains(ImGui::GetIO().MouseClickedPos[mouse_button]));

		bool held = ImGui::IsMouseDown(mouse_button) && !clicked_in_scrollbar;

		float cumulative_delta = std::sqrtf(ImGui::GetIO().MouseDragMaxDistanceSqr[mouse_button]);
		if (!held)
		{
			cumulative_delta = 0.0f;
		}

		if (held && delta.x != 0.0f)
		{
			if (cumulative_delta > threshold * 0.75f)
				ImGui::SetScrollX(window, window->Scroll.x + delta.x * mult.x);
			cumulative_delta += std::abs(delta.x);
		}
		if (held && delta.y != 0.0f)
		{
			if (cumulative_delta > threshold * 0.75f)
				ImGui::SetScrollY(window, window->Scroll.y + delta.y * mult.y);
			cumulative_delta += std::abs(delta.y);
		}
		if (cumulative_delta > threshold)
		{
			ImGui::ClearActiveID();
			ImGui::SetHoveredID(ImGui::GetID("ScrollID"));
		}
	}


	void TextMiddleAligned(const std::string& txt, v2f size)
	{
		auto draw_list = ImGui::GetWindowDrawList();

		const char* text_start = txt.c_str();
		const char* text_end = txt.c_str() + txt.size();
		std::vector<std::string_view> lines;
		while (text_start < text_end)
		{
			auto wrap_pos = draw_list->_Data->Font->CalcWordWrapPositionA(1.0f, text_start, text_end, size.x - ImGui::GetStyle().FramePadding.x * 2);
			lines.push_back({ text_start, wrap_pos });
			text_start = wrap_pos + 1;
		}
		v2f pos = (v2f)ImGui::GetCursorScreenPos() + size / 2;

		pos.y -= ((lines.size() - 1) * ImGui::GetTextLineHeightWithSpacing()) / 2;
		for (auto line : lines)
		{
			v2f line_size = ImGui::CalcTextSize(line.data(), line.data() + line.size());
			draw_list->AddText(pos - line_size / 2, ImGui::GetColorU32(ImGuiCol_Text), line.data(), line.data() + line.size());
			pos.y += ImGui::GetTextLineHeightWithSpacing();
		}
	}


	bool ButtonCenteredText(const std::string& txt, v2f size_arg)
	{
		auto label_size = CalcTextSize(txt.c_str());
		auto& style = ImGui::GetStyle();
		ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
		
		auto p1 = ImGui::GetCursorPos();
		auto ret = ImGui::ButtonSTD("##" + txt, size);
		auto p2 = ImGui::GetCursorPos();
		ImGui::SetCursorPos(p1);
		ImGui::TextMiddleAligned(txt, size);
		ImGui::SetCursorPos(p2);
		return ret;
	};

}

std::string CheckButtonText(bool checked, const std::string& txt, bool before)
{
	if (before)
		return checked ? ICON_FK_CHECK_SQUARE " " + txt : ICON_FK_SQUARE_O " " + txt;

	return checked ? txt + " " ICON_FK_CHECK_SQUARE : txt + ICON_FK_SQUARE_O" ";
}

std::string SelectedOptionText(bool selected, const std::string& txt, bool before)
{
	if (before)
		return selected ? ICON_FK_CHECK" " + txt : " " + txt;

	return selected ? txt + " " ICON_FK_CHECK : txt + " ";
}