#include "MPEG4EncoderVLC.h"

#include "FilePathManager.h"



IMPLEMENT_CLASS_INFO(MPEG4EncoderVLC);

IMPLEMENT_CONSTRUCTOR(MPEG4EncoderVLC)
{
}

MPEG4EncoderVLC::~MPEG4EncoderVLC()
{
}

void MPEG4EncoderVLC::InitModifiable()
{
	ParentClassType::InitModifiable();
	if (IsInit())
	{
	}
}

bool MPEG4EncoderVLC::InitEncoder(const char * filename, bool flipInput)
{
	return false;
}

void MPEG4EncoderVLC::EncodeFrame(double Time, void* data, int dataLen, int stride)
{
	
}

void MPEG4EncoderVLC::CloseEncoder()
{
	
}

bool MPEG4EncoderVLC::StartRecording()
{
	return false;
}

void MPEG4EncoderVLC::StopRecording()
{

}
