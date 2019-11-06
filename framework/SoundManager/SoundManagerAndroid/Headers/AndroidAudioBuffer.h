#ifndef _ANDROIDAUDIOBUFFER_H_
#define _ANDROIDAUDIOBUFFER_H_

#include "AudioBuffer.h"
#include <jni.h>

/**
* \class AndroidAudioBuffer
* \brief manage an audio buffer for Android
*/
class AndroidAudioBuffer : public AudioBuffer
{
friend class NitroModuleSoundManager;
public:
	DECLARE_CLASS_INFO(AndroidAudioBuffer,AudioBuffer,SoundManager)
	
	/// Kigs constructor
	AndroidAudioBuffer(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/// method used for streaming/updating
	virtual void Stream(AudioSource* src);
	
	/// method used for clearing the stream
	virtual void Empty(AudioSource* src);
	
protected:
	/// destructor
	virtual ~AndroidAudioBuffer();
	
	/// main loading method
	virtual bool LoadSound(const kstl::string& fileName);
	
	jobject		mySoundResourceStruct;
	
	void update(AudioSource* audioSrc,bool play=false);
	
};


#endif //_ANDROIDAUDIOBUFFER_H_
