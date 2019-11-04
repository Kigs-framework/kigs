#ifndef _AUDIOBUFFEROAL_H_
#define _AUDIOBUFFEROAL_H_

#include "AudioBuffer.h"
#include "al.h"
#include <inttypes.h>

class AudioBufferOAL : public AudioBuffer
{
protected:
	virtual ~AudioBufferOAL();

	ALuint  myBuffer;
	struct RIFF_Header 
	{
		char chunkID[4];
		uint32_t chunkSize;//size not including chunkSize or chunkID
		char format[4];
	};
	struct WAVE_Data
	{
		char subChunkID[4]; //should contain the word data
		uint32_t subChunk2Size; //Stores the size of the data block
	};

	struct WAVE_Format 
	{
		char subChunkID[4];
		uint16_t subChunkSize;
		short audioFormat;
		short numChannels;
		long sampleRate;
		long byteRate;
		short blockAlign;
		short bitsPerSample;
	};
	
public:
    DECLARE_CLASS_INFO(AudioBufferOAL,AudioBuffer,SoundManager)

    AudioBufferOAL(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    virtual void  Stream(AudioSource* src);
    virtual void  Empty(AudioSource* src);

	ALuint  GetOALBufferID(){return myBuffer;}

	//bool loadWavFile(const kstl::string filename);
	virtual bool LoadSound(const kstl::string&);
	virtual void Play();



};    

#endif //_AUDIOBUFFEROAL_H_
