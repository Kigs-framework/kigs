#pragma once

#include "ModuleBase.h"

#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <commctrl.h>
#include <basetsd.h>
#include <dinput.h>

namespace Kigs
{
	namespace Input
	{
		using namespace Kigs::Core;
		class ModuleInputDX;


		// ****************************************
		// * ModuleInputDX class
		// * --------------------------------------
		/**
		* \file	ModuleInputDX.h
		* \class	ModuleInputDX
		* \ingroup Input
		* \ingroup Module
		* \brief Specific Input Module for DirectX API.
		*
		*/
		// ****************************************
		class ModuleInputDX : public ModuleBase
		{
		public:

			DECLARE_CLASS_INFO(ModuleInputDX, ModuleBase, Input)

				ModuleInputDX(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);
			void Close();
			virtual void Update(const Timer& timer, void* addParam);

			LPDIRECTINPUT8	getDirectInput() { return mDirectInput; }

			bool	isInit() { return mIsInitOK; }

			unsigned int	getJoystickCount() { return mJoystickCount; }

			bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;

			virtual ~ModuleInputDX();
		protected:


			LPDIRECTINPUT8			mDirectInput;
			bool					mIsInitOK;
			unsigned int			mJoystickCount;

		};

	}
}
