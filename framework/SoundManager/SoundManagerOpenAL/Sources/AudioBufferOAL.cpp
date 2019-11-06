#include "PrecompiledHeaders.h"
#include <algorithm>
#include <cctype>
#include "AudioBufferOAL.h"
#include "AudioSourceOAL.h"
#include "Core.h"
#include "FilePathManager.h"

//#include "AL/alu.h"

/*
#include "AL/alut.h"
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>
*/

IMPLEMENT_CLASS_INFO(AudioBufferOAL)


AudioBufferOAL::AudioBufferOAL(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
AudioBuffer(name,PASS_CLASS_NAME_TREE_ARG)
, myBuffer(0)
{
}

AudioBufferOAL::~AudioBufferOAL()
{  
	KIGS_ASSERT(alGetError() == AL_NO_ERROR);
	if (myBuffer != 0)
		alDeleteBuffers(1,&myBuffer);
	KIGS_ASSERT(alGetError() == AL_NO_ERROR);
}   



bool AudioBufferOAL::LoadSound(const kstl::string& fullname)
{
	kstl::string ext = ".wav";
	kstl::string filefullname = fullname + ext;

	FilePathManager*	pathManager = (FilePathManager*)KigsCore::GetSingleton(_S_2_ID("FilePathManager"));

	kstl::string L_fullfilename;

	SmartPointer<FileHandle> fullfilenamehandle;
	fullfilenamehandle = pathManager->FindFullName(filefullname);

	if (fullfilenamehandle)
	{
		L_fullfilename = fullfilenamehandle->myFullFileName;
		int pos = L_fullfilename.rfind(".") + 1;
		if (pos != 0)
		{
			kstl::string extension = "";
			extension.append(L_fullfilename, pos, L_fullfilename.length() - pos);
			std::transform(extension.begin(), extension.end(), extension.begin(), (int(*)(int)) std::toupper);

			//Local Declarations
			FILE* soundFile = NULL;
			WAVE_Format wave_format;
			RIFF_Header riff_header;
			WAVE_Data wave_data;
			ALsizei* size = new ALsizei();
			ALsizei* frequency = new ALsizei();
			ALenum* format = new ALenum();
			unsigned char* data;

			try
			{
				soundFile = fopen(L_fullfilename.c_str(), "rb");
				if (!soundFile)
				{
					throw (filefullname);
					return false;
				}

				//check for RIFF and WAVE tag in memory
				fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);
				if (memcmp(riff_header.chunkID, "RIFF", 4) != 0 || 
					memcmp(riff_header.format, "WAVE", 4) != 0)
				{
					throw ("Invalid RIFF or WAVE Header");
					fclose(soundFile);
					return false;
				}

				//Read in the 2nd chunk for the wave info
				fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);
				//check for fmt tag in memory
				if (memcmp(wave_format.subChunkID, "fmt ", 4) != 0)
				{
					throw ("Invalid Wave Format");
					fclose(soundFile);
					return false;
				}

				do
				{
					//Read in the the last byte of data before the sound file
					fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);

					//check for data tag in memory
					if (memcmp(wave_data.subChunkID, "data", 4) == 0)
					{
						break;
					}
					else if (memcmp(wave_data.subChunkID, "LIST", 4) == 0)
						fseek(soundFile, wave_data.subChunk2Size, SEEK_CUR);
				} while (1);

				//Allocate memory for data
				//
				data = new unsigned char[wave_data.subChunk2Size];

				// Read in the sound data into the soundData variable
				if (!fread(data, wave_data.subChunk2Size, 1, soundFile))
				{
					throw ("error loading WAVE data into struct!");
					fclose(soundFile);
					return false;
				}

				//Now we set the variables that we passed in with the
				//data from the structs
				*size = wave_data.subChunk2Size;
				*frequency = wave_format.sampleRate;

				//The format is worked out by looking at the number of
				//channels and the bits per sample.
				if (wave_format.numChannels == 1)
				{
					if (wave_format.bitsPerSample == 8)
						*format = AL_FORMAT_MONO8;
					else if (wave_format.bitsPerSample == 16)
						*format = AL_FORMAT_MONO16;
				}
				else if (wave_format.numChannels == 2)
				{
					if (wave_format.bitsPerSample == 8)
						*format = AL_FORMAT_STEREO8;
					else if (wave_format.bitsPerSample == 16)
						*format = AL_FORMAT_STEREO16;
				}
				//create our openAL buffer and check for success
				alGenBuffers(1, &myBuffer);
				//now we put our data into the openAL buffer and
				//check for success
				alBufferData(myBuffer, *format, (void*)data, *size, *frequency);
				//clean up and return true if successful
				fclose(soundFile);

				return true;
			}
			catch (std::string error)
			{
				//clean up memory if wave loading fails
				if (soundFile != NULL)
					fclose(soundFile);
				//return false to indicate the failure to load wave
				return false;
			}

		}
	}
	return false;
}

void  AudioBufferOAL::Stream(AudioSource* src)
{
	if (myBuffer == 0)
		return;
	AudioSourceOAL* oalsrc=(AudioSourceOAL*)src;

	int currentID;

	alGetSourcei(oalsrc->GetOALSourceID(), AL_BUFFER, &currentID);

	if(currentID != GetOALBufferID())
	{
		alSourcei(oalsrc->GetOALSourceID(), AL_BUFFER, GetOALBufferID() );
		KIGS_ASSERT(alGetError() == AL_NO_ERROR);
	}
	/*
  int queued;    
  
  alGetSourcei(oalsrc->GetOALSourceID(), AL_BUFFERS_QUEUED, &queued);

  if(queued==0)
  {
    alSourceQueueBuffers(oalsrc->GetOALSourceID(), 1, &myBuffer);	
  }
*/
}

void  AudioBufferOAL::Empty(AudioSource* src)
{
	if (myBuffer == 0)
		return;

	AudioSourceOAL* oalsrc=(AudioSourceOAL*)src;
	ALuint SourceID = oalsrc->GetOALSourceID();
	
	// source must be in stopped state before releasing current buffer
	alSourceStop(oalsrc->GetOALSourceID());
	KIGS_ASSERT(alGetError() == AL_NO_ERROR);
	
	alSourcei(SourceID, AL_BUFFER, 0);
	
	int queued;
	alGetSourcei(SourceID, AL_BUFFERS_QUEUED, &queued);
	
	KIGS_ASSERT(alGetError() == AL_NO_ERROR);
	while(queued--)
	{
		ALuint buffer;      
		alSourceUnqueueBuffers(oalsrc->GetOALSourceID(), 1, &buffer);
		KIGS_ASSERT(alGetError() == AL_NO_ERROR);
	}
	
}

 

void AudioBufferOAL::Play()
{
	if (myBuffer == 0)
		return;
	AudioSourceOAL* as = (AudioSourceOAL*)(KigsCore::GetInstanceOf("as", _S_2_ID("AudioSource")));
	ALuint source = as->GetOALSourceID();
	alSourceQueueBuffers(source, 1, &myBuffer);
	alSourcePlay(source);
	
}