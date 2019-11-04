#ifndef _ANDROIDAUDIOLISTENER_H_
#define _ANDROIDAUDIOLISTENER_H_

#include "AudioListener.h"

/**
* \class AndroidAudioListenerActivator
* \brief same as AudioListenerActivator but doesn't compute velocity (for performance purposes)
*/
class AndroidAudioListenerActivator : public AudioListenerActivator
{
public:
	DECLARE_CLASS_INFO(AndroidAudioListenerActivator,AudioListenerActivator,SoundManager)
	
	/// Kigs constructor
	AndroidAudioListenerActivator(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/// compute time, position (but not velocity), and activate current listener
	virtual bool PreDraw(TravState*);
	
	friend class AudioListener;
};


/**
* \class AndroidAudioListener
* \brief manage an audio listener for Android
*/
class AndroidAudioListener : public AudioListener
{
public:
	DECLARE_CLASS_INFO(AndroidAudioListener,AudioListener,SoundManager)
	
	/// Kigs constructor
	AndroidAudioListener(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	friend class AndroidAudioListenerActivator;
	
protected:
	/// destructor
	virtual ~AndroidAudioListener();
	
	/// activate the listener
	virtual void Activate();
	
	/// desactivate the listener
	virtual void Desactivate();
	
};    

/*
inline code
*/
inline void AndroidAudioListener::Activate()    {myIsActive= true;}
inline void AndroidAudioListener::Desactivate() {myIsActive= false;}

#endif //_ANDROIDAUDIOLISTENER_H_

