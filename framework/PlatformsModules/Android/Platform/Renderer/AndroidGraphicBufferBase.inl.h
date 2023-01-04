/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AndroidGraphicBufferBase_h_
#define AndroidGraphicBufferBase_h_

#include <android\native_window.h>
#include <android\rect.h>

#include "TinyImage.h"

/**
 * This class allows access to Android's direct texturing mechanism. Locking
 * the buffer gives you a pointer you can read/write to directly. It is fully
 * threadsafe, but you probably really want to use the AndroidDirectTexture
 * class which will handle double buffering.
 *
 * In order to use the buffer in OpenGL, just call Bind() and it will attach
 * to whatever texture is bound to GL_TEXTURE_2D.
 */
class AndroidGraphicBufferBase
{
public:
  enum {
    UsageSoftwareRead = 1,
    UsageSoftwareWrite = 1 << 1,
    UsageTexture = 1 << 2,
    UsageTarget = 1 << 3,
    Usage2D = 1 << 4
  };

	AndroidGraphicBufferBase(uint32_t width, uint32_t height, uint32_t usage, Kigs::Pict::TinyImage::ImageFormat format) 
	{
		mWidth = width;
		mHeight = height;
		mUsage = usage;
		mFormat = format;
	}

	virtual ~AndroidGraphicBufferBase() { }
	
	uint32_t Width() { return mWidth; }
	uint32_t Height() { return mHeight; }

	virtual int Lock(uint32_t usage, unsigned char **bits) { return 0; }
	virtual int Lock(uint32_t usage, const ARect& rect, unsigned char **bits) { return 0; }
	virtual int Unlock() { return 0; }
	virtual bool Reallocate(uint32_t aWidth, uint32_t aHeight, Kigs::Pict::TinyImage::ImageFormat aFormat) { return false; }
	
	virtual bool Bind() { return false; }
	virtual ANativeWindow_Buffer * getNativeWindowsHandle() { return myNativeWindowBuffer; }

protected:
	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mUsage;
	Kigs::Pict::TinyImage::ImageFormat mFormat;

	ANativeWindow_Buffer * myNativeWindowBuffer;
	ANativeWindow * myNativeWindow;
};

#endif /* AndroidGraphicBuffer_h_ */
