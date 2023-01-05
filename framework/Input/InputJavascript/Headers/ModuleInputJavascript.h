#pragma once

#include "ModuleBase.h"

namespace Kigs
{
	namespace Input
	{
		using namespace Kigs::Core;
		
	
		extern SP<ModuleBase> PlatformInputModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);
		
		// ****************************************
		// * ModuleInputJavascript class
		// * --------------------------------------
		/**
		 * \class	ModuleInputJavascript
		 * \file	ModuleInputJavascript.h
		 * \ingroup Input
		 * \ingroup Module
		 * \brief	Javascript specific input module.
		 */
		 // ****************************************

		class ModuleInputJavascript : public ModuleBase
		{
		public:

			DECLARE_CLASS_INFO(ModuleInputJavascript,ModuleBase,Input)
					 
			ModuleInputJavascript(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
			
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;
			void Close() override;
			void Update(const Time::Timer& timer, void* addParam) override;

			bool	isInit(){return mIsInitOK;}

			unsigned int	getJoystickCount(){return mJoystickCount;}

			bool addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
			virtual ~ModuleInputJavascript();

		protected:
		 
			bool					mIsInitOK;
			unsigned int			mJoystickCount;

		}; 

	}
}
