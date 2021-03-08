#ifndef _MPEG4BufferStreamVLC_H_
#define _MPEG4BufferStreamVLC_H_

#include "mpeg4bufferstream.h"

#include "FilePathManager.h"
#include <memory>

struct libvlc_instance_t;
struct libvlc_media_t;
struct libvlc_media_player_t;
struct libvlc_event_manager_t;
struct libvlc_event_t;

class XAudio2PCMPlayer;

class MPEG4BufferStreamVLC : public MPEG4BufferStream
{
public:
	DECLARE_CLASS_INFO(MPEG4BufferStreamVLC, MPEG4BufferStream, CameraModule);
	DECLARE_CONSTRUCTOR(MPEG4BufferStreamVLC);
	SIGNALS(EndReached);

	virtual void InitModifiable() override;
	virtual void SetTime(kdouble t) override;
	virtual void NextFrame() override;

	virtual void Start() override;
	virtual void Pause() override;
	virtual void Stop() override;

	void SetPlaybackSpeed(float speed);


	void* Lock(void** pixels);
	void Unlock(void* const* pixels);
	void Display(int index);

	void VLCEvent(const libvlc_event_t* event);

protected:

	void NotifyUpdate(const unsigned int labelid) override;

	std::mutex mMtx;
	int mProcessingBuffer=-1;
	
	void Process() override;

	~MPEG4BufferStreamVLC() override;

	libvlc_media_t* m;
	libvlc_media_player_t* mp;
	libvlc_event_manager_t* evt_manager;

	XAudio2PCMPlayer* audio_player = nullptr;

	maFloat mVolume = BASE_ATTRIBUTE(Volume, 1.0f);
};

#endif //_MPEG4BufferStreamVLC_H_