#include "CameraVLC.h"

using ssize_t = int64_t;
#include "vlc/vlc.h"

libvlc_instance_t* gVLCInstance = nullptr;

void InitVLC(KigsCore* core)
{
	DECLARE_FULL_CLASS_INFO(core, MPEG4BufferStreamVLC, MPEG4BufferStream, CameraModule);
	DECLARE_FULL_CLASS_INFO(core, MPEG4EncoderVLC, MPEG4Encoder, CameraModule);

	char const *vlc_argv[] = {

	//	"--no-audio", // Don't play audio.
		"--no-xlib", // Don't use Xlib.
	};
	int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

	gVLCInstance = libvlc_new(vlc_argc, vlc_argv);
	if (NULL == gVLCInstance)
	{
		kigsprintf("LibVLC initialization failure.\n");
	}
}

void ShutdownVLC()
{
	libvlc_release(gVLCInstance);
}