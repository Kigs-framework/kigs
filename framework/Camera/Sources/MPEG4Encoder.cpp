#include "MPEG4Encoder.h"

using namespace Kigs::Camera;

IMPLEMENT_CLASS_INFO(MPEG4Encoder);

IMPLEMENT_CONSTRUCTOR(MPEG4Encoder)
, mSourceWidth(*this, false, "SourceWidth", 800)
, mSourceHeight(*this, false, "SourceHeight", 600)
, mTargetWidth(*this, false, "TargetWidth", 800)
, mTargetHeight(*this, false, "TargetHeight", 600)
, mFileName(*this, false, "FileName", "")
, mVFlip(*this, false, "VFlip", false)
, mSourceFormat(*this, false, "SourceFormat", "RGB24", "RGBA32", "YUV420P")
, mTargetFormat(*this, false, "TargetFormat", "RGB24", "RGBA32", "YUV420P")
{
}

void MPEG4Encoder::InitModifiable()
{
	CoreModifiable::InitModifiable();

	if (IsInit())
	{
	}
}


MPEG4Encoder::~MPEG4Encoder()
{

}