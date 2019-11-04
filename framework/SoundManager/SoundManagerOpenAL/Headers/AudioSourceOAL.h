#ifndef _AUDIOSOURCEOAL_H_
#define _AUDIOSOURCEOAL_H_

#include "AudioSource.h"
#include "al.h"


class AudioSourceOAL : public AudioSource
{
public:
    DECLARE_CLASS_INFO(AudioSourceOAL,AudioSource,SoundManager)

    AudioSourceOAL(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    ALuint  GetOALSourceID(){return mySource;}

protected:

	virtual	void	Protected1Draw(TravState* state);

  virtual ~AudioSourceOAL();

  ALuint    mySource;

  kdouble    myLastTime;
  Point3D   myLastPos;  

};    

#endif //_AUDIOSOURCEOAL_H_
