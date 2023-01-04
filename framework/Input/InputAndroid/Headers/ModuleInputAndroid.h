#pragma once

#include "ModuleBase.h"

namespace Kigs
{
	namespace Input
	{
		using namespace Kigs::Core;
		extern SP<ModuleBase> PlatformGUIModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);


		// ****************************************
		// * ModuleInputAndroid class
		// * --------------------------------------
		/**
		 * \file	ModuleInputAndroid.h
		 * \class	ModuleInputAndroid
		 * \ingroup Input
		 * \ingroup Module
		 * \brief	Specific module for android input management.
		 */
		 // ****************************************

		class ModuleInputAndroid : public ModuleBase
		{
		public:
			DECLARE_CLASS_INFO(ModuleInputAndroid, ModuleBase, Input)
				DECLARE_CONSTRUCTOR(ModuleInputAndroid);

			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;
			void Close() override;
			void Update(const Time::Timer& timer, void* /*addParam*/) override;

			bool	isInit() { return mIsInitOK; }

			unsigned int	getJoystickCount() { return mJoystickCount; }

			bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;

		protected:
			bool					mIsInitOK;
			unsigned int			mJoystickCount;

		};

	}
}
