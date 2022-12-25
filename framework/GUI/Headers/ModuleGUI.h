#pragma once
// Doxygen group defines

/*! \defgroup GUIModule GUI base module
*  GUIModule Classes
*/


#include "ModuleBase.h"

namespace Kigs
{
	namespace Gui
	{
		using namespace Core;
		SP<ModuleBase> PlatformGUIModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);

		// ****************************************
		// * ModuleGUI class
		// * --------------------------------------
		/**
		* \file	ModuleGUI.h
		* \class	ModuleGUI
		* \ingroup GUIModule
		* \ingroup Module
		* \brief Generic Module for GUI classes ( Windows management )
		*
		*/
		// ****************************************

		class ModuleGUI : public ModuleBase
		{
		public:

			DECLARE_CLASS_INFO(ModuleGUI, ModuleBase, GUI)

				//! constructor
				ModuleGUI(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			//! destructor
			virtual ~ModuleGUI();

			//! init module
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;

			//! close module
			void Close() override;

			//! update module
			void Update(const Timer& timer, void* addParam) override;

		protected:



		};

	}
}
