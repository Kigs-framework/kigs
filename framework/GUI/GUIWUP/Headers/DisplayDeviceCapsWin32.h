#pragma once

#include "DisplayDeviceCaps.h"

namespace Kigs
{
	namespace Gui
	{
		// ****************************************
		// * DisplayDeviceCapsWin32 class
		// * --------------------------------------
		/**
		* \file	DisplayDeviceCapsWin32.h
		* \class	DisplayDeviceCapsWin32
		* \ingroup GUIModule
		* \brief	Win32 UWP display device capacities.
		*/
		// ****************************************

		class DisplayDeviceCapsWin32 : public DisplayDeviceCaps
		{
		public:
			DECLARE_CLASS_INFO(DisplayDeviceCapsWin32, DisplayDeviceCaps, GUI)
				DisplayDeviceCapsWin32(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
		};

	}
}