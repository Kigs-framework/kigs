#pragma once

#include "MPEG4Encoder.h"
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"

struct AVFormatContext;
struct AVFrame;
struct AVPacket;
struct SwsContext;
struct EncoderInfo
{
	double lastTime = 0;
	AVFormatContext *oc;
	AVFrame *inframe;
	AVFrame *outframe;
	SwsContext*Sws;
	AVPacket* pkt;
	unsigned int PixFormat;
	int width;
	int height;
	bool FlippedInput;
	void * InputTarget;
};

struct AVStream;
struct SwsContext;
struct AVCodecContext;
struct AVIOContext;


class MPEG4EncoderFFMPEG : public MPEG4Encoder
{
public:
	DECLARE_CLASS_INFO(MPEG4EncoderFFMPEG, MPEG4Encoder, CameraModule);
	DECLARE_CONSTRUCTOR(MPEG4EncoderFFMPEG);

	virtual void EncodeFrame(double Time, void* data, int dataLen, int stride = 0) override;
	virtual bool StartRecording() override;
	virtual void StopRecording() override;

	bool InitEncoder(const char * filename, bool flipInput) override;
	void CloseEncoder() override;

protected:
	void InitModifiable() override;
	~MPEG4EncoderFFMPEG() override;
	

	EncoderInfo mEncoderInfo;

	
	long long mLastFramePts = -1;

	bool mIsScaleNeeded;

	bool	mIsRecording;
	bool	mNeedCloseEncoder;
	
	kstl::vector<unsigned char> mTempBuffer;
};
