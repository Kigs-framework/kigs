#ifndef AndroidGraphicBufferN_h_
#define AndroidGraphicBufferN_h_

#include <android\native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2ext.h>

typedef struct android_native_base_t
{
	int magic;
	int version;

	int reserved[4];

	/* reference-counting interface */
	void(*incRef)(struct android_native_base_t* base);
	void(*decRef)(struct android_native_base_t* base);
} android_native_base_t;

typedef struct native_handle
{
	int version;        /* sizeof(native_handle_t) */
	int numFds;         /* number of file-descriptors at &data[0] */
	int numInts;        /* number of ints at &data[numFds] */
	int *data;        /* numFds + numInts ints */
} native_handle_t;

// 
typedef struct ANativeWindowBuffer
{
	android_native_base_t common;

	int width;
	int height;
	int stride;
	int format;
	int usage;

	void* reserved[2];

	native_handle_t* handle;

	void* reserved_proc[8];
} ANativeWindowBuffer_t;

class AndroidGraphicBufferN : public AndroidGraphicBufferBase
{
public:
	AndroidGraphicBufferN(uint32_t width, uint32_t height, uint32_t usage, TinyImage::ImageFormat format) : AndroidGraphicBufferBase(width, height, usage, format)
	{
		//ANativeWindow_acquire(myNativeWindow);
		//ANativeWindow_setBuffersGeometry(myNativeWindow, width, height, GetAndroidFormat(format));
	}

	~AndroidGraphicBufferN()
	{
		//ANativeWindow_release(myNativeWindow);
	}

	bool Bind() override 
	{
#if __ANDROID_API__ < 24
		return false;
#else
		// create eglImage
		if (mEGLImage != nullptr)
			return true;

		//GraphicBuffer * pGraphicBuffer = new GraphicBuffer(ImageWidth, ImageHeight, PIXEL_FORMAT_RGB_565, GraphicBuffer::USAGE_SW_WRITE_OFTEN | GraphicBuffer::USAGE_HW_TEXTURE);



		//printf("size w:%d  h:%d", mWidth, mHeight);
		EGLint eglNCBAttrs[] = {
			EGL_WIDTH, (EGLint)mWidth,
			EGL_HEIGHT ,(EGLint)mHeight,
			EGL_RED_SIZE,8,
			EGL_GREEN_SIZE,8 ,
			EGL_BLUE_SIZE,8,
			EGL_ALPHA_SIZE,8,
			EGL_NATIVE_BUFFER_USAGE_ANDROID,EGL_NATIVE_BUFFER_USAGE_TEXTURE_BIT_ANDROID,
			EGL_NONE };
		mBuff = eglCreateNativeClientBufferANDROID(eglNCBAttrs);

		//printf("check cast");
		//ANativeWindowBuffer* nwb = reinterpret_cast<ANativeWindowBuffer*>(mBuff);
		//printf("common magic:%d  version:%d", nwb->common.magic, nwb->common.version);
		//printf("ANWB w:%d  h:%d  str:%d  fmt:%d", nwb->width, nwb->height, nwb->stride, nwb->format);
		//native_handle_t* data = nwb->handle;
		//printf("Handle v:%d  Fds:%d  Ints:%d  Data:%d", data->version, data->numFds, data->numInts, data->data);
		

		EGLint eglImgAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE };
		mEGLImage = eglCreateImageKHR(eglGetDisplay(EGL_DEFAULT_DISPLAY), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, mBuff, eglImgAttrs);
		//printf("%p", mEGLImage);
		
		glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, mEGLImage);
		return mEGLImage != nullptr;
#endif
	}

	int Lock(uint32_t usage, unsigned char **bits) override
	{
		//native_handle_t* data = reinterpret_cast<ANativeWindowBuffer*>(mBuff)->handle;
		//bits = reinterpret_cast<unsigned char**>(&data->data);
		return 1;
	}

	int Lock(uint32_t usage, const ARect& rect, unsigned char **bits) override
	{
		//native_handle_t* data = reinterpret_cast<ANativeWindowBuffer*>(mBuff)->handle;
		//bits = reinterpret_cast<unsigned char**>(&data->data);
		return 1;
	}

	int Unlock() override
	{
		return 1;
	}

	uint32_t GetAndroidFormat(TinyImage::ImageFormat aFormat)
	{
		switch (aFormat) {
		case TinyImage::ImageFormat::RGBA_32_8888:
			return WINDOW_FORMAT_RGBA_8888;
		case TinyImage::ImageFormat::RGB_24_888:
			return WINDOW_FORMAT_RGBX_8888;
		case TinyImage::ImageFormat::RGB_16_565:
			return WINDOW_FORMAT_RGB_565;
		default:
			return 0;
		}
	}

private :
	EGLClientBuffer mBuff;
	void *mEGLImage = nullptr;
};

#endif