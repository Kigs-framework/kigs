#ifndef _AUDIOBUFFERMODJS_H_
#define _AUDIOBUFFERMODJS_H_

#ifdef USEMIKMOD
#include "AudioBufferJS.h"
#include "al.h"
#include "mikmod.h"


class AudioBufferModJS : public AudioBufferJS
{
public:
    DECLARE_CLASS_INFO(AudioBufferModJS,AudioBufferJS,SoundManager)

    AudioBufferModJS(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
  
    virtual void  Stream(AudioSource* src);
 
protected:

  virtual bool LoadSound(const kstl::string&);

  virtual ~AudioBufferModJS();

  MODULE *myModule;

  ALuint myDoubleBuffer[2]; 

  ALenum          myFormat;     // internal format

};    

#endif //USEMIKMOD

#endif //_AUDIOBUFFERMODJS_H_
