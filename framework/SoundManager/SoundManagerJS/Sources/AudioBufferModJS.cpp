#include "PrecompiledHeaders.h"
#include "AudioBufferModJS.h"
#include "AudioSourceJS.h"
#include "Core.h"

#ifdef USEMIKMOD

IMPLEMENT_CLASS_INFO(AudioBufferModJS)

AudioBufferModJS::AudioBufferModJS(const kstl::string& name,CLASS_NAME_TREE_ARG) : AudioBufferJS(name,PASS_CLASS_NAME_TREE_ARG)
{
  alGenBuffers(2, myDoubleBuffer);
   
  MikMod_RegisterAllDrivers();

  MikMod_RegisterAllLoaders();

  

  md_mode |= DMODE_SOFT_MUSIC;

  if (MikMod_Init("")) 
  {
    printf("Could not initialize sound, reason: %s\n", MikMod_strerror(MikMod_errno));
  }

  if(md_mode&DMODE_STEREO)
  {
     if(md_mode&DMODE_16BITS)
      myFormat=AL_FORMAT_STEREO16; 
    else
      myFormat=AL_FORMAT_STEREO8;
  }
  else
  {
   if(md_mode&DMODE_16BITS)
      myFormat=AL_FORMAT_MONO16;
    else
      myFormat=AL_FORMAT_MONO8;


  }
}

AudioBufferModJS::~AudioBufferModJS()
{ 
  if(myModule)
  {
    Player_Stop();
    Player_Free(myModule);
  }
  
  MikMod_Exit();
  alDeleteBuffers(2, myDoubleBuffer);
}    

bool AudioBufferModJS::LoadSound(const kstl::string& fullname)
{

  myModule = Player_Load((char*)fullname.c_str(), 64, 0);
  if (myModule)
  {
    Player_Start(myModule);
    return true;
  }

  return false;
}

void  AudioBufferModJS::Stream(AudioSource* src)
{

  if(!Player_Active()) 
  {
     return;
  }
    
  AudioSourceJS* JSsrc=(AudioSourceJS*)src;

  int queued=0;    
  
  alGetSourcei(JSsrc->GetJSSourceID(), AL_BUFFERS_QUEUED, &queued);

  if(queued==0)
  {
    MikModBufferIsDump[0]=1;
    MikModBufferIsDump[1]=1;

    MikMod_Update();

    if(MikModBufferDumpLength[0])
    {
      alBufferData(myDoubleBuffer[0], myFormat, MikModAudioBuffer[0], MikModBufferDumpLength[0], md_mixfreq);
      alSourceQueueBuffers(JSsrc->GetJSSourceID(), 1, &myDoubleBuffer[0]);	
      MikModBufferDumpLength[0]=0;
    }

    if(MikModBufferDumpLength[1])
    {
      alBufferData(myDoubleBuffer[1], myFormat, MikModAudioBuffer[1], MikModBufferDumpLength[1], md_mixfreq);
      alSourceQueueBuffers(JSsrc->GetJSSourceID(), 1, &myDoubleBuffer[1]);	
      MikModBufferDumpLength[1]=0;
    }
  }
  else
  {
    
    int processed=0;
    alGetSourcei(JSsrc->GetJSSourceID(), AL_BUFFERS_PROCESSED, &processed);
    while(processed--)
    {        
      ALuint buffer;
      alSourceUnqueueBuffers(JSsrc->GetJSSourceID(), 1, &buffer);

      if(buffer == myDoubleBuffer[0])
      {
        MikModBufferIsDump[0]=1;
        MikMod_Update();
        if(MikModBufferDumpLength[0])
        {
          alBufferData(myDoubleBuffer[0], myFormat, MikModAudioBuffer[0], MikModBufferDumpLength[0], md_mixfreq);
          alSourceQueueBuffers(JSsrc->GetJSSourceID(), 1, &myDoubleBuffer[0]);	
          MikModBufferDumpLength[0]=0;
        }

      }
      else if(buffer == myDoubleBuffer[1])
      {
        MikModBufferIsDump[1]=1;
        MikMod_Update();
        if(MikModBufferDumpLength[1])
        {
          alBufferData(myDoubleBuffer[1], myFormat, MikModAudioBuffer[1], MikModBufferDumpLength[1], md_mixfreq);
          alSourceQueueBuffers(JSsrc->GetJSSourceID(), 1, &myDoubleBuffer[1]);	
          MikModBufferDumpLength[1]=0;
        }
      } 
    }
  }
}

#endif // USEMIKMOD