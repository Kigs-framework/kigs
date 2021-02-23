#pragma once

#include "MPEG4Encoder.h"
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"



class MPEG4EncoderVLC : public MPEG4Encoder
{
public:
	DECLARE_CLASS_INFO(MPEG4EncoderVLC, MPEG4Encoder, CameraModule);
	DECLARE_CONSTRUCTOR(MPEG4EncoderVLC);

	virtual void EncodeFrame(double Time, void* data, int dataLen, int stride = 0) override;
	virtual bool StartRecording() override;
	virtual void StopRecording() override;

	bool InitEncoder(const char * filename, bool flipInput) override;
	void CloseEncoder() override;

protected:
	void InitModifiable() override;
	~MPEG4EncoderVLC() override;

};
