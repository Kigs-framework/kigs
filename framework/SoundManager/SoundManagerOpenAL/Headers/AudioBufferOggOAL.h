#ifndef _AUDIOBUFFEROGGOAL_H_
#define _AUDIOBUFFEROGGOAL_H_

#include "AudioBufferOAL.h"
#include "al.h"
#include "FilePathManager.h"
/*#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>*/

#define BUFFER_SIZE (4096*16)

class AudioBufferOggOAL : public AudioBufferOAL
{
public:
    DECLARE_CLASS_INFO(AudioBufferOggOAL,AudioBufferOAL,SoundManager)

    AudioBufferOggOAL(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
  
    virtual void  Stream(AudioSource* src);
 
protected:

  bool  StreamOgg(ALuint buffer, bool Loop);

  virtual bool LoadSound(const kstl::string&);

  virtual ~AudioBufferOggOAL();

  SmartPointer<FileHandle>           myOggFile;  // file handle        
  /*OggVorbis_File  myOggStream;       // stream handle        
  vorbis_info*    myVorbisInfo;      // some formatting data        
  vorbis_comment* myVorbisComment; // user comments*/
  ALenum          myFormat;     // internal format

  ALuint myDoubleBuffer[2]; 

  char		myDecodeBuffer[BUFFER_SIZE];
  int		myDecodeBufferFilledPart;

  ALuint	myUnqueuedBuffers[2];

};    

#endif //_AUDIOBUFFEROAL_H_
