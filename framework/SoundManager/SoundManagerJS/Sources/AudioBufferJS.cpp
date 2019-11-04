#include "PrecompiledHeaders.h"
#include <algorithm>
#include <cctype>
#include "AudioBufferJS.h"
#include "AudioSourceJS.h"
#include "Core.h"

extern "C" bool		JSDeleteSound(int a_ID);
extern "C" void		JSStopSound(int a_ID);
extern "C" int		JSLoadSound(const char* a_SoundName, bool a_loop, float a_volume);
extern "C" float	JSPlaySound(int a_ID, bool loop, float volume);
extern "C" int		JSGetSoundState(int a_ID);
extern "C" void		JSSetVolume(int a_ID, float _volume);
extern "C" void		JSSetAllVolumes(float _volume);

IMPLEMENT_CLASS_INFO(AudioBufferJS)


AudioBufferJS::AudioBufferJS(const kstl::string& name,CLASS_NAME_TREE_ARG) : AudioBuffer(name,PASS_CLASS_NAME_TREE_ARG)
, myID(-1)
, myVolume(*this, false, LABEL_AND_ID(Volume),1.0)
, myLoop(*this, false, LABEL_AND_ID(Loop), false)
{
	myVolume.changeNotificationLevel(Owner);
}

AudioBufferJS::~AudioBufferJS()
{ 
	JSDeleteSound(myID);
}   

void		 AudioBufferJS::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == myVolume.getLabelID())
	{
		JSSetVolume(myID, myVolume);
	}
}

bool AudioBufferJS::LoadSound(const kstl::string& fullname)
{

	myID = JSLoadSound(fullname.c_str(), (bool)myLoop, (kfloat)myVolume);

	return true;
}

void  AudioBufferJS::Stream(AudioSource* src)
{
}

void  AudioBufferJS::Empty(AudioSource* src)
{
}


void AudioBufferJS::Play()
{
	JSPlaySound(myID, myLoop, myVolume);
}