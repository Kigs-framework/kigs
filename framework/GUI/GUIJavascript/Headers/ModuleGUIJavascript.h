#pragma once
#include "ModuleBase.h"


namespace Kigs
{
	namespace Gui
	{
		using namespace Kigs::Core;
		extern SP<ModuleBase> PlatformGUIModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);
	

		// ****************************************
		// * ModuleGUIJavascript class
		// * --------------------------------------
		/**
		* \file	ModuleGUIJavascript.h
		* \class	ModuleGUIJavascript
		* \ingroup GUIModule
		* \ingroup Module
		* \brief Specific Emscripten GUI module
		*
		*/
		// ****************************************

		class ModuleGUIJavascript : public ModuleBase
		{
		public:

			DECLARE_CLASS_INFO(ModuleGUIJavascript,ModuleBase,GUI)
					 
			ModuleGUIJavascript(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
			
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override; 
			void Close() override;
			void Update(const Time::Timer& timer, void* addParam) override;
			 virtual ~ModuleGUIJavascript();
		 
		protected:
			
		 
		}; 

	}
}
