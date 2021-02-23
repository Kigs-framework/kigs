#pragma once



#include "Core.h"
#include "MPEG4BufferStreamFFMPEG.h"
#include "MPEG4EncoderFFMPEG.h"


inline void InitFFMPEGClasses(KigsCore* core)
{
	DECLARE_FULL_CLASS_INFO(core, MPEG4BufferStreamFFMPEG, MPEG4BufferStream, CameraModule);
	DECLARE_FULL_CLASS_INFO(core, MPEG4EncoderFFMPEG, MPEG4Encoder, CameraModule);
}