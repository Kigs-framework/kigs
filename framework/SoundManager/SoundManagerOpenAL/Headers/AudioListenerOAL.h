#ifndef _AUDIOLISTENEROAL_H_
#define _AUDIOLISTENEROAL_H_

#include "AudioListener.h"

class AudioListenerOAL : public AudioListener
{
public:
    DECLARE_CLASS_INFO(AudioListenerOAL,AudioListener,SoundManager)

    AudioListenerOAL(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
  
protected:

	virtual void			Activate();
	virtual void			Desactivate();


	virtual		~AudioListenerOAL();

};    

#endif //_AUDIOLISTENEROAL_H_
