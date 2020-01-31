#pragma once

#include "UI/UITexturedItem.h"
#include "maReference.h"
#include "SmartPointer.h"

class UIStream : public UITexturedItem
{
public:
	DECLARE_CLASS_INFO(UIStream, UITexturedItem, 2DLayers);
	DECLARE_CONSTRUCTOR(UIStream);

	virtual void InitModifiable() override;
	virtual void Update(const Timer&, void*) override;

	virtual bool addItem(const CMSP& item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override;
	virtual bool removeItem(const CMSP& item  DECLARE_DEFAULT_LINK_NAME) override;

	//unsigned char* CurrentBuffer() { return _temp.data(); }

protected:

	virtual ~UIStream() {}
	virtual void NotifyUpdate(const unsigned int labelid) override;

	SmartPointer<CoreModifiable> _framebufferstream;

	maString _video_file;
	maReference _framebuffer_ref;
	maBool _autoplay;
	maBool _loop;
	maBool _auto_size;
	maString _notification_start;
	maString _notification_end;

	maFloat mVolume = BASE_ATTRIBUTE(Volume, 1.0f);

	bool myIsPlaying;

	DECLARE_METHOD(StartVideo);

};
