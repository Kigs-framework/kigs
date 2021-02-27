#include "MPEG4Encoder.h"


IMPLEMENT_CLASS_INFO(MPEG4Encoder);

IMPLEMENT_CONSTRUCTOR(MPEG4Encoder)
, mSourceWidth(*this, false, LABEL_AND_ID(SourceWidth), 800)
, mSourceHeight(*this, false, LABEL_AND_ID(SourceHeight), 600)
, mTargetWidth(*this, false, LABEL_AND_ID(TargetWidth), 800)
, mTargetHeight(*this, false, LABEL_AND_ID(TargetHeight), 600)
, mFileName(*this, false, LABEL_AND_ID(FileName), "")
, mVFlip(*this, false, LABEL_AND_ID(VFlip), false)
, mSourceFormat(*this, false, LABEL_AND_ID(SourceFormat), "RGB24", "RGBA32", "YUV420P")
, mTargetFormat(*this, false, LABEL_AND_ID(TargetFormat), "RGB24", "RGBA32", "YUV420P")
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