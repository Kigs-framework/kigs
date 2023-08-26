#pragma once

#include "MouseDevice.h"
#include "ModuleInputAndroid.h"

#include "maReference.h"

#include <jni.h>

namespace Kigs
{
	namespace Input
	{
		class MultiTouchDevice;

		// ****************************************
		// * MouseAndroid class
		// * --------------------------------------
		/**
		* \file	MouseAndroid.h
		* \class	MouseAndroid
		* \ingroup Input
		* \brief Android mouse management (touch).
		*
		*/
		// ****************************************

		class	MouseAndroid : public MouseDevice
		{
		public:
			DECLARE_CLASS_INFO(MouseAndroid, MouseDevice, Input)
				DECLARE_CONSTRUCTOR(MouseAndroid);

			bool	Aquire() override;
			bool	Release() override;

			void	UpdateDevice() override;

			void	DoInputDeviceDescription() override;

			void	IncWheelCount() { mWheelCount++; }
			void	IncButtonCount() { mButtonsCount++; }
			virtual ~MouseAndroid();

		protected:

			uint32_t	mCurrentActionPointer=(uint32_t)0xFFFFFFFF;
			DECLARE_METHOD(ReinitCB);

			maReference mMultiTouch = BASE_ATTRIBUTE(MultiTouch, "");
			MultiTouchDevice* mMultiTouchPointer = nullptr;

			int mFrameCountSinceLastValidTouched;
		};

	}
}