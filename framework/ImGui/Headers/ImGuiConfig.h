#pragma once
#include <imgui.h>
//#define FORCE_REMOTE_IMGUI

#if (defined _M_ARM || defined FORCE_REMOTE_IMGUI || defined WUP) && false
extern bool gIsHolographic;
#define gImGuiRemoteAvailable true
#define IMGUI_WEBBY_REMOTE 1
#else
#define IMGUI_WEBBY_REMOTE 0
#define gImGuiRemoteAvailable false

namespace ImGui
{
	inline void RemoteInit(const char*,int){}
	inline void RemoteUpdate(){}
	struct RemoteInput
	{
		ImVec2	MousePos;
		int		MouseButtons;
		float	MouseWheelDelta;
		bool	KeyCtrl;
		bool	KeyShift;
		bool	KeysDown[256];
	};
	inline bool RemoteGetInput(RemoteInput&) { return false; }
	inline bool RemoteDraw(ImDrawList** const, int) { return false; }
}
#endif