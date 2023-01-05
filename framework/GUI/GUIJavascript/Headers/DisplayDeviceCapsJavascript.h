#pragma once

#include "DisplayDeviceCaps.h"

namespace Kigs
{
	namespace Gui
	{

		// ****************************************
		// * DisplayDeviceCapsJavascript class
		// * --------------------------------------
		/**
		* \file	DisplayDeviceCapsJavascript.h
		* \class	DisplayDeviceCapsJavascript
		* \ingroup GUIModule
		* \brief	Javascript display device capacities.
		*/
		// ****************************************

		class DisplayDeviceCapsJavascript : public DisplayDeviceCaps
		{
			public:
			DECLARE_CLASS_INFO(DisplayDeviceCapsJavascript,DisplayDeviceCaps,GUI)

			//! constructor
			DisplayDeviceCapsJavascript(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);

			bool SupportWindowedMode() override
			{
				return true; // enable creation of several windows in browser ?
			}
				//! destructor
			virtual ~DisplayDeviceCapsJavascript();

			protected:


		};    

	}
}