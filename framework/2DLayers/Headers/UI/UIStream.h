#pragma once

#include "UI/UITexturedItem.h"
#include "maString.h"
#include "SmartPointer.h"

namespace Kigs
{
	namespace Draw2D
	{
		// ****************************************
		// * UIStream class
		// * --------------------------------------
		/**
		* \file	UIStream.h
		* \class	UIStream
		* \ingroup 2DLayers
		* \brief	Display a video.
		*/
		// ****************************************

		class UIStream : public UITexturedItem
		{
		public:
			DECLARE_CLASS_INFO(UIStream, UITexturedItem, 2DLayers);
			DECLARE_CONSTRUCTOR(UIStream);
			virtual ~UIStream() {}

			virtual void InitModifiable() override;
			virtual void Update(const Timer&, void*) override;

			virtual bool addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
			virtual bool removeItem(const CMSP& item  DECLARE_DEFAULT_LINK_NAME) override;

		protected:


			virtual void NotifyUpdate(const unsigned int labelid) override;

			SmartPointer<CoreModifiable>		mFrameBufferStream;

			// don't use mapped attributes as they will be sent as parameter to CoreMethods
			maBool								mAutoplay = BASE_ATTRIBUTE(Autoplay,true);
			maBool								mLoop = BASE_ATTRIBUTE(Loop, true);
			maBool								mAutoSize = BASE_ATTRIBUTE(AutoSize, false);
			maFloat								mVolume = BASE_ATTRIBUTE(Volume, 1.0f);
			maString							mNotificationStart = BASE_ATTRIBUTE(NotificationStart, "");
			maString							mNotificationEnd = BASE_ATTRIBUTE(NotificationEnd, "");
			maString							mVideoFile = BASE_ATTRIBUTE(VideoFile, "");
			//std::weak_ptr<CoreModifiable>		mFrameBuffer;

			bool mIsPlaying;

			DECLARE_METHOD(StartVideo);
			COREMODIFIABLE_METHODS(StartVideo);
		};

	}
}