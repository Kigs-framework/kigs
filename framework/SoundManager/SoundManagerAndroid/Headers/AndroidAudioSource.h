#ifndef _ANDROIDAUDIOSOURCE_H_
#define _ANDROIDAUDIOSOURCE_H_

#include "AudioSource.h"
#include <jni.h>

// state values
enum audio_state
{
	STOP  = 0,
	PLAY  = 1,
	PAUSE = 2
};


/**
* \class AndroidAudioSource
* \brief manage an audio source for Nitro
*/
class AndroidAudioSource : public AudioSource
{
public:
	DECLARE_CLASS_INFO(AndroidAudioSource,AudioSource,SoundManager)
	
	/// Kigs constructor
	AndroidAudioSource(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	friend class AndroidAudioBuffer;
	
protected:
	
	void NotifyUpdate(const unsigned int  labelid );
	
	/// destructor
	virtual ~AndroidAudioSource();
	
	/// method used to play sound data from current buffer
	virtual void Protected1Draw(TravState* state);
	
	void	Stop()
	{
		myState=STOP;
	}
	
		/// attenuation on/off
	maBool  myIsAttenuated;
	
	/// attenuation value (0~1)
	maFloat myAttenuation;
	
	/// pan on/off
	maBool myIsComputedPan;
	
	/// pan value (-1~1)
	maFloat myPan;
	
	jobject		mySoundStreamStruct;
	
	bool		wasPaused;
	
	// keep track of change so no need to call for update if no changes
	kfloat myLastVolume;
	kfloat myLastPitch;
	kfloat myLastLeftVolume;
};    

#endif //_ANDROIDAUDIOSOURCE_H_
