#include "PrecompiledHeaders.h"
#include "ModuleSoundManager.h"
#include "AudioSourceJS.h"
#include "AudioBufferJS.h"
#include "AudioListener.h"
#include "TravState.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(AudioSourceJS)


AudioSourceJS::AudioSourceJS(const kstl::string& name,CLASS_NAME_TREE_ARG) : AudioSource(name,PASS_CLASS_NAME_TREE_ARG)
{
	alGenSources(1,&mySource);
	assert(alGetError() == AL_NO_ERROR);
	myLastTime=-1;
}

AudioSourceJS::~AudioSourceJS()
{  
	alDeleteSources(1,&mySource);
	mySource = 0;
	assert(alGetError() == AL_NO_ERROR);
}    

void    AudioSourceJS::Protected1Draw(TravState* trstate)
{
	// if no buffer, exit
	if(!myBuffer)
	{
		return;
	}
	ALenum state=0;
	alGetSourcei(mySource, AL_SOURCE_STATE, &state);
	assert(alGetError() == AL_NO_ERROR);

	if( myState == 1) 
	{
		myBuffer->Stream(this);
		assert(alGetError() == AL_NO_ERROR);
	}
	// playing
	if( (myState == 1) && (state != AL_PLAYING))
	{


		// was playing before
		if(myIsPlaying&&(!myIsLooping))

		{
			myState=0;
			myIsPlaying=false;
		}
		else
		{
			
			alSourcePlay(mySource);
			assert(alGetError() == AL_NO_ERROR);
			myIsPlaying=true;
		}

	}

	if( (myState == 2) && (state !=AL_PAUSED)) // pause
	{
		alSourcePause(mySource);
		assert(alGetError() == AL_NO_ERROR);
	}

	if( (myState == 0) && (state !=AL_STOPPED )) // stop
	{
		alSourceStop(mySource);
		assert(alGetError() == AL_NO_ERROR);
	}

	Vector3D  v(0.0,0.0,0.0);
	//
	if(myLastTime!=trstate->GetTime())
	{
		if(myLastTime==-1)
		{
			myLastTime=trstate->GetTime();
			myLastPos.Set(myLocalToGlobalMatrix.e[3][0],myLocalToGlobalMatrix.e[3][1],myLocalToGlobalMatrix.e[3][2]);  
		}
		else
		{
			v.Set(myLocalToGlobalMatrix.e[3][0],myLocalToGlobalMatrix.e[3][1],myLocalToGlobalMatrix.e[3][2]);
			v-=myLastPos;
			v*=trstate->GetTime()-myLastTime;
			myLastPos.Set(myLocalToGlobalMatrix.e[3][0],myLocalToGlobalMatrix.e[3][1],myLocalToGlobalMatrix.e[3][2]);  
			myLastTime=trstate->GetTime();

		}
	}

	Point3D pos(myLocalToGlobalMatrix.e[3][0],myLocalToGlobalMatrix.e[3][1],myLocalToGlobalMatrix.e[3][2]);

	kstl::vector<AudioListener*>& listenerStack=mySoundManager->GetListenerStack();

	unsigned long currentsize=listenerStack.size();
	if(currentsize)
	{
		listenerStack[currentsize-1]->TransformSource(pos,v);
	}
	alSource3f(mySource,AL_VELOCITY,v.x,v.y,v.z);

	alSourcef (mySource, AL_PITCH,    myPitch     );
	alSourcef (mySource, AL_GAIN,     myGain      );
	//  alSourcei (mySource, AL_LOOPING,  myIsLooping );
	alSourcei (mySource, AL_LOOPING,  false );


	alSourcef(mySource,AL_REFERENCE_DISTANCE,m_ReferenceDistance);
	alSourcef(mySource,AL_MAX_DISTANCE,m_MaxDistance);
	alSourcef(mySource,AL_ROLLOFF_FACTOR,m_RollOfFactor);

	alSource3f(mySource,AL_POSITION,pos.x,pos.y,pos.z);
	assert(alGetError() == AL_NO_ERROR);
}


