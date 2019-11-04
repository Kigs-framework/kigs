#ifndef _AUDIOLISTENERJS_H_
#define _AUDIOLISTENERJS_H_

#include "AudioListener.h"

class AudioListenerJS : public AudioListener
{
public:
    DECLARE_CLASS_INFO(AudioListenerJS,AudioListener,SoundManager)

    AudioListenerJS(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
  
protected:

	virtual void			Activate();
	virtual void			Desactivate();


	virtual		~AudioListenerJS();

};    

#endif //_AUDIOLISTENERJS_H_
