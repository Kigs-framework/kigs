#pragma once

namespace Kigs
{
	namespace Tools
	{
#ifndef KIGS_TOOLS_IMPL
		extern bool gKigsToolsAvailable;
#endif

#ifdef KIGS_TOOLS
		constexpr bool KIGS_TOOLS_DEFINED = true;
#else
		constexpr bool KIGS_TOOLS_DEFINED = false;
#endif

		void ShowKigsTools(bool show);
		bool UpdateKigsTools();
		void DestroyKigsTools();
		void SelectObjectKigsTools(Kigs::Core::CoreModifiable* obj);

		void RegisterWidget(const std::string& id, std::function<void()> draw_function);
	}
}