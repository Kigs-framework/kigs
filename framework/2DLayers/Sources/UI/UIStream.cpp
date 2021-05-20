#include "UI/UIStream.h"
#include <CoreBaseApplication.h>
#include "Timer.h"
#include "NotificationCenter.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIStream, UIStream, 2DLayers);;
IMPLEMENT_CLASS_INFO(UIStream)


IMPLEMENT_CONSTRUCTOR(UIStream)
, mVideoFile(*this, true, "VideoFile", "")
, mFrameBuffer(*this, false, "FrameBuffer", "")
, mAutoplay(*this, false, "Autoplay", true)
, mLoop(*this, false, "Loop", true)
, mAutoSize(*this, false, "AutoSize", false)
, mNotificationStart(*this, false, "NotificationStart", "")
, mNotificationEnd(*this, false, "NotificationEnd", "")
, mIsPlaying(false)
{
}

void UIStream::InitModifiable()
{
	UITexturedItem::InitModifiable();

	mVolume.changeNotificationLevel(Owner);

	if(IsInit())
	{
		if(!mVideoFile.const_ref().empty())
		{
			mFrameBufferStream = KigsCore::GetInstanceOf(getName() + "_stream", "MPEG4BufferStream");
			CMSP timer = KigsCore::GetCoreApplication()->GetApplicationTimer();
			mFrameBufferStream->addItem(timer);
			mFrameBufferStream->setValue("FileName", mVideoFile);
			mFrameBufferStream->setValue("Format", "RGB24");
			mFrameBufferStream->setValue("Volume", (float)mVolume);
			mFrameBufferStream->Init();

			KigsCore::Connect(mFrameBufferStream.get(), "EndReached", this, "EndReached", [this](CoreModifiable* fb)
			{
				if (mLoop)
				{
					fb->SimpleCall("Stop");
					fb->SimpleCall("Play");
				}
			});

			if (!mFrameBufferStream->IsInit())
			{
				mFrameBufferStream = nullptr;
				return;
			}
			if (mAutoplay)
			{
				mIsPlaying = true;
				mFrameBufferStream->CallMethod("Play", this);
			}
		}
		else
		{
			CoreModifiable* fb = mFrameBuffer;
			if (fb)
				mFrameBufferStream = fb->SharedFromThis();
			else
			{
				return;
			}
		}
	}
	
	SP<Texture> texture = KigsCore::GetInstanceOf(getName() + "_streamtex", "Texture");
	texture->setValue("IsDynamic", true);
	texture->AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "IsBGR", false);
	texture->Init();
	mTexturePointer->setTexture(texture);

	KigsCore::GetNotificationCenter()->addObserver(this, "StartVideo", mNotificationStart.const_ref());
}

void UIStream::Update(const Timer& timer, void* v)
{
	if(mIsPlaying &&mFrameBufferStream)
	{
		mFrameBufferStream->CallUpdate(timer, 0);
		bool is_ready=false;
		mFrameBufferStream->CallMethod("HasReadyBuffer", this, &is_ready);
		if(is_ready)
		{
			Point2DI size;
			mFrameBufferStream->getValue("Width", size.x);
			mFrameBufferStream->getValue("Height", size.y);
			//_temp.resize(size.x*size.y*4);

			unsigned char * data = 0;
			mFrameBufferStream->CallMethod("GetNewestReadyBuffer", this, (void*)&data);

			if(mAutoSize)
			{
				SetNodeFlag(Node2D_SizeChanged);
				mSize = size;				
			}


			auto linesize = mFrameBufferStream->getValue<u32>("LineSize");

			SmartPointer<TinyImage>	img = TinyImage::CreateImage(data, size.x, size.y, TinyImage::RGBA_32_8888, linesize);
			//TinyImage::ExportImage("test.png", data, size.x, size.y, TinyImage::RGB_24_888, TinyImage::PNG_IMAGE);

			mTexturePointer->getTexture()->CreateFromImage(img);
			
			mFrameBufferStream->CallMethod("FreeBuffers", this);
		}
		else
		{
			bool is_ended = false;
			mFrameBufferStream->CallMethod("HasReachEnd", this, &is_ended);
			if(is_ended)
			{
				if (mLoop)
				{
					mFrameBufferStream->CallMethod("Stop", this);
					mFrameBufferStream->CallMethod("Play", this);
				}
				else
					mIsPlaying = false;

				if (!mNotificationEnd.const_ref().empty())
					KigsCore::GetNotificationCenter()->postNotificationName(mNotificationEnd.const_ref(), this);
			}
		}
	}
}

bool UIStream::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType("FrameBufferStream"))
	{
		mFrameBufferStream = item;
		if (mAutoplay)
		{
			mIsPlaying = true;
			mFrameBufferStream->CallMethod("Play", this);
		}
		return true;
	}
	return UITexturedItem::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool UIStream::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if(mFrameBufferStream == item)
	{
		mFrameBufferStream = 0;
		return true;
	}
	
	return UITexturedItem::removeItem(item PASS_LINK_NAME(linkName));

}

void UIStream::NotifyUpdate(const unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);

	if(labelid == mVideoFile.getLabelID() || labelid == mFrameBuffer.getLabelID())
	{
		Init();
	}
	else if(labelid == mNotificationStart.getLabelID())
	{
		KigsCore::GetNotificationCenter()->removeObserver(this);
		if (!mNotificationEnd.const_ref().empty())
			KigsCore::GetNotificationCenter()->postNotificationName(mNotificationEnd.const_ref(), this);
	}
	else if (labelid == mVolume.getID())
	{
		if (mFrameBufferStream) mFrameBufferStream->setValue("Volume", (float)mVolume);
	}
}

DEFINE_METHOD(UIStream, StartVideo)
{
	if (mFrameBufferStream)
	{
		mIsPlaying = true;
		mFrameBufferStream->CallMethod("Play", this);
	}
	return false;
}