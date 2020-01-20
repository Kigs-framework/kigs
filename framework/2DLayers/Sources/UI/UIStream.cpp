#include "UI/UIStream.h"
#include <CoreBaseApplication.h>
#include "Timer.h"
#include "NotificationCenter.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIStream, UIStream, 2DLayers);;
IMPLEMENT_CLASS_INFO(UIStream)


IMPLEMENT_CONSTRUCTOR(UIStream)
, _video_file(*this, true, "VideoFile", "")
, _framebuffer_ref(*this, false, "FrameBuffer", "")
, _autoplay(*this, false, "Autoplay", true)
, _loop(*this, false, "Loop", true)
, _auto_size(*this, false, "AutoSize", false)
, _notification_start(*this, false, "NotificationStart", "")
, _notification_end(*this, false, "NotificationEnd", "")
, myIsPlaying(false)
{
}

void UIStream::InitModifiable()
{
	UITexturedItem::InitModifiable();

	mVolume.changeNotificationLevel(Owner);

	if(IsInit())
	{
		if(!_video_file.const_ref().empty())
		{
			_framebufferstream = KigsCore::GetInstanceOf(getName() + "_stream", "MPEG4BufferStream");
			CMSP timer = KigsCore::GetCoreApplication()->GetApplicationTimer();
			_framebufferstream->addItem(timer);
			_framebufferstream->setValue("FileName", _video_file);
			_framebufferstream->setValue("Format", "RGB24");
			_framebufferstream->setValue("Volume", (float)mVolume);
			_framebufferstream->Init();

			KigsCore::Connect(_framebufferstream.get(), "EndReached", this, "EndReached", [this](CoreModifiable* fb)
			{
				if (_loop)
				{
					fb->SimpleCall("Stop");
					fb->SimpleCall("Play");
				}
			});

			if (!_framebufferstream->IsInit())
			{
				_framebufferstream = nullptr;
				return;
			}
			if (_autoplay)
			{
				myIsPlaying = true;
				_framebufferstream->CallMethod("Play", this);
			}
		}
		else
		{
			CoreModifiable* fb = _framebuffer_ref;
			if (fb)
				_framebufferstream = NonOwningRawPtrToSmartPtr(fb);
			else
			{
				return;
			}
		}
	}
	
	SP<Texture> texture = KigsCore::GetInstanceOf(getName() + "_streamtex", "Texture");
	texture->setValue("IsDynamic", true);
	texture->AddDynamicAttribute(BOOL, "IsBGR", false);
	texture->Init();
	SetTexture(texture.get());

	KigsCore::GetNotificationCenter()->addObserver(this, "StartVideo", _notification_start.const_ref());
}

void UIStream::Update(const Timer& timer, void* v)
{
	if(myIsPlaying &&_framebufferstream)
	{
		_framebufferstream->CallUpdate(timer, 0);
		bool is_ready=false;
		_framebufferstream->CallMethod("HasReadyBuffer", this, &is_ready);
		if(is_ready)
		{
			Point2DI size;
			_framebufferstream->getValue("Width", size.x);
			_framebufferstream->getValue("Height", size.y);
			//_temp.resize(size.x*size.y*4);

			unsigned char * data = 0;
			_framebufferstream->CallMethod("GetNewestReadyBuffer", this, (void*)&data);

			if(_auto_size)
			{
				myNeedUpdatePosition = true;
				mySizeX = size.x;
				mySizeY = size.y;				
			}


			auto linesize = _framebufferstream->getValue<u32>("LineSize");

			SmartPointer<TinyImage>	img = OwningRawPtrToSmartPtr(TinyImage::CreateImage(data, size.x, size.y, TinyImage::RGBA_32_8888, linesize));
			//TinyImage::ExportImage("test.png", data, size.x, size.y, TinyImage::RGB_24_888, TinyImage::PNG_IMAGE);

			GetTexture()->CreateFromImage(img);
			
			_framebufferstream->CallMethod("FreeBuffers", this);
		}
		else
		{
			bool is_ended = false;
			_framebufferstream->CallMethod("HasReachEnd", this, &is_ended);
			if(is_ended)
			{
				if (_loop)
				{
					_framebufferstream->CallMethod("Stop", this);
					_framebufferstream->CallMethod("Play", this);
				}
				else
					myIsPlaying = false;

				if (!_notification_end.const_ref().empty())
					KigsCore::GetNotificationCenter()->postNotificationName(_notification_end.const_ref(), this);
			}
		}
	}
}

bool UIStream::addItem(CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType("FrameBufferStream"))
	{
		_framebufferstream = item;
		if (_autoplay)
		{
			myIsPlaying = true;
			_framebufferstream->CallMethod("Play", this);
		}
		return true;
	}
	return UITexturedItem::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool UIStream::removeItem(CMSP& item DECLARE_LINK_NAME)
{
	if(_framebufferstream == item)
	{
		_framebufferstream = 0;
		return true;
	}
	
	return UITexturedItem::removeItem(item PASS_LINK_NAME(linkName));

}

void UIStream::NotifyUpdate(const unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);

	if(labelid == _video_file.getLabelID() || labelid == _framebuffer_ref.getLabelID())
	{
		Init();
	}
	else if(labelid == _notification_start.getLabelID())
	{
		KigsCore::GetNotificationCenter()->removeObserver(this);
		if (!_notification_end.const_ref().empty())
			KigsCore::GetNotificationCenter()->postNotificationName(_notification_end.const_ref(), this);
	}
	else if (labelid == mVolume.getID())
	{
		if (_framebufferstream) _framebufferstream->setValue("Volume", (float)mVolume);
	}
}

DEFINE_METHOD(UIStream, StartVideo)
{
	if (_framebufferstream)
	{
		myIsPlaying = true;
		_framebufferstream->CallMethod("Play", this);
	}
	return false;
}