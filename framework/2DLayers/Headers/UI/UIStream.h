#pragma once

#include "UI/UITexturedItem.h"
#include "maReference.h"
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

			SmartPointer<CoreModifiable> mFrameBufferStream;

			bool								mAutoplay = true;
			bool								mLoop = true;
			bool								mAutoSize = false;
			float								mVolume = 1.0f;
			std::string							mNotificationStart = "";
			std::string							mNotificationEnd = "";
			std::string							mVideoFile="";
			std::weak_ptr<CoreModifiable>		mFrameBuffer;

			WRAP_ATTRIBUTES(mAutoplay, mLoop, mAutoSize, mVolume, mNotificationStart, mNotificationEnd, mVideoFile, mFrameBuffer);

			bool mIsPlaying;

			DECLARE_METHOD(StartVideo);
			COREMODIFIABLE_METHODS(StartVideo);
		};

	}
}