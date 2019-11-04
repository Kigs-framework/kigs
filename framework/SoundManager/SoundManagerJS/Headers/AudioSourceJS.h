#ifndef _AUDIOSOURCEJS_H_
#define _AUDIOSOURCEJS_H_

#include "AudioSource.h"
//#include "al.h"


class AudioSourceJS : public AudioSource
{
public:
    DECLARE_CLASS_INFO(AudioSourceJS,AudioSource,SoundManager)

    AudioSourceJS(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    unsigned int  GetJSSourceID(){return mySource;}

protected:

	virtual	void	Protected1Draw(TravState* state) override;

  virtual ~AudioSourceJS();

  unsigned int    mySource;

  kdouble    myLastTime;
  Point3D   myLastPos;  

};    

#endif //_AUDIOSOURCEJS_H_
