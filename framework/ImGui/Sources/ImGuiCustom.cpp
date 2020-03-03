#include "ImGuiCustom.h"

#include "ImGuiLayer.h"

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
		ImGui::Dummy(v2f(centered_x, 0)); ImGui::SameLine();
	}
}