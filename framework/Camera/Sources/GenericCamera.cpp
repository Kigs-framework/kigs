#include "PrecompiledHeaders.h"
#include "GenericCamera.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(GenericCamera)

void GenericCamera::InitModifiable()
{
	FrameBufferStream::InitModifiable();

	int WantedResX = mWidth;
	int WantedResY = mHeight;
	mNeedCrop = false;

	// do width multiple of 16

	WantedResX /= 16;
	WantedResX *= 16;
	mWidth = WantedResX;

	// if mInitResX == 0 and / or mInitResY == 0, they will be set after the hardware was init 
	if ((mInitResX >mWidth) || (mInitResY > mHeight))
	{
		WantedResX = mInitResX;
		WantedResY = mInitResY;
	}
	
	if ((WantedResX < mInitResX) || (WantedResY < mInitResY))
	{
		mNeedCrop = true;	
	}

	// check if something must be changed
	mOffsetX.changeNotificationLevel(Owner);
	mOffsetY.changeNotificationLevel(Owner);
	mWidth.changeNotificationLevel(Owner);
	mHeight.changeNotificationLevel(Owner);
}

// called when one of the width / height / offset value is changed
void GenericCamera::RecomputeAllParams()
{

	// camera is already init, so width and height can not be > thand InitRes

	if (mWidth > mInitResX)
	{
		mWidth = mInitResX;
	}
	if (mHeight > mInitResY)
	{
		mHeight = mInitResY;
	}
		
	int WantedResX = mWidth;
	int WantedResY = mHeight;

	mNeedCrop = false;

	// do width multiple of 16

	WantedResX /= 16;
	WantedResX *= 16;
	mWidth = WantedResX;

	if ((WantedResX < mInitResX) || (WantedResY < mInitResY))
	{
		// offset X must be a multiple of 16
		int offset = ((int)mOffsetX) >> 4;
		offset = offset << 4;

		if (offset < 0)
		{
			offset = 0;
		}
		else if ((offset + mWidth)>mInitResX)
		{
			offset = mInitResX - mWidth;
		}
		mOffsetX = offset;

		// offset Y multiple of 2
		offset = ((int)mOffsetY) >> 1;
		offset = offset << 1;

		if (offset < 0)
		{
			offset = 0;
		}
		else if ((offset + mHeight)>mInitResY)
		{
			offset = mInitResY - mHeight;
		}
		mOffsetY = offset;

		mNeedCrop = true;
	}

	// wait for all buffers to be ok to change

	int countAvailable = 0;

	while (countAvailable != 3)
	{
		for (int i = 0; i < 3; i++)
		{
			if (mBufferState[i] != ProcessingBuffer)
			{
				mBufferState[i] = ProcessingBuffer;
				++countAvailable;
			}
		}
	}

	FreeFrameBuffers();
	AllocateFrameBuffers();

	for (int i = 0; i < 3; i++)
	{
		mBufferState[i] = FreeBuffer;
	}
}


void GenericCamera::NotifyUpdate(const unsigned int labelid)
{
	// if width / height offset has changed, recompute the whole thing, but initResX / initResY can't be modified
	if ((labelid == mOffsetX.getID()) || (labelid == mOffsetY.getID()) || (labelid == mWidth.getID()) || (labelid == mHeight.getID()))
	{
		RecomputeAllParams();
	}
	
}

DEFINE_METHOD(GenericCamera, Start)
{
	StartPreview();
	return false;
}