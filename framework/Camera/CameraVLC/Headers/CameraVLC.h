#pragma once



#include "Core.h"
#include "MPEG4BufferStreamVLC.h"
#include "MPEG4EncoderVLC.h"

struct libvlc_instance_t;

extern libvlc_instance_t* gVLCInstance;

void InitVLC(KigsCore* core);
void ShutdownVLC();
