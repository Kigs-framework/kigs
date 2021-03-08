#ifndef _FRAMEBUFFERSTREAM_H_
#define _FRAMEBUFFERSTREAM_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "Timer.h"
#include "CoreRawBuffer.h"
#include "SIMD.h"

#include <mutex>

enum	CamBufferState
{
	FreeBuffer,			// available for use
	ReadyBuffer,		// image is waiting there
	ProcessingBuffer			// someone else use the buffer
};

// ****************************************
// * FrameBufferStream class
// * --------------------------------------
/**
* \file	FrameBufferStream.h
* \class	FrameBufferStream
* \ingroup Camera
* \brief Manage an array of buffers to receive video stream.
*/
// ****************************************

class FrameBufferStream : public CoreModifiable
{
public:
	static constexpr size_t BUFFER_COUNT = 3u;
    DECLARE_ABSTRACT_CLASS_INFO(FrameBufferStream, CoreModifiable, CameraModule)
	DECLARE_CONSTRUCTOR(FrameBufferStream);

	void	Update(const Timer& /*timer*/, void* addParam) override;

	bool	addItem(const CMSP& item, ItemPosition pos = Last) override;
	bool	removeItem(const CMSP& item) override;

	// return a pointer on the buffer
	unsigned char*	GetBuffer(int index)
	{
		std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
		if(index!=-1)
		{
			return (unsigned char*)mFrameBuffers[index]->buffer();
		}
		return 0;
	}

	unsigned char*	GetConvertedBuffer(int index, unsigned char* buf = NULL, const kstl::string& format="");

	// return a new buffer to be delete
	unsigned char* GetRGB24Buffer(int index, unsigned char* buf=NULL);

	// retreive older ready buffer
	int	GetReadyBufferIndex()
	{
		std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
		int best_found=-1;
		kdouble best_time=-KFLOAT_ONE;

		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			if ((mBufferState[i] == ReadyBuffer) && ((mStateTime[i] < best_time) || (best_time == -KFLOAT_ONE)))
			{
				best_found = i;
				best_time = mStateTime[i];
			}
		}

		// no ready buffer
		return best_found;
	}

	// retreive newest ready buffer
	// and discard olders
	int	GetNewestReadyBufferIndex()
	{
		std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
		int best_found = -1;
		kdouble best_time = -KFLOAT_ONE;


		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			if ((mBufferState[i] == ReadyBuffer) && ((mStateTime[i]>best_time) || (best_time == -KFLOAT_ONE)))
			{
				best_found = i;
				best_time = mStateTime[i];
			}
		}

		if (best_found >= 0)
		{
			if (mBufferState[(best_found + 1) % BUFFER_COUNT] == ReadyBuffer)
				mBufferState[(best_found + 1) % BUFFER_COUNT] = FreeBuffer;

			if (mBufferState[(best_found + 2) % BUFFER_COUNT] == ReadyBuffer)
				mBufferState[(best_found + 2) % BUFFER_COUNT] = FreeBuffer;
		}
		// no ready buffer
		return best_found;
	}
	void	SetBufferState(int index,CamBufferState bstate)
	{
		std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
		mBufferState[index]=bstate;
		if(mTimer)
			mStateTime[index]=mTimer->GetTime();
	}

	virtual void ResetAllBuffers()
	{
		std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			mBufferState[i] = FreeBuffer;
			if (mTimer)
				mStateTime[i] = mTimer->GetTime();
		}
	}

	// Old
	virtual void	GetBufferSize(int& sizex,int& sizey)
	{
		sizex = mWidth;
		sizey = mHeight;
	}

	Point2DI GetBufferSize() const { return Point2DI((int)mWidth, (int)mHeight); } 


	// cla
	static inline unsigned char clampUC(int x)
	{
		return x < 0 ? 0 : (x > 255 ? 255 : x);
	}

	static inline void fastYUVtoRGB(unsigned char* YUV, unsigned char* RGB)
	{
		// use 16 bit floating point coef

		int Y = YUV[0] * 65536;
		int U = YUV[1] - 128;
		int V = YUV[2] - 128;

		const int c1 = (int)(1.4f * 65536.0f);
		const int c2 = (int)(-0.343f * 65536.0f);
		const int c3 = (int)(-0.711f * 65536.0f);
		const int c4 = (int)(1.765f * 65536.0f);

		RGB[0] = clampUC((Y + V * c1 + 32768) >> 16);
		RGB[1] = clampUC((Y + U * c2 + V * c3 + 32768) >> 16);
		RGB[2] = clampUC((Y + U * c4 + 32768) >> 16);
	}

	static inline void fastRGB24toRGB565(unsigned char* RGB24, unsigned char* RGB565)
	{
		unsigned short * tmp = reinterpret_cast<unsigned short*>(RGB565);
		tmp[0] = ((((RGB24[0] >> 3) << 11) | ((RGB24[1] >> 2) << 5) | (RGB24[2] >> 3)));
	}

	static inline void fastRGBtoYUV(unsigned char* YUV, unsigned char* RGB)
	{
		// use 16 bit floating point coef

		int R = RGB[0];
		int G = RGB[1];
		int B = RGB[2];

		const int c1 = (int)(0.299f * 65536.0f);
		const int c2 = (int)(0.587f * 65536.0f);
		const int c3 = (int)(0.114f * 65536.0f);
		const int c4 = (int)(-0.169f * 65536.0f);
		const int c5 = (int)(-0.331f * 65536.0f);
		const int c6 = (int)(0.5f * 65536.0f);
		const int c7 = (int)(-0.419f * 65536.0f);
		const int c8 = (int)(-0.081f * 65536.0f);

		YUV[0] = clampUC((R* c1 + G*c2 + B*c3 + 32768) >> 16);
		YUV[1] = clampUC(128 + ((R* c4 + G*c5 + B*c6 + 32768) >> 16));
		YUV[2] = clampUC(128 + ((R* c6 + G*c7 + B*c8 + 32768) >> 16));
	}

	// utility convert mFunc 
	static void ConvertFormat_420SPToRGB565(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_I420ToYUV422(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_RGB24ToYUV422(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_YUY2ToYUV422(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_YUV24ToYUV422(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_I420ToYUV24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_RGB24ToYUV24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_YUY2ToYUV24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_I420ToRGB24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_NV12ToRGB24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_NV12ToYUV24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_YUV24ToRGB24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_YUY2ToRGB24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_YUV422ToRGB24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_BGR24ToRGB24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_BGR24ToRGB32(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_BGR555ToRGB24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_RGB555ToRGB24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_BGR24ToRGB555(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_RGB24ToRGB555(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_RGB24ToRGB32(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_420SPToYUV24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_420SPToYUV422(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_420SPToRGB24(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_420SPToRGB32(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_YUV422ToRGB32(void* bufferin, void* bufferout, int sx, int sy);
	static void ConvertFormat_YUV24ToRGB32(void* bufferin, void* bufferout, int sx, int sy);

	static void CropBuffer_I420(void* bufferin, long& bufferlen,int sxin,int syin,int sxout,int syout,int offetx,int offsety);
	static void CropBuffer_RGB24(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety);
	static void CropBuffer_RGB32(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety);
	static void CropBuffer_YUY2(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety);
	static void CropBuffer_YUV24(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety);
	static void CropBuffer_BGR555(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety);
	static void CropBuffer_420SP(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety);

	static void CopyFormat_8 (void* bufferin, void* bufferout, int sx, int sy) { memcpy(bufferout, bufferin, sx * sy * 1); }
	static void CopyFormat_12(void* bufferin, void* bufferout, int sx, int sy) { memcpy(bufferout, bufferin, (sx * sy * 3)>>1); }
	static void CopyFormat_16(void* bufferin, void* bufferout, int sx, int sy) { memcpy(bufferout, bufferin, sx * sy * 2); }
	static void CopyFormat_24(void* bufferin, void* bufferout, int sx, int sy) { memcpy(bufferout, bufferin, sx * sy * 3); }
	static void CopyFormat_32(void* bufferin, void* bufferout, int sx, int sy) { memcpy(bufferout, bufferin, sx * sy * 4); }

	// utility method, not to be used to get all the pixel in image (just easy color picking)
	// return a Point3DI with each component between 0 an 255
	Point3DI	getYUVColorAtPixel(int px, int py, int bufferi);
	Point3DI	getRGBColorAtPixel(int px, int py, int bufferi);


	static inline void	FastSimdLineYUVtoRGB(unsigned char* read, unsigned char* write, int sx)
	{
		// simd on 8 byte chunks
		int sx8 = sx >> 3;
		int i;

#ifdef _M_ARM
		unsigned char* readchunk = (unsigned char*)__builtin_assume_aligned(read, 8);
		unsigned char* writechunk = (unsigned char*)__builtin_assume_aligned(write, 8);
#else
		unsigned char* readchunk = read;
		unsigned char* writechunk = write;
#endif

		int8x8_t c1 = vdup_n_s8((int8_t)(0.7f * 64.0f));
		int8x8_t c2 = vdup_n_s8((int8_t)(0.343f * 64.0f));
		int8x8_t c3 = vdup_n_s8((int8_t)(0.711f * 64.0f));
		int8x8_t c4 = vdup_n_s8((int8_t)(0.882f * 64.0f));

		int16x8_t cc1 = vdupq_n_u16(0.7f*8192.0f);
		int16x8_t cc2 = vdupq_n_u16(0.343f*8192.0f);
		int16x8_t cc3 = vdupq_n_u16(0.711f*8192.0f);
		int16x8_t cc4 = vdupq_n_u16(0.882f*8192.0f);

		for (i = 0; i < sx8; i++)
		{

			uint8x8x3_t	readYUV = vld3_u8(readchunk);
			uint8x8x3_t writeRGB;


			/*		unsigned char* YUV = readchunk;
			unsigned char* RGB = writechunk;

			int Y = YUV[0] * 65536;
			int U = YUV[1] - 128;
			int V = YUV[2] - 128;

			const int c1 = (int)(1.4f * 65536.0f);
			const int c2 = (int)(-0.343f * 65536.0f);
			const int c3 = (int)(-0.711f * 65536.0f);
			const int c4 = (int)(1.765f * 65536.0f);

			RGB[0] = ((Y + V * c1 + 32768) >> 16);
			RGB[1] = ((Y + U * c2 + V * c3 + 32768) >> 16);
			RGB[2] = ((Y + U * c4 + 32768) >> 16);*/

			// R computation
			int16x8_t  temp;
			temp = vshll_n_u8(readYUV.val[0], 6);
			temp = vmlal_u8(temp, readYUV.val[2], c1);
			temp = vsubq_s16(temp, cc1);
			temp = vmlal_u8(temp, readYUV.val[2], c1);
			temp = vsubq_s16(temp, cc1);

			writeRGB.val[0] = vqrshrun_n_s16(temp, 6);

			int16x8_t  temp1;
			// G computation
			temp1 = vshll_n_u8(readYUV.val[0], 6);
			temp1 = vaddq_s16(temp1, cc2);
			temp1 = vmlsl_u8(temp1, readYUV.val[1], c2);
			temp1 = vaddq_s16(temp1, cc3);
			temp1 = vmlsl_u8(temp1, readYUV.val[2], c3);

			writeRGB.val[1] = vqrshrun_n_s16(temp1, 6);

			// B computation
			temp = vshll_n_u8(readYUV.val[0], 6);
			temp = vmlal_u8(temp, readYUV.val[1], c4);
			temp = vsubq_s16(temp, cc4);
			temp = vmlal_u8(temp, readYUV.val[1], c4);
			temp = vsubq_s16(temp, cc4);

			writeRGB.val[2] = vqrshrun_n_s16(temp, 6);

			vst3_u8(writechunk, writeRGB);


			readchunk += 3 * 8;
			writechunk += 3 * 8;
		}

		// then compute remaining pixels

		sx -= sx8 << 3;
		for (i = 0; i < sx; i++)
		{
			FrameBufferStream::fastYUVtoRGB(readchunk, writechunk);
			writechunk+=3;
			readchunk+=3;
		}
	}

protected:

	void	InitModifiable() override;
	virtual void	Process() = 0;

	int	GetFreeBuffer()
	{
		std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			if (mBufferState[i] == FreeBuffer)
			{
				return i;
			}
		}

		return -1;
	}

	virtual	void AllocateFrameBuffers()=0;
	virtual	void FreeFrameBuffers()=0;

	CamBufferState								mBufferState[BUFFER_COUNT];
	SP<AlignedCoreRawBuffer<16, unsigned char>>	mFrameBuffers[BUFFER_COUNT];
	double										mStateTime[BUFFER_COUNT];
	float										mPixelSize = 2;

	FrameBufferStream*							mFrameBufferStream = nullptr;

	maUInt										mWidth = BASE_ATTRIBUTE(Width, 0);
	maUInt										mHeight = BASE_ATTRIBUTE(Height, 0);;
	maInt										mLineSize = BASE_ATTRIBUTE(LineSize, 0);;
	maEnum<7>									mFormat = BASE_ATTRIBUTE(Format, "RGB555", "RGB565", "YUV422", "RGB24", "RGB32", "YUV24", "NV12");

	Timer*										mTimer = nullptr;

	DECLARE_METHOD(GetReadyBuffer);
	DECLARE_METHOD(GetNewestReadyBuffer);
	DECLARE_METHOD(GetNewestReadyBufferRGB24);
	DECLARE_METHOD(HasReadyBuffer);
	DECLARE_METHOD(FreeBuffers);

	COREMODIFIABLE_METHODS(GetReadyBuffer, GetNewestReadyBuffer, GetNewestReadyBufferRGB24, HasReadyBuffer, FreeBuffers);
	std::recursive_mutex mBuffersMutex;
};

#endif //_FRAMEBUFFERSTREAM_H_
