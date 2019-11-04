#ifndef _AUDIOBUFFERMODOAL_H_
#define _AUDIOBUFFERMODOAL_H_

#ifdef USEMIKMOD
#include "AudioBufferOAL.h"
#include "al.h"
#include "mikmod.h"


class AudioBufferModOAL : public AudioBufferOAL
{
public:
    DECLARE_CLASS_INFO(AudioBufferModOAL,AudioBufferOAL,SoundManager)

    AudioBufferModOAL(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
  
    virtual void  Stream(AudioSource* src);
 
protected:

  virtual bool LoadSound(const kstl::string&);

  virtual ~AudioBufferModOAL();

  MODULE *myModule;

  ALuint myDoubleBuffer[2]; 

  ALenum          myFormat;     // internal format

};    

#endif //USEMIKMOD

#endif //_AUDIOBUFFERMODOAL_H_
