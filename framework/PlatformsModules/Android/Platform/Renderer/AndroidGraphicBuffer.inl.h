/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AndroidGraphicBuffer_h_
#define AndroidGraphicBuffer_h_

#include <dlfcn.h>
#include <GLES2/gl2.h>
#include <EGL\eglext.h>

#include "TinyImage.h"

#include <android\rect.h>

typedef void *EGLContext;
typedef void *EGLDisplay;
typedef uint32_t EGLenum;
typedef int32_t EGLint;
typedef uint32_t EGLBoolean;

#define ANDROID_LIBUI_PATH "libui.so"
#define ANDROID_GLES_PATH "libGLESv2.so"
#define ANDROID_EGL_PATH "libEGL.so"

enum {
	/* buffer is never read in software */
	GRALLOC_USAGE_SW_READ_NEVER = 0x00000000,
	/* buffer is rarely read in software */
	GRALLOC_USAGE_SW_READ_RARELY = 0x00000002,
	/* buffer is often read in software */
	GRALLOC_USAGE_SW_READ_OFTEN = 0x00000003,
	/* mask for the software read values */
	GRALLOC_USAGE_SW_READ_MASK = 0x0000000F,

	/* buffer is never written in software */
	GRALLOC_USAGE_SW_WRITE_NEVER = 0x00000000,
	/* buffer is never written in software */
	GRALLOC_USAGE_SW_WRITE_RARELY = 0x00000020,
	/* buffer is never written in software */
	GRALLOC_USAGE_SW_WRITE_OFTEN = 0x00000030,
	/* mask for the software write values */
	GRALLOC_USAGE_SW_WRITE_MASK = 0x000000F0,

	/* buffer will be used as an OpenGL ES texture */
	GRALLOC_USAGE_HW_TEXTURE = 0x00000100,
	/* buffer will be used as an OpenGL ES render target */
	GRALLOC_USAGE_HW_RENDER = 0x00000200,
	/* buffer will be used by the 2D hardware blitter */
	GRALLOC_USAGE_HW_2D = 0x00000400,
	/* buffer will be used with the framebuffer device */
	GRALLOC_USAGE_HW_FB = 0x00001000,
	/* mask for the software usage bit-mask */
	GRALLOC_USAGE_HW_MASK = 0x00001F00,
};

enum {
	HAL_PIXEL_FORMAT_RGBA_8888 = 1,
	HAL_PIXEL_FORMAT_RGBX_8888 = 2,
	HAL_PIXEL_FORMAT_RGB_888 = 3,
	HAL_PIXEL_FORMAT_RGB_565 = 4,
	HAL_PIXEL_FORMAT_BGRA_8888 = 5,
	HAL_PIXEL_FORMAT_RGBA_5551 = 6,
	HAL_PIXEL_FORMAT_RGBA_4444 = 7,
};

static bool gTryRealloc = true;

static class GLFunctions
{
public:
	GLFunctions()
		: fGetDisplay(nullptr)
		, fEGLGetError(nullptr)
		, fCreateImageKHR(nullptr)
		, fDestroyImageKHR(nullptr)
		, fImageTargetTexture2DOES(nullptr)
		, fBindTexture(nullptr)
		, fGLGetError(nullptr)
		, fGraphicBufferCtor(nullptr)
		, fGraphicBufferDtor(nullptr)
		, fGraphicBufferLock(nullptr)
		, fGraphicBufferLockRect(nullptr)
		, fGraphicBufferUnlock(nullptr)
		, fGraphicBufferGetNativeBuffer(nullptr)
		, fGraphicBufferReallocate(nullptr)
		, mInitialized(false)
	{
	}

	typedef EGLDisplay(*pfnGetDisplay)(void *display_id);
	pfnGetDisplay fGetDisplay;
	typedef EGLint(*pfnEGLGetError)(void);
	pfnEGLGetError fEGLGetError;

	typedef EGLImageKHR(*pfnCreateImageKHR)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);
	pfnCreateImageKHR fCreateImageKHR;
	typedef EGLBoolean(*pfnDestroyImageKHR)(EGLDisplay dpy, EGLImageKHR image);
	pfnDestroyImageKHR fDestroyImageKHR;

	typedef void(*pfnImageTargetTexture2DOES)(GLenum target, EGLImageKHR image);
	pfnImageTargetTexture2DOES fImageTargetTexture2DOES;

	typedef void(*pfnBindTexture)(GLenum target, GLuint texture);
	pfnBindTexture fBindTexture;

	typedef GLenum(*pfnGLGetError)();
	pfnGLGetError fGLGetError;

	typedef void(*pfnGraphicBufferCtor)(void*, uint32_t w, uint32_t h, uint32_t format, uint32_t usage);
	pfnGraphicBufferCtor fGraphicBufferCtor;

	typedef void(*pfnGraphicBufferDtor)(void*);
	pfnGraphicBufferDtor fGraphicBufferDtor;

	typedef int(*pfnGraphicBufferLock)(void*, uint32_t usage, unsigned char **addr);
	pfnGraphicBufferLock fGraphicBufferLock;

	typedef int(*pfnGraphicBufferLockRect)(void*, uint32_t usage, const ARect&, unsigned char **addr);
	pfnGraphicBufferLockRect fGraphicBufferLockRect;

	typedef int(*pfnGraphicBufferUnlock)(void*);
	pfnGraphicBufferUnlock fGraphicBufferUnlock;

	typedef void* (*pfnGraphicBufferGetNativeBuffer)(void*);
	pfnGraphicBufferGetNativeBuffer fGraphicBufferGetNativeBuffer;

	typedef int(*pfnGraphicBufferReallocate)(void*, uint32_t w, uint32_t h, uint32_t format);
	pfnGraphicBufferReallocate fGraphicBufferReallocate;

	bool EnsureInitialized()
	{
		if (mInitialized) {
			return true;
		}

		void *handle = dlopen(ANDROID_EGL_PATH, RTLD_LAZY);
		if (!handle) {
			printf("Couldn't load EGL library");
			return false;
		}

		fGetDisplay = (pfnGetDisplay)dlsym(handle, "eglGetDisplay");
		fEGLGetError = (pfnEGLGetError)dlsym(handle, "eglGetError");
		fCreateImageKHR = (pfnCreateImageKHR)dlsym(handle, "eglCreateImageKHR");
		fDestroyImageKHR = (pfnDestroyImageKHR)dlsym(handle, "eglDestroyImageKHR");

		if (!fGetDisplay || !fEGLGetError || !fCreateImageKHR || !fDestroyImageKHR) {
			printf("Failed to find some EGL functions");
			return false;
		}

		handle = dlopen(ANDROID_GLES_PATH, RTLD_LAZY);
		if (!handle) {
			printf("Couldn't load GL library");
			return false;
		}

		fImageTargetTexture2DOES = (pfnImageTargetTexture2DOES)dlsym(handle, "glEGLImageTargetTexture2DOES");
		fBindTexture = (pfnBindTexture)dlsym(handle, "glBindTexture");
		fGLGetError = (pfnGLGetError)dlsym(handle, "glGetError");

		if (!fImageTargetTexture2DOES || !fBindTexture || !fGLGetError) {
			printf("Failed to find some GL functions");
			return false;
		}

		handle = dlopen(ANDROID_LIBUI_PATH, RTLD_LAZY);
		if (!handle) {
			printf("Couldn't load libui.so");
			return false;
		}

		fGraphicBufferCtor = (pfnGraphicBufferCtor)dlsym(handle, "_ZN7android13GraphicBufferC1Ejjij");
		fGraphicBufferDtor = (pfnGraphicBufferDtor)dlsym(handle, "_ZN7android13GraphicBufferD1Ev");
		fGraphicBufferLock = (pfnGraphicBufferLock)dlsym(handle, "_ZN7android13GraphicBuffer4lockEjPPv");
		fGraphicBufferLockRect = (pfnGraphicBufferLockRect)dlsym(handle, "_ZN7android13GraphicBuffer4lockEjRKNS_4RectEPPv");
		fGraphicBufferUnlock = (pfnGraphicBufferUnlock)dlsym(handle, "_ZN7android13GraphicBuffer6unlockEv");
		fGraphicBufferGetNativeBuffer = (pfnGraphicBufferGetNativeBuffer)dlsym(handle, "_ZNK7android13GraphicBuffer15getNativeBufferEv");
		fGraphicBufferReallocate = (pfnGraphicBufferReallocate)dlsym(handle, "_ZN7android13GraphicBuffer10reallocateEjjij");

		if (!fGraphicBufferCtor || !fGraphicBufferDtor || !fGraphicBufferLock ||
			!fGraphicBufferUnlock || !fGraphicBufferGetNativeBuffer) {
			printf("Failed to lookup some GraphicBuffer functions");
			return false;
		}

		mInitialized = true;
		return true;
	}

private:
	bool mInitialized;

} sGLFunctions;



struct ANativeWindowHandle
{
public:
	char reserved[40];
	ANativeWindow_Buffer ANW;
};


typedef void* EGLImageKHR;
typedef void* EGLClientBuffer;

class AndroidGraphicBuffer : public AndroidGraphicBufferBase
{
public:
	AndroidGraphicBuffer(uint32_t width, uint32_t height, uint32_t usage, Kigs::Pict::TinyImage::ImageFormat format) : AndroidGraphicBufferBase(width, height, usage, format) {	}
	~AndroidGraphicBuffer() override { DestroyBuffer(); }


	int Lock(uint32_t usage, unsigned char **bits) override
	{
		if (!EnsureInitialized())
			return false;

		return sGLFunctions.fGraphicBufferLock(mHandle, GetAndroidUsage(usage), bits);
	}
	int Lock(uint32_t usage, const ARect& rect, unsigned char **bits) override 
	{
		if (!EnsureInitialized())
			return false;

		return sGLFunctions.fGraphicBufferLockRect(mHandle, GetAndroidUsage(usage), rect, bits);
	}
	int Unlock() override
	{
		if (!EnsureInitialized())
			return false;

		return sGLFunctions.fGraphicBufferUnlock(mHandle);
	}
	bool Reallocate(uint32_t aWidth, uint32_t aHeight, Kigs::Pict::TinyImage::ImageFormat aFormat) override
	{
		if (!EnsureInitialized())
			return false;

		mWidth = aWidth;
		mHeight = aHeight;
		mFormat = aFormat;

		// Sometimes GraphicBuffer::reallocate just doesn't work. In those cases we'll just allocate a brand
		// new buffer. If reallocate fails once, never try it again.
		int err = 0;
		if (!gTryRealloc || (err = sGLFunctions.fGraphicBufferReallocate(mHandle, aWidth, aHeight, GetAndroidFormat(aFormat))) != 0) {
			printf("REALLOC FAILLED %x\n", err);
			DestroyBuffer();
			EnsureBufferCreated();

			//gTryRealloc = false;
		}

		if (mEGLImage != NULL)
			sGLFunctions.fDestroyImageKHR(sGLFunctions.fGetDisplay(EGL_DEFAULT_DISPLAY), mEGLImage);
		mEGLImage = NULL;
		return true;
	}

	bool Bind() override
	{
		if (!EnsureInitialized())
			return false;

		if (!EnsureEGLImage()) {
			printf("No valid EGLImage!");
			return false;
		}

		sGLFunctions.fImageTargetTexture2DOES(GL_TEXTURE_2D, mEGLImage);
		return true;
	}

	ANativeWindow_Buffer * getNativeWindowsHandle() override { return &myANativeWindowHandle->ANW; }

private:
  ANativeWindowHandle * myANativeWindowHandle;

  bool EnsureInitialized() 
  {
	  if (!sGLFunctions.EnsureInitialized())
	  {
		  printf("sGLFunctions init FAILED");
		  return false;
	  }

	  EnsureBufferCreated();
	  return true;
  }

  bool EnsureEGLImage()
  {
	  if (mEGLImage)
		  return true;
	  if (!EnsureInitialized())
		  return false;

	  EGLint eglImgAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE };

	  void* nativeBuffer = sGLFunctions.fGraphicBufferGetNativeBuffer(mHandle);
	  mEGLImage = sGLFunctions.fCreateImageKHR(sGLFunctions.fGetDisplay(EGL_DEFAULT_DISPLAY), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, (EGLClientBuffer)nativeBuffer, eglImgAttrs);
	  return mEGLImage != nullptr;
  }

  void DestroyBuffer() 
  {
	  mEGLImage = nullptr;

	  if (mHandle) {
		  if (sGLFunctions.EnsureInitialized()) {
			  sGLFunctions.fGraphicBufferDtor(mHandle);
		  }
		  free(mHandle);
		  mHandle = nullptr;
	  }
  }

  bool EnsureBufferCreated()
  {
	  if (!mHandle) {
		  mHandle = malloc(1024);
		  sGLFunctions.fGraphicBufferCtor(mHandle, mWidth, mHeight, GetAndroidFormat(mFormat), GetAndroidUsage(mUsage));
		  myANativeWindowHandle = reinterpret_cast<ANativeWindowHandle*>(mHandle);
	  }

	  return true;
  }

  uint32_t GetAndroidUsage(uint32_t aUsage)
  {
	  uint32_t flags = 0;

	  if (aUsage & UsageSoftwareRead) {
		  flags |= GRALLOC_USAGE_SW_READ_OFTEN;
	  }

	  if (aUsage & UsageSoftwareWrite) {
		  flags |= GRALLOC_USAGE_SW_WRITE_OFTEN;
	  }

	  if (aUsage & UsageTexture) {
		  flags |= GRALLOC_USAGE_HW_TEXTURE;
	  }

	  if (aUsage & UsageTarget) {
		  flags |= GRALLOC_USAGE_HW_RENDER;
	  }

	  if (aUsage & Usage2D) {
		  flags |= GRALLOC_USAGE_HW_2D;
	  }

	  return flags;
  }
  uint32_t GetAndroidFormat(Kigs::Pict::TinyImage::ImageFormat aFormat)
  {
	  switch (aFormat) {
	  case Kigs::Pict::TinyImage::ImageFormat::RGBA_32_8888:
		  return HAL_PIXEL_FORMAT_RGBA_8888;
	  case Kigs::Pict::TinyImage::ImageFormat::RGB_24_888:
		  return HAL_PIXEL_FORMAT_RGB_888;
	  case Kigs::Pict::TinyImage::ImageFormat::RGB_16_565:
		  return HAL_PIXEL_FORMAT_RGB_565;
	  default:
		  return 0;
	  }
  }

  void *mHandle = nullptr;
  void *mEGLImage = nullptr;
};

#endif /* AndroidGraphicBuffer_h_ */
