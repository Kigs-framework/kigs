#pragma once

#include "kstlstring.h"

struct lua_State;

namespace ImGui
{
	enum class DockStatus
	{
		Docked,
		Float,
		Dragged
	};
IMGUI_API void InitDock();
IMGUI_API void ShutdownDock();
IMGUI_API void RootDock(const ImVec2& pos, const ImVec2& size);
IMGUI_API bool BeginDock(const char* label, bool* opened = nullptr, ImGuiWindowFlags extra_flags = 0, const ImVec2& default_size = ImVec2(-1, -1), DockStatus* status = nullptr);
IMGUI_API void EndDock();
IMGUI_API void SetDockActive();
IMGUI_API void SaveDock(const kstl::string& file);
IMGUI_API void LoadDock(lua_State* L);


} // namespace ImGui