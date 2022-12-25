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
		* \brief	Specific Win32 DisplayDeviceCaps, used to get display device list and capacities.
		*/
		// ****************************************
		class DisplayDeviceCapsWin32 : public DisplayDeviceCaps
		{
		public:
			DECLARE_CLASS_INFO(DisplayDeviceCapsWin32, DisplayDeviceCaps, GUI)

				//! constructor
				DisplayDeviceCapsWin32(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			//! destructor
			virtual ~DisplayDeviceCapsWin32();

			bool SupportWindowedMode() override
			{
				return true; // enable creation of several windows on Win32
			}
		};

	}
}
