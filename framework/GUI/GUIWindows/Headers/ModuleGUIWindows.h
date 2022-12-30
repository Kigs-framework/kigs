
#pragma once

#include "ModuleBase.h"

namespace Kigs
{
	namespace Gui
	{
		using namespace Kigs::Core;
		using namespace Kigs::Time;

		class ModuleGUIWindows;

		// ****************************************
		// * ModuleGUIWindows class
		// * --------------------------------------
		/**
		* \file	ModuleGUIWindows.h
		* \class	DisplayDeviceCaps
		* \ingroup GUIModule
		* \ingroup Module
		* \brief	Specific Win32 GUI module.
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
			virtual void Update(const Timer& timer, void* addParam);
		};
	}
}
