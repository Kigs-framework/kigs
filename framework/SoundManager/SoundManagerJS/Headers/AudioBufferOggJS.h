#ifndef _AUDIOBUFFEROGGJS_H_
#define _AUDIOBUFFEROGGJS_H_

#include "AudioBufferJS.h"
//#include "al.h"
/*#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>*/

#define BUFFER_SIZE (4096*16)

class AudioBufferOggJS : public AudioBufferJS
{
public:
    DECLARE_CLASS_INFO(AudioBufferOggJS,AudioBufferJS,SoundManager)

    AudioBufferOggJS(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
  
    virtual void  Stream(AudioSource* src);
 
protected:

  bool  StreamOgg(unsigned int buffer, bool Loop);

  virtual bool LoadSound(const kstl::string&);

  virtual ~AudioBufferOggJS();

  FILE*           myOggFile;  // file handle        
  /*OggVorbis_File  myOggStream;       // stream handle        
  vorbis_info*    myVorbisInfo;      // some formatting data        
  vorbis_comment* myVorbisComment; // user comments*/
 // ALenum          myFormat;     // internal format

  unsigned int myDoubleBuffer[2]; 

  char		myDecodeBuffer[BUFFER_SIZE];
  int		myDecodeBufferFilledPart;

  unsigned int	myUnqueuedBuffers[2];

};    

#endif //_AUDIOBUFFERJS_H_
