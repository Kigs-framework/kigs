#include "PrecompiledHeaders.h"
#include "FrameBufferStream.h"
#include "Core.h"

#include "Remotery.h"

using namespace Kigs::Camera;

IMPLEMENT_CLASS_INFO(FrameBufferStream)

IMPLEMENT_CONSTRUCTOR(FrameBufferStream)
{
	mBufferState[0] = mBufferState[1] = mBufferState[2] = FreeBuffer;
	mFrameBuffers[0] = mFrameBuffers[1] = mFrameBuffers[2] = 0;
	mStateTime[0] = mStateTime[1] = mStateTime[2] = 0.0;
}

void FrameBufferStream::Update(const Time::Timer& timer, void* addParam)
{
	if (mFrameBufferStream)
		mFrameBufferStream->CallUpdate(timer, addParam);

	Process();
}

void FrameBufferStream::InitModifiable()
{
	CoreModifiable::InitModifiable();

	switch (mFormat)
	{
	case 4: // RGB32
		mPixelSize = 4;
		break;
	case 3: // RGB24
	case 5: // YUV24
		mPixelSize = 3;
		break;
	case 6: // NV12
		mPixelSize = 1.5f;
		break;
	default: //RGB555, RGB565
		break;
	}
}

//! add item, if item is a rendering screen, set the given item as the used rendering screen (to be done before init)
bool FrameBufferStream::addItem(const CMSP& item, ItemPosition pos)
{
	if (item->isSubType("FrameBufferStream"))
	{
		mFrameBufferStream = (FrameBufferStream*)item.get();

		if (mFrameBufferStream->mTimer)
		{
			mTimer = mFrameBufferStream->mTimer;
		}
	}
	else if (item->isSubType("Timer"))
	{
		mTimer = (Time::Timer*)item.get();
	}

	return CoreModifiable::addItem(item, pos);

}

//! remove item, if item is a rendering screen, set rendering screen pointer to 0
bool FrameBufferStream::removeItem(const CMSP& item)
{
	if (item->isSubType("FrameBufferStream"))
	{
		if (mFrameBufferStream == item.get())
		{
			if (mTimer == mFrameBufferStream->mTimer)
			{
				mTimer = nullptr;
			}
			mFrameBufferStream = nullptr;
		}
	}
	else if (item->isSubType("Timer"))
	{
		if (mTimer == item.get())
			mTimer = nullptr;
	}

	return CoreModifiable::removeItem(item);

}

unsigned char*	FrameBufferStream::GetConvertedBuffer(int index, unsigned char* buf, const std::string& targetFormat)
{
	if (index != -1)
	{
		int sizex, sizey;
		GetBufferSize(sizex, sizey);
		if ((sizex > 0) && (sizey > 0))
		{
			std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
			unsigned char* newbuffer = buf;
			//"RGB555", "RGB565", "YUV422", "RGB24", "RGB32", "YUV24", "NV12"
			std::string format = (std::string)mFormat;
			if (targetFormat == "RGB24")
			{
				if (buf == NULL)
					newbuffer = new unsigned char[sizex*sizey * 3];

				if (format == "YUV422")
				{
					ConvertFormat_YUV422ToRGB24(mFrameBuffers[index]->buffer(), newbuffer, sizex, sizey);
				}
				else if (format == "YUV24")
				{
					ConvertFormat_YUV24ToRGB24(mFrameBuffers[index]->buffer(), newbuffer, sizex, sizey);
				}
				else if (format == "NV12")
				{
					ConvertFormat_NV12ToRGB24(mFrameBuffers[index]->buffer(), newbuffer, sizex, sizey);
				}
				else
					printf("convertion unmanaged\n");
			}
			if (targetFormat == "RGB32")
			{
				if (buf == NULL)
					newbuffer = new unsigned char[sizex*sizey * 4];
				if (format == "RGB24")
				{
					ConvertFormat_RGB24ToRGB32(mFrameBuffers[index]->buffer(), newbuffer, sizex, sizey);
				}
				else if (format == "YUV422")
				{
					ConvertFormat_YUV422ToRGB32(mFrameBuffers[index]->buffer(), newbuffer, sizex, sizey);
				}
				else if (format == "YUV24")
				{
					ConvertFormat_YUV24ToRGB32(mFrameBuffers[index]->buffer(), newbuffer, sizex, sizey);
				}
				else
					printf("convertion unmanaged\n");
			}
			else
				printf("convertion unmanaged\n");

			return newbuffer;
		}
	}
	return 0;
}

// return a new buffer to be delete
unsigned char* FrameBufferStream::GetRGB24Buffer(int index, unsigned char* buf)
{
	if (index != -1)
	{
		int sizex, sizey;
		GetBufferSize(sizex, sizey);
		if ((sizex > 0) && (sizey > 0))
		{
			std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
			//int imageSize=sizex*sizey*3;
			unsigned char* newbuffer;
			if (buf == NULL)
				newbuffer = new unsigned char[sizex*sizey * 3];
			else
				newbuffer = buf;

			std::string format = (std::string)mFormat;

			if (format == "RGB24")
			{
				CopyFormat_24(mFrameBuffers[index]->buffer(), newbuffer, sizex, sizey);
			}
			else if (format == "RGB555")
			{
				ConvertFormat_RGB555ToRGB24(mFrameBuffers[index]->buffer(), newbuffer, sizex, sizey);
			}
			else if (format == "YUV422")
			{
				ConvertFormat_YUV422ToRGB24(mFrameBuffers[index]->buffer(), newbuffer, sizex, sizey);
			}
			else if (format == "YUV24")
			{
				ConvertFormat_YUV24ToRGB24(mFrameBuffers[index]->buffer(), newbuffer, sizex, sizey);
			}
			else if (format == "NV12")
			{
				ConvertFormat_NV12ToRGB24(mFrameBuffers[index]->buffer(), newbuffer, sizex, sizey);
			}
			return newbuffer;
		}
	}
	return 0;
}

DEFINE_METHOD(FrameBufferStream, HasReadyBuffer)
{
	*((bool*)privateParams) = GetReadyBufferIndex() != -1;
	return false;
}

DEFINE_METHOD(FrameBufferStream, GetNewestReadyBufferRGB24)
{
	unsigned char* buffer = (unsigned char*)privateParams;
	int	index = GetNewestReadyBufferIndex();
	if (index != -1)
	{
		GetRGB24Buffer(index, buffer);
	}
	return false;
}

DEFINE_METHOD(FrameBufferStream, GetNewestReadyBuffer)
{
	unsigned char** return_value = (unsigned char**)privateParams;
	*return_value = nullptr;
	int	index = GetNewestReadyBufferIndex();
	if (index != -1)
	{
		*return_value = GetBuffer(index);
	}
	return false;
}

DEFINE_METHOD(FrameBufferStream, GetReadyBuffer)
{
	unsigned char** return_value = (unsigned char**)privateParams;
	*return_value = nullptr;
	int	index = GetReadyBufferIndex();
	if (index != -1)
	{
		*return_value = GetBuffer(index);
	}
	return false;
}

DEFINE_METHOD(FrameBufferStream, FreeBuffers)
{
	mBufferState[0] = FreeBuffer;
	mBufferState[1] = FreeBuffer;
	mBufferState[2] = FreeBuffer;
	return false;
}

// utility convert func 
// I420 have 3 seperate plane 
void FrameBufferStream::ConvertFormat_I420ToYUV422(void* bufferin, void* bufferout, int sx, int sy)
{
	unsigned char*	yplane = (unsigned char*)bufferin;
	unsigned char*	uplane = yplane + sx*sy;
	unsigned char*	vplane = uplane + (sx*sy) / 4;

	int i, j;

	for (j = 0; j < sy; j++)
	{

		unsigned int* writebuffer = (unsigned int*)(((unsigned short*)bufferout) + (sx*j));

		unsigned char*	readYplane = yplane + sx * j;
		int	uvoffset = (sx / 2)*(j / 2);

		for (i = 0; i < sx; i++)
		{

			unsigned int YVal = (unsigned int)(*readYplane);
			unsigned int UVal = (unsigned int)(*(uplane + uvoffset + i / 2));
			unsigned int VVal = (unsigned int)(*(vplane + uvoffset + i / 2));

			if (i & 1)
			{
				(*writebuffer) |= YVal << 16;
				writebuffer++;
			}
			else
			{
				*writebuffer = (YVal) | (UVal << 8) | (VVal << 24);
			}

			readYplane++;
		}
	}
}

void FrameBufferStream::ConvertFormat_RGB24ToYUV422(void* bufferin, void* bufferout, int sx, int sy)
{
	//TODO
}

void FrameBufferStream::ConvertFormat_YUY2ToYUV422(void* bufferin, void* bufferout, int sx, int sy)
{
	// is it the same ?
	memcpy(bufferout, bufferin, sx*sy * 2);
}

void FrameBufferStream::ConvertFormat_YUV24ToYUV422(void* bufferin, void* bufferout, int sx, int sy)
{
	// TODO
}

void FrameBufferStream::ConvertFormat_I420ToYUV24(void* bufferin, void* bufferout, int sx, int sy)
{
	unsigned char*	yplane = (unsigned char*)bufferin;
	unsigned char*	uplane = yplane + sx*sy;
	unsigned char*	vplane = uplane + (sx*sy) / 4;

	int i, j;
	unsigned char* writebuffer = (unsigned char*)bufferout;
	unsigned char* readYplane = yplane;

	for (j = 0; j < sy; j++)
	{
		int	uvoffset = (sx / 2)*(j / 2);

		for (i = 0; i < sx; i++)
		{
			writebuffer[0] = (*readYplane);
			writebuffer[1] = (*(uplane + uvoffset + i / 2));
			writebuffer[2] = (*(vplane + uvoffset + i / 2));

			writebuffer += 3;
			readYplane++;
		}
	}
}

void FrameBufferStream::ConvertFormat_NV12ToYUV24(void* bufferin, void* bufferout, int sx, int sy)
{
	// ! warning : no check to see if sx is multiple of 16 

	rmt_ScopedCPUSample(ConvertFormat_420SPToYUV24, 0);
	unsigned char* uplane = (unsigned char*)bufferin + sx*sy;
	int i, j;

	unsigned char* readYplane1 = (unsigned char*)bufferin;
	unsigned char* readYplane2 = (unsigned char*)bufferin + sx;
	unsigned char* readUplane = uplane;
	unsigned char* writebuffer1 = (unsigned char*)bufferout;
	unsigned char* writebuffer2 = (unsigned char*)bufferout + sx * 3;

	uint8x8_t	mask1;
	uint8x8_t	mask2;
	unsigned char* maskAddress = (unsigned char*)&mask1;
	maskAddress[0] = 0x01;
	maskAddress[1] = 0x01;
	maskAddress[2] = 0x03;
	maskAddress[3] = 0x03;
	maskAddress[4] = 0x05;
	maskAddress[5] = 0x05;
	maskAddress[6] = 0x07;
	maskAddress[7] = 0x07;

	maskAddress = (unsigned char*)&mask2;
	maskAddress[0] = 0x00;
	maskAddress[1] = 0x00;
	maskAddress[2] = 0x02;
	maskAddress[3] = 0x02;
	maskAddress[4] = 0x04;
	maskAddress[5] = 0x04;
	maskAddress[6] = 0x06;
	maskAddress[7] = 0x06;

	const int syDiv2 = sy >> 1;
	const int sxDiv8 = sx >> 3;

	for (j = 0; j < syDiv2; ++j) // two lines at once 
	{
		for (i = 0; i < sxDiv8; ++i) // 16 pixels at once
		{
			// read 2*8 bytes in uv plane
			uint8x8_t	readUV1 = vld1_u8(readUplane);
			uint8x8_t	readUV2;


			readUV2 = vtbl1_u8(readUV1, mask2);
			readUplane += 8;
			readUV1 = vtbl1_u8(readUV1, mask1);

			uint8x8_t readY1 = vld1_u8(readYplane1);


#if defined(__ARM_NEON)
			__builtin_neon_vst3_v(writebuffer1, readY1, readUV1, readUV2, 16);
#else
			uint8x8x3_t read1;
			read1.val[0] = readY1;
			read1.val[1] = readUV2;
			read1.val[2] = readUV1;

			vst3_u8(writebuffer1, read1);
#endif

			readY1 = vld1_u8(readYplane2);
			writebuffer1 += 3 * 8;
			readYplane1 += 8;


#if defined(__ARM_NEON)
			__builtin_neon_vst3_v(writebuffer2, readY1, readUV1, readUV2, 16);
#else

			read1.val[0] = readY1;
			vst3_u8(writebuffer2, read1);
#endif
			writebuffer2 += 3 * 8;
			readYplane2 += 8;
		}

		writebuffer1 += sx * 3;
		writebuffer2 += sx * 3;

		readYplane1 += sx;
		readYplane2 += sx;

	}
}


void FrameBufferStream::ConvertFormat_RGB24ToYUV24(void* bufferin, void* bufferout, int sx, int sy)
{
	unsigned char*	bufferread = ((unsigned char*)bufferin);
	unsigned char*	bufferwrite = ((unsigned char*)bufferout);

	for (int i = 0; i < sy; i++)
	{

		for (int j = 0; j < sx; j++)
		{
			fastRGBtoYUV(bufferwrite, bufferread);

			bufferread += 3;
			bufferwrite += 3;
		}


	}
}

void FrameBufferStream::ConvertFormat_YUY2ToYUV24(void* bufferin, void* bufferout, int sx, int sy)
{

	int indexyuv = 0;
	unsigned char* readyuv = (unsigned char*)bufferin;
	unsigned char* writeyuv = (unsigned char*)bufferout;

	int imageSize = sx*sy * 3;

	while (indexyuv < imageSize)
	{
		writeyuv[0] = readyuv[0];
		writeyuv[1] = readyuv[1];
		writeyuv[2] = readyuv[3];

		writeyuv += 3;

		writeyuv[0] = readyuv[2];
		writeyuv[1] = readyuv[1];
		writeyuv[2] = readyuv[3];

		writeyuv += 3;
		readyuv += 4;
		indexyuv += 6;
	}
}

void FrameBufferStream::ConvertFormat_I420ToRGB24(void* bufferin, void* bufferout, int sx, int sy)
{
	unsigned char YUV[3];
	unsigned char*	yplane = (unsigned char*)bufferin;
	unsigned char*	uplane = yplane + sx*sy;
	unsigned char*	vplane = uplane + (sx*sy) / 4;
	int i, j;

	unsigned char* readYplane = yplane;
	unsigned char* writebuffer = (unsigned char*)bufferout;

	for (j = 0; j < sy; j++)
	{
		int	uvoffset = (sx >> 1)*(j >> 1);

		for (i = 0; i < sx; i++)
		{

			YUV[0] = (unsigned int)(*readYplane);
			YUV[1] = (unsigned int)(*(uplane + uvoffset + i / 2));
			YUV[2] = (unsigned int)(*(vplane + uvoffset + i / 2));

			fastYUVtoRGB(YUV, writebuffer);
			writebuffer += 3;
			readYplane++;
		}
	}
}


void FrameBufferStream::ConvertFormat_NV12ToRGB24(void* bufferin, void* bufferout, int sx, int sy)
{
	unsigned char YUV[3];
	unsigned char* uplane = (unsigned char*)bufferin + sx*sy;
	int i, j;

	unsigned char* readYplane = (unsigned char*)bufferin;
	unsigned char* readUplane;
	unsigned char* writebuffer = (unsigned char*)bufferout;

	for (j = 0; j < sy; j++)
	{
		int	uvoffset = sx*(j >> 1);

		for (i = 0; i < sx; i++)
		{
			readUplane = uplane + (uvoffset + (i & 0xFFFFFFFE));

			YUV[0] = (*readYplane);
			YUV[1] = readUplane[0];
			YUV[2] = readUplane[1];

			fastYUVtoRGB(YUV, writebuffer);
			writebuffer += 3;
			readYplane++;
		}
	}
}

void FrameBufferStream::ConvertFormat_420SPToRGB32(void* bufferin, void* bufferout, int sx, int sy)
{
	unsigned char YUV[3];
	unsigned char* uplane = (unsigned char*)bufferin + sx*sy;
	int i, j;

	unsigned char* readYplane = (unsigned char*)bufferin;
	unsigned char* readUplane;
	unsigned char* writebuffer = (unsigned char*)bufferout;

	for (j = 0; j < sy; j++)
	{
		int	uvoffset = sx*(j >> 1);

		for (i = 0; i < sx; i++)
		{
			readUplane = uplane + (uvoffset + (i & 0xFFFFFFFE));

			YUV[0] = (*readYplane);
			YUV[1] = readUplane[1];
			YUV[2] = readUplane[0];

			fastYUVtoRGB(YUV, writebuffer);
			writebuffer[3] = 255;
			writebuffer += 4;
			readYplane++;
		}
	}
}


void FrameBufferStream::ConvertFormat_420SPToRGB24(void* bufferin, void* bufferout, int sx, int sy)
{
	unsigned char YUV[3];
	unsigned char* uplane = (unsigned char*)bufferin + sx*sy;
	int i, j;

	unsigned char* readYplane = (unsigned char*)bufferin;
	unsigned char* readUplane;
	unsigned char* writebuffer = (unsigned char*)bufferout;

	for (j = 0; j < sy; j++)
	{
		int	uvoffset = sx*(j >> 1);

		for (i = 0; i < sx; i++)
		{
			readUplane = uplane + (uvoffset + (i & 0xFFFFFFFE));

			YUV[0] = (*readYplane);
			YUV[1] = readUplane[1];
			YUV[2] = readUplane[0];

			fastYUVtoRGB(YUV, writebuffer);
			writebuffer += 3;
			readYplane++;
		}
	}
}

void FrameBufferStream::ConvertFormat_420SPToRGB565(void* bufferin, void* bufferout, int sx, int sy)
{
	unsigned char YUV[3];
	unsigned char* uplane = (unsigned char*)bufferin + sx*sy;
	int i, j;

	unsigned char* readYplane = (unsigned char*)bufferin;
	unsigned char* readUplane;
	unsigned char* writebuffer = (unsigned char*)bufferout;

	unsigned char tmp[3];

	for (j = 0; j < sy; j++)
	{
		int	uvoffset = sx*(j >> 1);

		for (i = 0; i < sx; i++)
		{
			readUplane = uplane + (uvoffset + (i & 0xFFFFFFFE));

			YUV[0] = (*readYplane);
			YUV[1] = readUplane[1];
			YUV[2] = readUplane[0];


			fastYUVtoRGB(YUV, tmp);
			fastRGB24toRGB565(tmp, writebuffer);


			writebuffer += 2;
			readYplane++;
		}
	}
}

void FrameBufferStream::ConvertFormat_YUV24ToRGB32(void* bufferin, void* bufferout, int sx, int sy)
{
	// convert
	int indexrgb = 0;
	unsigned char* readyuv = (unsigned char*)bufferin;
	unsigned char* writergb = (unsigned char*)bufferout;
	int imageSize = sx*sy * 4;

	while (indexrgb < imageSize)
	{
		fastYUVtoRGB(readyuv, writergb);
		writergb[3] = 255;
		writergb += 4;
		readyuv += 3;
		indexrgb += 4;
	}
}

void FrameBufferStream::ConvertFormat_420SPToYUV422(void* bufferin, void* bufferout, int sx, int sy)
{

}

void FrameBufferStream::ConvertFormat_420SPToYUV24(void* bufferin, void* bufferout, int sx, int sy)
{
	// ! warning : no check to see if sx is multiple of 16 
	
	rmt_ScopedCPUSample(ConvertFormat_420SPToYUV24, 0);
	unsigned char* uplane = (unsigned char*)bufferin + sx*sy;
	int i, j;

	unsigned char* readYplane1 = (unsigned char*)bufferin;
	unsigned char* readYplane2 = (unsigned char*)bufferin+sx;
	unsigned char* readUplane=uplane;
	unsigned char* writebuffer1 = (unsigned char*)bufferout;
	unsigned char* writebuffer2 = (unsigned char*)bufferout+sx*3;

	uint8x8_t	mask1;
	uint8x8_t	mask2;
	unsigned char* maskAddress = (unsigned char*)&mask1;
	maskAddress[0] = 0x01;
	maskAddress[1] = 0x01;
	maskAddress[2] = 0x03;
	maskAddress[3] = 0x03;
	maskAddress[4] = 0x05;
	maskAddress[5] = 0x05;
	maskAddress[6] = 0x07;
	maskAddress[7] = 0x07;

	maskAddress = (unsigned char*)&mask2;
	maskAddress[0] = 0x00;
	maskAddress[1] = 0x00;
	maskAddress[2] = 0x02;
	maskAddress[3] = 0x02;
	maskAddress[4] = 0x04;
	maskAddress[5] = 0x04;
	maskAddress[6] = 0x06;
	maskAddress[7] = 0x06;

	const int syDiv2 = sy >> 1;
	const int sxDiv8 = sx >> 3;

	for (j = 0; j < syDiv2; ++j) // two lines at once 
	{
		for (i = 0; i < sxDiv8; ++i) // 16 pixels at once
		{
			// read 2*8 bytes in uv plane
			uint8x8_t	readUV1 = vld1_u8(readUplane);				
			uint8x8_t	readUV2;


			readUV2 = vtbl1_u8(readUV1, mask2); 
			readUplane += 8;
			readUV1 = vtbl1_u8(readUV1, mask1);

			uint8x8_t readY1 = vld1_u8(readYplane1);


#if defined(__ARM_NEON)
			__builtin_neon_vst3_v(writebuffer1, readY1, readUV1, readUV2, 16);
#else
			uint8x8x3_t read1;
			read1.val[0] = readY1;
			read1.val[1] = readUV1;
			read1.val[2] = readUV2;

			vst3_u8(writebuffer1, read1);
#endif

			readY1 = vld1_u8(readYplane2);
			writebuffer1 += 3 * 8;
			readYplane1 += 8;

			
#if defined(__ARM_NEON)
			__builtin_neon_vst3_v(writebuffer2, readY1, readUV1, readUV2, 16);
#else
			
			read1.val[0] = readY1;
			vst3_u8(writebuffer2, read1);
#endif
			writebuffer2 += 3 * 8;
			readYplane2+=8;
		}

		writebuffer1 += sx * 3;
		writebuffer2 += sx * 3;

		readYplane1 += sx;
		readYplane2 += sx;

	}

}

void FrameBufferStream::ConvertFormat_YUV24ToRGB24(void* bufferin, void* bufferout, int sx, int sy)
{
	// convert
	unsigned char* readyuv = (unsigned char*)bufferin;
	unsigned char* writergb = (unsigned char*)bufferout;
	int lineSize = sx* 3;


	for (int y = 0; y < sy; ++y)
	{

		FastSimdLineYUVtoRGB((unsigned char*)readyuv, (unsigned char*)writergb, sx);
		readyuv += lineSize;
		writergb += lineSize;
	}

}
void FrameBufferStream::ConvertFormat_YUY2ToRGB24(void* bufferin, void* bufferout, int sx, int sy)
{
	// TODO
}

void FrameBufferStream::ConvertFormat_BGR24ToRGB24(void* bufferin, void* bufferout, int sx, int sy)
{
	// bgr to rgb
	unsigned char*	bufferOffset = ((unsigned char*)bufferin);

	for (int i = 0; i < sy; i++)
	{
		unsigned char* writebuffer = (((unsigned char*)bufferout) + 3 * (sx*i));

		int jmult = 0;
		for (int j = 0; j < sx; j++)
		{
			*writebuffer++ = bufferOffset[jmult + 2];
			*writebuffer++ = bufferOffset[jmult + 1];
			*writebuffer++ = bufferOffset[jmult];

			jmult += 3;
		}

		// next line
		bufferOffset += sx * 3;
	}
}

void FrameBufferStream::ConvertFormat_BGR24ToRGB32(void* bufferin, void* bufferout, int sx, int sy)
{
	// bgr to rgb
	unsigned char*	bufferOffset = ((unsigned char*)bufferin);

	for (int i = 0; i < sy; i++)
	{
		unsigned char* writebuffer = (((unsigned char*)bufferout) + 4 * (sx*i));

		int jmult = 0;
		for (int j = 0; j < sx; j++)
		{
			*writebuffer++ = bufferOffset[jmult + 2];
			*writebuffer++ = bufferOffset[jmult + 1];
			*writebuffer++ = bufferOffset[jmult];
			*writebuffer++ = 255;

			jmult += 4;
		}

		// next line
		bufferOffset += sx * 4;
	}
}

void FrameBufferStream::ConvertFormat_BGR555ToRGB24(void* bufferin, void* bufferout, int sx, int sy)
{
	// TODO
}
void FrameBufferStream::ConvertFormat_BGR24ToRGB555(void* bufferin, void* bufferout, int sx, int sy)
{
	// bgr to rgb
	unsigned char*	bufferOffset = ((unsigned char*)bufferin);

	for (int i = 0; i < sy; i++)
	{
		unsigned short* writebuffer = (((unsigned short*)bufferout) + (sx*i));

		int jmult = 0;
		for (int j = 0; j < sx; j++)
		{
			*writebuffer++ = (bufferOffset[jmult] >> 3) | ((bufferOffset[jmult + 1] >> 3) << 5) | ((bufferOffset[jmult + 2] >> 3) << 10) | 0x8000;
			jmult += 3;
		}

		// next line
		bufferOffset += sx * 3;
	}
}

void FrameBufferStream::ConvertFormat_RGB24ToRGB555(void* bufferin, void* bufferout, int sx, int sy)
{
	// bgr to rgb
	unsigned char*	bufferOffset = ((unsigned char*)bufferin);

	for (int i = 0; i < sy; i++)
	{
		unsigned short* writebuffer = (((unsigned short*)bufferout) + (sx*i));

		int jmult = 0;
		for (int j = 0; j < sx; j++)
		{
			*writebuffer++ = (bufferOffset[jmult + 2] >> 3) | ((bufferOffset[jmult + 1] >> 3) << 5) | ((bufferOffset[jmult] >> 3) << 10) | 0x8000;
			jmult += 3;
		}

		// next line
		bufferOffset += sx * 3;
	}
}

void FrameBufferStream::ConvertFormat_RGB24ToRGB32(void* bufferin, void* bufferout, int sx, int sy)
{
	// bgr to rgb
	unsigned char* bufferOffset = ((unsigned char*)bufferin);
	unsigned char* writebuffer =  ((unsigned char*)bufferout);

	int imageSize = sx*sy * 4;
	int indexrgb = 0;
	while (indexrgb < imageSize)
	{
		writebuffer[0] = bufferOffset[0];
		writebuffer[1] = bufferOffset[1];
		writebuffer[2] = bufferOffset[2];
		writebuffer[3] = 255;

		bufferOffset += 3;
		writebuffer += 4;
		indexrgb += 4;
	}
}

void FrameBufferStream::ConvertFormat_YUV422ToRGB24(void* bufferin, void* bufferout, int sx, int sy)
{
	unsigned char YUV[3];
	int indexrgb = 0;
	unsigned char* readyuv = (unsigned char*)bufferin;
	unsigned char* writergb = (unsigned char*)bufferout;

	int imageSize = sx*sy * 3;

	while (indexrgb < imageSize)
	{
		YUV[0] = readyuv[0];
		YUV[1] = readyuv[1];
		YUV[2] = readyuv[3];

		fastYUVtoRGB(YUV, writergb);
		writergb += 3;

		YUV[0] = readyuv[2];
		fastYUVtoRGB(YUV, writergb);
		writergb += 3;
		readyuv += 4;
		indexrgb += 6;
	}
}

void FrameBufferStream::ConvertFormat_YUV422ToRGB32(void* bufferin, void* bufferout, int sx, int sy)
{
	unsigned char YUV[3];
	int indexrgb = 0;
	unsigned char* readyuv = (unsigned char*)bufferin;
	unsigned char* writergb = (unsigned char*)bufferout;

	int imageSize = sx*sy * 4;

	while (indexrgb < imageSize)
	{
		YUV[0] = readyuv[0];
		YUV[1] = readyuv[1];
		YUV[2] = readyuv[3];

		fastYUVtoRGB(YUV, writergb);
		writergb[3] = 255;
		writergb += 4;

		YUV[0] = readyuv[2];
		fastYUVtoRGB(YUV, writergb);
		writergb[3] = 255;
		writergb += 4;
		readyuv += 4;
		indexrgb += 8;
	}
}

void ConvertFormat_YUV24ToRGB32(void* bufferin, void* bufferout, int sx, int sy);

void FrameBufferStream::ConvertFormat_RGB555ToRGB24(void* bufferin, void* bufferout, int sx, int sy)
{
	unsigned short*	bufferOffset = ((unsigned short*)bufferin);

	for (int i = 0; i < sy; i++)
	{
		unsigned char* writebuffer = (((unsigned char*)bufferout) + (sx*i * 3));

		for (int j = 0; j < sx; j++)
		{
			unsigned short pix = bufferOffset[j];

			unsigned short R = (pix & 0x7C00) >> 7;
			unsigned short G = (pix & 0x03E0) >> 2;
			unsigned short B = (pix & 0x001F) << 3;

			*writebuffer++ = (unsigned char)R;
			*writebuffer++ = (unsigned char)G;
			*writebuffer++ = (unsigned char)B;
		}

		// next line
		bufferOffset += sx;
	}
}

Point3DI	FrameBufferStream::getYUVColorAtPixel(int px, int py, int bufferi)
{
	Point3DI	result(0, 0, 0);

	int sizex, sizey;
	GetBufferSize(sizex, sizey);

	unsigned char* currentBuffer = (unsigned char*)mFrameBuffers[bufferi]->buffer();

	// first check if in image
	if ((px >= 0) && (px < sizex) && (py >= 0) && (py < sizey))
	{

		switch ((int)mFormat)
		{
			// "RGB555"
		case 0:
		{
			// get pixel 
			unsigned short*	pixel = ((unsigned short*)currentBuffer) + sizex*py + px;

			// get RGB
			unsigned char RGB[3];

			RGB[0] = ((*pixel) >> 7) & 0xF8;
			RGB[1] = ((*pixel) >> 2) & 0xF8;
			RGB[2] = ((*pixel) << 3) & 0xF8;

			unsigned char YUV[3];

			fastRGBtoYUV(YUV, RGB);

			result.Set(YUV[0], YUV[1], YUV[2]);
		}
		break;
		// "YUV422"
		case 1:
		{
			unsigned short*	pixel = ((unsigned short*)currentBuffer) + sizex*py + ((px >> 1) << 1);
			unsigned char* readyuv = (unsigned char*)pixel;

			unsigned char YUV[3];
			if (px & 1)
			{
				YUV[0] = readyuv[2];
				YUV[1] = readyuv[1];
				YUV[2] = readyuv[3];
			}
			else
			{
				YUV[0] = readyuv[0];
				YUV[1] = readyuv[1];
				YUV[2] = readyuv[3];
			}
			result.Set(YUV[0], YUV[1], YUV[2]);

		}
		break;
		// "RGB24"
		case 2:
		{
			// get pixel 
			unsigned char*	RGB = currentBuffer + (sizex*py + px) * 3;
			unsigned char YUV[3];

			fastRGBtoYUV(YUV, RGB);

			result.Set(YUV[0], YUV[1], YUV[2]);
		}
		break;
		// "YUV24"
		case 3:
		{
			// get pixel 
			unsigned char*	YUV = currentBuffer + (sizex*py + px) * 3;
			result.Set(YUV[0], YUV[1], YUV[2]);
		}
		break;
		}
	}

	return result;
}
Point3DI	FrameBufferStream::getRGBColorAtPixel(int px, int py, int bufferi)
{
	Point3DI	result(0, 0, 0);

	int sizex, sizey;
	GetBufferSize(sizex, sizey);

	unsigned char* currentBuffer = (unsigned char*)mFrameBuffers[bufferi]->buffer();

	// first check if in image
	if ((px >= 0) && (px < sizex) && (py >= 0) && (py < sizey))
	{

		switch ((int)mFormat)
		{
			// "RGB555"
		case 0:
		{
			// get pixel 
			unsigned short*	pixel = ((unsigned short*)currentBuffer) + sizex*py + px;

			// get RGB
			unsigned char RGB[3];

			RGB[0] = ((*pixel) >> 7) & 0xF8;
			RGB[1] = ((*pixel) >> 2) & 0xF8;
			RGB[2] = ((*pixel) << 3) & 0xF8;

			result.Set(RGB[0], RGB[1], RGB[2]);
		}
		break;
		// "YUV422"
		case 1:
		{
			unsigned short*	pixel = ((unsigned short*)currentBuffer) + sizex*py + ((px >> 1) << 1);
			unsigned char* readyuv = (unsigned char*)pixel;

			unsigned char YUV[3];
			if (px & 1)
			{
				YUV[0] = readyuv[2];
				YUV[1] = readyuv[1];
				YUV[2] = readyuv[3];
			}
			else
			{
				YUV[0] = readyuv[0];
				YUV[1] = readyuv[1];
				YUV[2] = readyuv[3];
			}

			unsigned char RGB[3];
			fastYUVtoRGB(YUV, RGB);

			result.Set(RGB[0], RGB[1], RGB[2]);

		}
		break;
		// "RGB24"
		case 2:
		{
			// get pixel 
			unsigned char*	RGB = currentBuffer + (sizex*py + px) * 3;
			result.Set(RGB[0], RGB[1], RGB[2]);
		}
		break;
		// "YUV24"
		case 3:
		{
			// get pixel 
			unsigned char*	YUV = currentBuffer + (sizex*py + px) * 3;
			unsigned char RGB[3];
			fastYUVtoRGB(YUV, RGB);

			result.Set(RGB[0], RGB[1], RGB[2]);
		}
		break;
		}
	}

	return result;
}

// use memmove as we cannot guarantee buffer are not overlapping
void FrameBufferStream::CropBuffer_I420(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety)
{


	// treat the three buffers independently
	// y is one byte per pixel
	unsigned char*	yplane = (unsigned char*)bufferin;
	unsigned char*  yread = yplane + offetx + offsety*sxin;
	unsigned char*  write = yplane;

	int j;

	for (j = 0; j < syout; j++)
	{
		memmove(write, yread, sxout);
		write += sxout;
		yread += sxin;
	}

	// uplane is 1 byte for 2x2 pixel block
	unsigned char*	uplane = yplane + sxin*syin;
	unsigned char*  uread = uplane + (offetx >> 1) + ((offsety >> 1)*(sxin >> 1));
	for (j = 0; j < (syout >> 1); j++)
	{
		memmove(write, uread, (sxout >> 1));
		write += sxout >> 1;
		uread += sxin >> 1;
	}

	// vplane is 1 byte for 2x2 pixel block
	unsigned char*	vplane = uplane + ((sxin*syin) >> 2);
	unsigned char*  vread = vplane + (offetx >> 1) + ((offsety >> 1)*(sxin >> 1));
	for (j = 0; j < (syout >> 1); j++)
	{
		memmove(write, vread, (sxout >> 1));
		write += sxout >> 1;
		vread += sxin >> 1;
	}

	bufferlen = (sxout*syout) + (((sxout*syout) >> 2) << 1);
}

void FrameBufferStream::CropBuffer_RGB24(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety)
{
	unsigned char*	rgbplane = (unsigned char*)bufferin;
	unsigned char*  rgbread = rgbplane + (offetx + offsety*sxin) * 3;
	unsigned char*  write = rgbplane;

	int j;

	for (j = 0; j < syout; j++)
	{
		memmove(write, rgbread, sxout * 3);
		write += sxout * 3;
		rgbread += sxin * 3;
	}

	bufferlen = sxout*syout * 3;
}

void FrameBufferStream::CropBuffer_RGB32(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety)
{
	unsigned char*	rgbplane = (unsigned char*)bufferin;
	unsigned char*  rgbread = rgbplane + (offetx + offsety*sxin) * 4;
	unsigned char*  write = rgbplane;

	int j;

	for (j = 0; j < syout; j++)
	{
		memmove(write, rgbread, sxout * 4);
		write += sxout * 4;
		rgbread += sxin * 4;
	}

	bufferlen = sxout*syout * 4;
}

void FrameBufferStream::CropBuffer_YUY2(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety)
{
	//int indexyuv = 0;
	unsigned char* yuvbuffer = (unsigned char*)bufferin;
	unsigned char* yuvread = yuvbuffer + (((offetx >> 1) + ((offsety*sxin) >> 1)) << 2);

	unsigned char* write = (unsigned char*)yuvbuffer;

	int j;

	for (j = 0; j < syout; j++)
	{
		memmove(write, yuvread, sxout * 2);
		write += sxout * 2;
		yuvread += sxin * 2;
	}

	bufferlen = sxout*syout * 2;

}
void FrameBufferStream::CropBuffer_YUV24(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety)
{
	// just call CropBuffer_RGB24
	CropBuffer_RGB24(bufferin, bufferlen, sxin, syin, sxout, syout, offetx, offsety);
}
void FrameBufferStream::CropBuffer_BGR555(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety)
{
	unsigned char*	rgbplane = (unsigned char*)bufferin;
	unsigned char*  rgbread = rgbplane + (offetx + offsety*sxin) * 2;
	unsigned char*  write = rgbplane;

	int j;

	for (j = 0; j < syout; j++)
	{
		memmove(write, rgbread, sxout * 2);
		write += sxout * 2;
		rgbread += sxin * 2;
	}

	bufferlen = sxout*syout * 2;
}
void FrameBufferStream::CropBuffer_420SP(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety)
{
	// treat the three buffers independently
	// y is one byte per pixel
	unsigned char*	yplane = (unsigned char*)bufferin;
	unsigned char*  yread = yplane + offetx + offsety*sxin;
	unsigned char*  write = yplane;

	int j;

	for (j = 0; j < syout; j++)
	{
		memmove(write, yread, sxout);
		write += sxout;
		yread += sxin;
	}

	// uplane is 1 byte for 2x2 pixel block
	unsigned char*	uplane = yplane + sxin*syin;
	unsigned char*  uread = uplane + (offetx)+((offsety >> 1)*sxin);
	for (j = 0; j < (syout >> 1); j++)
	{
		memmove(write, uread, (sxout));
		write += sxout;
		uread += sxin;
	}

	/*// vplane is 1 byte for 2x2 pixel block
	unsigned char*	vplane = uplane + ((sxin*syin) >> 2);
	unsigned char*  vread = vplane + (offetx >> 1) + ((offsety >> 1)*(sxin >> 1));
	for (j = 0; j < (syout >> 1); j++)
	{
		memmove(write, vread, (sxout >> 1));
		write += sxout >> 1;
		vread += sxin >> 1;
	}*/

	bufferlen = (sxout*syout) + (((sxout*syout) >> 2) << 1);
}