#pragma once

#include "ModuleBase.h"
#include "ModuleGUI.h"

namespace Kigs
{
	namespace Gui
	{
		// ****************************************
		// * ModuleGUIWindows class
		// * --------------------------------------
		/**
		* \file	ModuleGUIWindows.h
		* \class	ModuleGUIWindows
		* \ingroup GUIModule
		* \ingroup Module
		* \brief Specific Win32 UWP GUI module
		*
		*/
		// ****************************************

		class ModuleGUIWindows : public ModuleBase
		{
		public:
			DECLARE_CLASS_INFO(ModuleGUIWindows, ModuleBase, GUI)
				ModuleGUIWindows(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~ModuleGUIWindows();

			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);
			void Close();
			virtual void Update(const Time::Timer& timer, void* addParam);
		};

	}
}
