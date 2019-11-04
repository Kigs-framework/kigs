#include "PrecompiledHeaders.h"
#include "AudioBufferOggJS.h"
#include "AudioSourceJS.h"
#include "Core.h"
//#include "AL/alu.h"
//#include "AL/alut.h"




IMPLEMENT_CLASS_INFO(AudioBufferOggJS)

AudioBufferOggJS::AudioBufferOggJS(const kstl::string& name,CLASS_NAME_TREE_ARG) : AudioBufferJS(name,PASS_CLASS_NAME_TREE_ARG)
{
  myOggFile=0;
 // alGenBuffers(2, myDoubleBuffer);

  myUnqueuedBuffers[0]=myDoubleBuffer[0];
  myUnqueuedBuffers[1]=myDoubleBuffer[1];
  myDecodeBufferFilledPart=0;
}

AudioBufferOggJS::~AudioBufferOggJS()
{ 
  //ov_clear(&myOggStream);
  if(myOggFile)
  {
    Platform_fclose(myOggFile);    
  }
  alDeleteBuffers(2, myDoubleBuffer);
}    

bool AudioBufferOggJS::LoadSound(const kstl::string& fullname)
{
  if((myOggFile = Platform_fopen(fullname.c_str(), "rb")))
  {
    int result;
         
    /*if((result = ov_open(myOggFile, &myOggStream, NULL, 0)) >= 0)    
    {       
      myVorbisInfo = ov_info(&myOggStream, -1);
      myVorbisComment = ov_comment(&myOggStream, -1);
      if(myVorbisInfo->channels == 1)
        myFormat = AL_FORMAT_MONO16;
      else 
        myFormat = AL_FORMAT_STEREO16; 

      return true;
    }*/
  }

  return false;
}

void  AudioBufferOggJS::Stream(AudioSource* src)
{
	AudioSourceJS* JSsrc=(AudioSourceJS*)src;

	int queued;   
	bool Loop;
	src->getValue(LABEL_TO_ID(IsLooping),Loop);

	alGetSourcei(JSsrc->GetJSSourceID(), AL_BUFFERS_QUEUED, &queued);

	if(queued==0)
	{
		// ask to fill the full buffer at start
		myDecodeBufferFilledPart=-1;
		if(StreamOgg(myDoubleBuffer[0],Loop))
		{
			alSourceQueueBuffers(JSsrc->GetJSSourceID(), 1, &myDoubleBuffer[0]);	
			myUnqueuedBuffers[0]=-1;
			myDecodeBufferFilledPart=0;
		}
		myDecodeBufferFilledPart=-1;
		if(StreamOgg(myDoubleBuffer[1],Loop))
		{
			alSourceQueueBuffers(JSsrc->GetJSSourceID(), 1, &myDoubleBuffer[1]);	
			myUnqueuedBuffers[1]=-1;
			myDecodeBufferFilledPart=0;
		}
	}
	else
	{
		int processed;
		int unqueuedBufferIndex=0;
		while(myUnqueuedBuffers[unqueuedBufferIndex]!=-1)
		{
			unqueuedBufferIndex++;
			if(unqueuedBufferIndex>1)
			{
				KIGS_ERROR("More than two OpenAL buffers are unqueued ?",2);
				unqueuedBufferIndex=0;
				break;
			}
		}
		alGetSourcei(JSsrc->GetJSSourceID(), AL_BUFFERS_PROCESSED, &processed);
		while(processed--)
		{     
			if(unqueuedBufferIndex>1)
			{
				KIGS_ERROR("More than two OpenAL buffers are unqueued ?",2);
				unqueuedBufferIndex=0;
			}

			alSourceUnqueueBuffers(JSsrc->GetJSSourceID(), 1, &myUnqueuedBuffers[unqueuedBufferIndex]);
			unqueuedBufferIndex++;
		}
	}

	// fill buffer and queue it if needed
	if(myUnqueuedBuffers[0]!=-1)
	{
		if(StreamOgg(myUnqueuedBuffers[0],Loop))
		{
			myDecodeBufferFilledPart=0;
			alSourceQueueBuffers(JSsrc->GetJSSourceID(), 1, &myUnqueuedBuffers[0]); 
			// if next buffer need to be filled also
			myUnqueuedBuffers[0]=myUnqueuedBuffers[1];
			myUnqueuedBuffers[1]=-1;
		}
	}
}


bool  AudioBufferOggJS::StreamOgg(ALuint buffer, bool Loop)
{
	bool returnvalue=false;
	/*bool doItUntilFull=true;
	if(myDecodeBufferFilledPart==-1)
	{
		doItUntilFull=false;
		myDecodeBufferFilledPart=0;
	}
	int  section;  
	int  result; 
	int		repeat=4;
	while( myDecodeBufferFilledPart < BUFFER_SIZE ) 
	{       
		result = ov_read(&myOggStream, myDecodeBuffer + myDecodeBufferFilledPart, BUFFER_SIZE - myDecodeBufferFilledPart, 0, 2, 1, &section);   

		if(result > 0) 
		{
			myDecodeBufferFilledPart += result;  
			// ok buffer is full
			if(myDecodeBufferFilledPart == BUFFER_SIZE)
			{
				returnvalue=true;
			}
		}
		else
		{
			 if(result < 0)    
			 {
				 // ERREUR de LECTURE
				return false;
			 }
			 else 
			 {
				 // result = 0... Fin du fichier
				 if (Loop) 
				 {
					 // in loop mode, start reading again from the beginning
					 ov_raw_seek(&myOggStream,0);
				 }
				 else 
				 {	
					 // ok, we have finish reading the file and not in loop mode, do add last
					 // part of the buffer and exit
					 returnvalue=true;
					 break;
				 }
			 }
		}
		if(doItUntilFull)
		{
			repeat--;
			if(repeat==0)
			{
				break;
			}
		}
	}   
	if(myDecodeBufferFilledPart == 0)   
	{
		return false;    
	}
	if(returnvalue)
	{
		alBufferData(buffer, myFormat, myDecodeBuffer, myDecodeBufferFilledPart, myVorbisInfo->rate); 
	}*/
	return returnvalue;
}
