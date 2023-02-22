#include "UI/UIStream.h"
#include <CoreBaseApplication.h>
#include "Timer.h"
#include "NotificationCenter.h"

using namespace Kigs::Draw2D;
using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(UIStream)


IMPLEMENT_CONSTRUCTOR(UIStream)
, mIsPlaying(false)
{
	setInitParameter("VideoFile", true);
}

void UIStream::InitModifiable()
{
	UITexturedItem::InitModifiable();

	setOwnerNotification("Volume", true);
	if(IsInit())
	{
		if(!mVideoFile.empty())
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
			CMSP currentFB=getValue<CMSP>("FrameBuffer");
			if (currentFB)
				mFrameBufferStream = currentFB;
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

	KigsCore::GetNotificationCenter()->addObserver(this, "StartVideo", mNotificationStart);
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

			SmartPointer<Pict::TinyImage>	img = Pict::TinyImage::CreateImage(data, size.x, size.y, Pict::TinyImage::RGBA_32_8888, linesize);
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

				if (!mNotificationEnd.empty())
					KigsCore::GetNotificationCenter()->postNotificationName(mNotificationEnd, this);
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

	if(labelid == KigsID("VideoFile")._id  || labelid == KigsID("FrameBuffer")._id )
	{
		Init();
	}
	else if(labelid == KigsID("NotificationStart")._id )
	{
		KigsCore::GetNotificationCenter()->removeObserver(this);
		if (!mNotificationEnd.empty())
			KigsCore::GetNotificationCenter()->postNotificationName(mNotificationEnd, this);
	}
	else if (labelid == KigsID("Volume")._id)
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