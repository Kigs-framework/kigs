#include "PrecompiledHeaders.h"
#include "MPEG4BufferStreamVLC.h"
#include <algorithm>
#include <ModuleFileManager.h>


#include "CameraVLC.h"

using ssize_t = int64_t;
#include "vlc/vlc.h"


#include "XAudio2PCMPlayer.h"


// VLC prepares to render a video frame.
static void* lock(void *ctx, void **p_pixels)
{
	MPEG4BufferStreamVLC* instance = (MPEG4BufferStreamVLC*)ctx;
	return instance->Lock(p_pixels);
}


void* MPEG4BufferStreamVLC::Lock(void** pixels)
{
	//mMtx.lock();
	int index = GetFreeBuffer();
	void* ptr = GetBuffer(index);
	if (ptr == NULL)
	{
		index = GetReadyBufferIndex();
	}
	SetBufferState(index, CamBufferState::ProcessingBuffer);
	*pixels = GetBuffer(index);
	//mMtx.unlock();
	return (void*)index;
}

// VLC just rendered a video frame.
static void unlock(void *ctx, void *id, void *const *p_pixels)
{
	//MPEG4BufferStreamVLC* instance = (MPEG4BufferStreamVLC*)ctx;
	//instance->Unlock(p_pixels);
}

void MPEG4BufferStreamVLC::Unlock(void* const* pixels)
{
	//u8* rgba = (u8*)*pixels;
	
}

// VLC wants to display a video frame.
static void display(void *ctx, void *id)
{
	MPEG4BufferStreamVLC* instance = (MPEG4BufferStreamVLC*)ctx;
	instance->Display((int)id);
}

void MPEG4BufferStreamVLC::Display(int index)
{
	//mMtx.lock();
	SetBufferState(index, CamBufferState::ReadyBuffer);
	//mProcessingBuffer = -1;
	//mMtx.unlock();
}

IMPLEMENT_CLASS_INFO(MPEG4BufferStreamVLC)


//! constructor
IMPLEMENT_CONSTRUCTOR(MPEG4BufferStreamVLC)
{
}

//! destructor
MPEG4BufferStreamVLC::~MPEG4BufferStreamVLC()
{
	libvlc_media_player_stop(mp);
	libvlc_media_player_release(mp);

	if (myIsAllocated)
		FreeFrameBuffers();

	delete audio_player;
}

void MPEG4BufferStreamVLC::Start()
{
	if (libvlc_media_player_get_state(mp) == libvlc_state_t::libvlc_Paused)
	{
		libvlc_media_player_set_pause(mp, false);
	}
	else
	{
		libvlc_media_player_play(mp);
	}
}

void MPEG4BufferStreamVLC::Pause()
{
	libvlc_media_player_set_pause(mp, true);
}

void MPEG4BufferStreamVLC::Stop()
{
	libvlc_media_player_stop(mp);
}

void MPEG4BufferStreamVLC::Process()
{
	if (myHasReachEnd)
	{
		myHasReachEnd = false;
		EmitSignal(Signals::EndReached, this);
	}
	//myCurrentTime = myDuration * libvlc_media_player_get_position(mp);
	myCurrentTime = libvlc_media_player_get_time(mp)/1000.0;
}

void MPEG4BufferStreamVLC::SetPlaybackSpeed(float speed)
{
	libvlc_media_player_set_rate(mp, speed);
}

void MPEG4BufferStreamVLC::SetTime(kdouble t)
{
	libvlc_media_player_set_time(mp, t*1000.0);
}

void MPEG4BufferStreamVLC::NextFrame()
{
	libvlc_media_player_next_frame(mp);
}

void vlcEvent(const libvlc_event_t* event, void* user_data)
{
	auto vlc = (MPEG4BufferStreamVLC*)user_data;
	vlc->VLCEvent(event);
}

void MPEG4BufferStreamVLC::VLCEvent(const libvlc_event_t* event)
{
	if (event->type == libvlc_MediaPlayerEndReached)
	{
		myHasReachEnd = true;
		myState = Stopped;
	}
}

void MPEG4BufferStreamVLC::NotifyUpdate(const unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);
	if(labelid == mVolume.getID())
		libvlc_audio_set_volume(mp, 100 * mVolume);
}

void MPEG4BufferStreamVLC::InitModifiable()
{
	MPEG4BufferStream::InitModifiable();
	
	myWidth = 1860;
	myHeight = 1570;
	myFrameSize = myWidth * myHeight * 4;
	myLineSize = myWidth * 4;

	AllocateFrameBuffers();
	myIsAllocated = true;
	
	m = libvlc_media_new_path(gVLCInstance, myFileName.c_str());
	//libvlc_media_add_option(m, ":avcodec-hw=dxva2");


	

	mVolume.changeNotificationLevel(Owner);

	mp = libvlc_media_player_new_from_media(m);
	libvlc_media_release(m);

	libvlc_audio_set_volume(mp, 100 * mVolume);
	libvlc_video_set_callbacks(mp, lock, unlock, display, this);
	libvlc_video_set_format(mp, "RV32", myWidth, myHeight, myWidth * 4);
	libvlc_media_player_play(mp);

	evt_manager = libvlc_media_player_event_manager(mp);
	libvlc_event_attach(evt_manager, libvlc_MediaPlayerEndReached, vlcEvent, this);
	
	libvlc_time_t durr = -1;
	while(durr == -1)
		durr = libvlc_media_get_duration(m);

	myDuration = durr / 1000.0f;
}