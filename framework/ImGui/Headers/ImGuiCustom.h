#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "CoreSTL.h"

namespace ImGui
{
	bool TextureButton(const std::string& texture_name, ImVec2 size = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
	void Texture(const std::string& texture_name, ImVec2 size = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	void CenterWidget(float widget_width);
	void CenterText(const std::string& txt, bool wrapped=false);

	void TextMiddleAligned(const std::string& txt, v2f size);
	bool ButtonCenteredText(const std::string& txt, v2f size_arg);

	// Align the next widget from the right of the window, with enough space for "nb_of_elements" (to account for spacing between elements) totaling "sum_of_elements_width" of size
	// Returns the width available between current cursor pos and the right aligned element
	float SameLineAlignRight(float sum_of_elements_width, u32 nb_of_elements = 1);
	float GetRemainingWidth(float sum_of_elements_width, u32 nb_of_elements = 1);

	// Get the size one element should have to fit "count" of them in a single line perfectly
	float GetElementWidthForSubdivision(int count);

	void Label(const std::string& txt);

	void ButtonLabel(const std::string& txt, v2f size = v2f(0, 0));
	bool ButtonWithLabel(const std::string& label, const std::string txt, float label_width = 0.0f, v2f size = v2f(0, 0));

	void Strikethrough(float offset_before = 0.0f, float offset_after = 0.0f, ImColor color = ImColor(0, 0, 0, 255), int width = 1);

	inline bool ButtonSTD(const std::string& label, const v2f& size = v2f(0,0))
	{
		return ImGui::Button(label.c_str(), size);
	}

	bool ButtonCenteredSTD(const std::string& label, v2f size = v2f(0, 0));
	
	inline void TextSTD(const std::string& text)
	{
		ImGui::Text(text.c_str());
	}

	inline void SetTooltipSTD(const std::string& text)
	{
		ImGui::SetTooltip(text.c_str());
	}

	template<auto F, typename ... Args>
	inline auto FlowLayout(const std::string& str, Args&&... args)
	{
		auto window = ImGui::GetCurrentWindow();
		auto remaining = ImGui::GetContentRegionMaxAbs().x - window->DC.CursorPosPrevLine.x;
		remaining -= ImGui::GetStyle().ItemSpacing.x;
		remaining -= ImGui::GetStyle().FramePadding.x * 2;
		if (ImGui::CalcTextSize(str.c_str(), 0, true).x < remaining)
			ImGui::SameLine();

		return F(str.c_str(), FWD(args)...);
	}

	bool ToggleButton(const char* str_id, bool* v, bool reverse_side = false);

	void ScrollWhenDragging(const ImVec2& mult, float threshold = 96.0f, ImGuiMouseButton mouse_button = ImGuiMouseButton_Left);

	inline void PushStyleSpacing(v2f spacing)
	{
		if (spacing.x < 0.0f) spacing.x = ImGui::GetStyle().ItemSpacing.x;
		if (spacing.y < 0.0f) spacing.y = ImGui::GetStyle().ItemSpacing.y;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);
	}
	
	void PushSaturatedButton(float mult = 1.25f);
	void PopSaturatedButton();

	bool IsEnterPressed();
}

std::string CheckButtonText(bool checked, const std::string& txt, bool before = true);
std::string SelectedOptionText(bool selected, const std::string& txt, bool before = true);

v3f RGBtoHSV(v3f rgb);
v3f HSVtoRGB(v3f hsv);