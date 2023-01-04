#pragma once

#include "MultiTouchDevice.h"
#include "ModuleInputAndroid.h"


#include <jni.h>

namespace Kigs
{
	namespace Input
	{

		// ****************************************
		// * MultiTouchAndroid class
		// * --------------------------------------
		/**
		* \file	MultiTouchAndroid.h
		* \class	MultiTouchAndroid
		* \ingroup Input
		* \brief Android multitouch management.
		*
		*/
		// ****************************************
		class	MultiTouchAndroid : public MultiTouchDevice
		{
		public:
			DECLARE_CLASS_INFO(MultiTouchAndroid, MultiTouchDevice, Input)

				MultiTouchAndroid(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			void	UpdateDevice() override;

			void	DoInputDeviceDescription() override;
			virtual ~MultiTouchAndroid();

		protected:

			DECLARE_METHOD(ReinitCB);

			std::vector<jobject> mTouchList;
			jmethodID 	mGetEventCount;
			jmethodID 	mGetEvent;
			jmethodID 	mClearEventList;

			jmethodID mEventGetX;
			jmethodID mEventGetY;
			jmethodID mEventGetAction;

		};

	}
}
