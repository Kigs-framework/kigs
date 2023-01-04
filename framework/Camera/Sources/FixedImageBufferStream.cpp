#include "PrecompiledHeaders.h"
#include "FixedImageBufferStream.h"
#include "Core.h"
#include "ModuleFileManager.h"
#include "TinyImage.h"
#include "TinyImageLoaderContext.h"

using namespace Kigs::Camera;
using namespace Kigs::Pict;

IMPLEMENT_CLASS_INFO(FixedImageBufferStream)

// I would like to have RGB_24 in every case
static TinyImageLoaderContext	gImageContext = { { TinyImage::NO_TEXTURE,						//NO_TEXTURE,
TinyImage::RGB_24_888,							//A3I5_TRANSLUCENT
TinyImage::RGB_24_888,							//PALETTE16_4_COLOR			(ABGR 1555)
TinyImage::RGB_24_888,							//PALETTE16_16_COLOR		(ABGR 1555)
TinyImage::RGB_24_888,							//PALETTE16_256_COLOR		(ABGR 1555)
TinyImage::RGB_24_888,							//COMPRESSED_4X4_TEXEL
TinyImage::RGB_24_888,							//A5I3_TRANSLUCENT
TinyImage::RGB_24_888,							//ABGR_16_1555_DIRECT_COLOR
TinyImage::RGB_24_888,							//RGBA_32_8888
TinyImage::RGB_24_888,							//RGB_24_888
TinyImage::RGB_24_888,							//RGBA_16_4444
TinyImage::RGB_24_888,							//BGRA_16_5551
TinyImage::RGB_24_888,							//RGBA_16_5551
TinyImage::RGB_24_888,							//RGB_16_565
TinyImage::RGB_24_888,							//COMPRESSED_PVRTC
TinyImage::RGB_24_888,							//PALETTE32_4_COLOR
TinyImage::RGB_24_888,							//PALETTE32_16_COLOR
TinyImage::RGB_24_888,							//PALETTE32_256_COLOR
TinyImage::RGB_24_888,							//PALETTE24_4_COLOR
TinyImage::RGB_24_888,							//PALETTE24_16_COLOR
TinyImage::RGB_24_888								//PALETTE24_256_COLOR
} };

//! constructor
FixedImageBufferStream::FixedImageBufferStream(const std::string& name, CLASS_NAME_TREE_ARG) : FrameBufferStream(name, PASS_CLASS_NAME_TREE_ARG)
, mFilename(*this, false, "FileName", "")
, mNoConvert(*this, false, "NoConvert", false)
, mImageData(0)
{
	
}

FixedImageBufferStream::~FixedImageBufferStream()
{
	if (mImageData)
	{
		delete[] mImageData;
		mImageData = 0;
	}
}


void	FixedImageBufferStream::Process()
{
	int buffertoset = GetFreeBuffer();
	if (buffertoset == -1)
	{
		return;
	}
	SetBufferState(buffertoset, ProcessingBuffer);

	memcpy(mFrameBuffers[buffertoset]->buffer(), mImageData, mWidth * mHeight * mPixelSize);

	SetBufferState(buffertoset, ReadyBuffer);
}

void	FixedImageBufferStream::InitModifiable()
{
	if (mFilename.const_ref() != "")
	{
		
		SP<File::FileHandle> fullfilenamehandle;
		auto pathManager = KigsCore::Singleton<File::FilePathManager>();
		fullfilenamehandle = pathManager->FindFullName(mFilename.const_ref());

		if (fullfilenamehandle)
		{
			TinyImage::PushContext(gImageContext);
			auto image = TinyImage::CreateImage(fullfilenamehandle.get());
			if (image)
			{
				FrameBufferStream::InitModifiable();
				mWidth = image->GetWidth();
				mHeight = image->GetHeight();

				if (mImageData) delete[] mImageData;
				mImageData = nullptr;
				mImageData = new unsigned char[mWidth*mHeight*mPixelSize];

				if((bool)mNoConvert) 
				{
					// Ignore format, data in bmp file is not RGB we just want to reinterpret it as mFormat
					CopyFormat_24(image->GetPixelData(), mImageData, mWidth, mHeight);
				}
				else
				{
					switch ((int)mFormat)
					{
									// "RGB555"
						case 0:
						{
							ConvertFormat_RGB24ToRGB555(image->GetPixelData(), mImageData, mWidth, mHeight);
						}
						break;
									// "YUV422"
						case 2:
						{
							ConvertFormat_RGB24ToYUV422(image->GetPixelData(), mImageData, mWidth, mHeight);
						}
						break;
									// "RGB24"
						case 3:
						{
							CopyFormat_24(image->GetPixelData(), mImageData, mWidth, mHeight);
						}
						break;
									// "YUV24"
						case 5:
						{
							ConvertFormat_RGB24ToYUV24(image->GetPixelData(), mImageData, mWidth, mHeight);
						}
						break;

						default:
							KIGS_ERROR("Unsupported format", 0);
					}
				}
			}
			TinyImage::PopContext();
			if (mFrameBuffers[0])
				FreeFrameBuffers();

			AllocateFrameBuffers();
		}
	}
}

void FixedImageBufferStream::AllocateFrameBuffers()
{
	if (mWidth && mHeight)
	{
		unsigned int frameSize = mWidth*mHeight*mPixelSize;
		mFrameBuffers[0] = std::make_shared<AlignedCoreRawBuffer<16, unsigned char>>(frameSize, false); 
		mFrameBuffers[1] = std::make_shared<AlignedCoreRawBuffer<16, unsigned char>>(frameSize, false);
		mFrameBuffers[2] = std::make_shared<AlignedCoreRawBuffer<16, unsigned char>>(frameSize, false);
	}
}

void FixedImageBufferStream::FreeFrameBuffers()
{
	mFrameBuffers[0] = mFrameBuffers[1] = mFrameBuffers[2] = 0;
}