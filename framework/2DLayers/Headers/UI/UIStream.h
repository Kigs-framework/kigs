#pragma once

#include "UI/UITexturedItem.h"
#include "maReference.h"
#include "SmartPointer.h"

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

	virtual void InitModifiable() override;
	virtual void Update(const Timer&, void*) override;

	virtual bool addItem(const CMSP& item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override;
	virtual bool removeItem(const CMSP& item  DECLARE_DEFAULT_LINK_NAME) override;

protected:

	virtual ~UIStream() {}
	virtual void NotifyUpdate(const unsigned int labelid) override;

	SmartPointer<CoreModifiable> mFrameBufferStream;

	maString mVideoFile;
	maReference mFrameBuffer;
	maBool mAutoplay;
	maBool mLoop;
	maBool mAutoSize;
	maString mNotificationStart;
	maString mNotificationEnd;

	maFloat mVolume = BASE_ATTRIBUTE(Volume, 1.0f);

	bool mIsPlaying;

	DECLARE_METHOD(StartVideo);
	COREMODIFIABLE_METHODS(StartVideo);
};
