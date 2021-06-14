#include "ImGuiCustom.h"

#include "ImGuiLayer.h"

#include "imgui_internal.h"

namespace ImGui
{
	bool TextureButton(const std::string& texture_name, ImVec2 size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
	{
		auto tex = ((ImGuiLayer*)ImGui::GetIO().UserData)->GetTexture(texture_name);
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
		v2f tex_size = tex->GetSize();
		u32 w, h;
		tex->GetPow2Size(w, h);
		auto mult = v2f(tex_size.x / w, tex_size.y / h);
		if (size.x == 0) size.x = tex_size.x;
		if (size.y == 0) size.y = tex_size.y;
		ImGui::Image((ImTextureID)tex, size, v2f(uv0)*mult, v2f(uv1)*mult, tint_col, border_col);
	}

	void CenterWidget(float widget_width)
	{
		auto centered_x = (ImGui::GetWindowWidth() - widget_width) / 2;
		ImGui::Dummy(v2f(0, 0)); ImGui::SameLine(centered_x);
	}

	void CenterText(const std::string& txt, bool wrapped)
	{
		CenterWidget(ImGui::CalcTextSize(txt.c_str()).x);
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
		return (ImGui::GetWindowWidth() - ImGui::GetColumnOffset()) / count - ImGui::GetStyle().ItemSpacing.x;
	}
	void Label(const std::string& txt)
	{
		ImGui::GetWindowDrawList()->AddText(v2f(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x, ImGui::GetCursorPosY() /*+ ImGui::GetStyle().FramePadding.y*/ + ImGui::GetStyle().ItemSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), txt.c_str());
		ImGui::Dummy(ImGui::CalcTextSize(txt.c_str()));
	}
	void ButtonLabel(const std::string& txt, v2f size)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
		ImGui::ButtonEx(txt.c_str(), size, ImGuiButtonFlags_Disabled);
		ImGui::PopStyleVar();
	}
	bool ButtonWithLabel(const std::string& label, const std::string txt, float label_width, v2f size)
	{
		bool b = ImGui::ButtonEx(label.c_str(), v2f(label_width, size.y), ImGuiButtonFlags_Disabled);
		ImGui::SameLine(0, 0);
		b = ImGui::Button(txt.c_str(), size) || b;
		return b;
	}

	void Strikethrough(float offset_before, float offset_after, ImColor color)
	{
		auto rect_min = ImGui::GetItemRectMin();
		auto rect_max = ImGui::GetItemRectMax();
		v2f from{ rect_min.x + offset_before, (rect_min.y + rect_max.y) / 2 };
		v2f to{ rect_max.x + offset_after, from.y };
		ImGui::GetWindowDrawList()->AddLine(from, to, color);
	}
	

}