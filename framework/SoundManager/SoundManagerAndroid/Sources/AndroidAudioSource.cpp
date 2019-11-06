#include "PrecompiledHeaders.h"
#include "AndroidAudioSource.h"
#include "AndroidAudioBuffer.h"
#include "AndroidAudioListener.h"
#include "ModuleSoundManager.h"
#include <jni.h>


IMPLEMENT_CLASS_INFO(AndroidAudioSource)

AndroidAudioSource::AndroidAudioSource(const kstl::string& name,CLASS_NAME_TREE_ARG)
: AudioSource(name,PASS_CLASS_NAME_TREE_ARG),
myIsAttenuated(*this,false,LABEL_AND_ID(IsAttenuated),false),
myAttenuation(*this,false,LABEL_AND_ID(Attenuation),KFLOAT_ONE),
myIsComputedPan(*this,false,LABEL_AND_ID(IsComputedPan),false),
myPan(*this,false,LABEL_AND_ID(Pan),KFLOAT_ZERO),
wasPaused(false)
{
	mySoundStreamStruct=0;
	myLastVolume=-KFLOAT_ONE;
	myLastLeftVolume=-KFLOAT_ONE;
	myLastPitch=-KFLOAT_ONE;
	myState.changeNotificationLevel(Owner);
}


AndroidAudioSource::~AndroidAudioSource() 
{
}


void AndroidAudioSource::Protected1Draw(TravState* state)
{
	// if no buffer, exit
	if(!myBuffer)
	{
		return;
	}
	
	int sourceState=(int)myState;

	if(sourceState && (myIsAttenuated || myIsComputedPan))
	{
		// compute relative position to the current listener
		Point3D pos(myLocalToGlobalMatrix.e[3][0],myLocalToGlobalMatrix.e[3][1],myLocalToGlobalMatrix.e[3][2]);
		
		//myLocalToGlobalMatrix.TransformPoints(&pos,1);
		
		kstl::vector<AudioListener*>& listenerStack=mySoundManager->GetListenerStack();
		unsigned long currentsize=listenerStack.size();
		if(currentsize)
		{
			listenerStack[currentsize-1]->GetGlobalToLocal().TransformPoints(&pos,1);
		}
		
		if(myIsAttenuated)
		{
			// compute distance between this source and the current listener (in 2D)
			kfloat dist= Norm(pos);
			
			// compute attenuation using the <Inverse Distance Clamped> model
			kfloat minDist, maxDist, factor;
			
			m_ReferenceDistance.getValue(minDist);
			m_MaxDistance.getValue(maxDist);
			m_RollOfFactor.getValue(factor);
			
			/*****
				attenuation formula
				G= gain, M= minimum distance, R= roll off factor, D= distance
				
				gain
				G= M / (M + R*(D-M))
				
				roll off factor
				R= (M/G - M)* 1/(D-M)
			*****/
			
			if(dist>maxDist)
				// above maximum distance, the sound is completly attenuated
				myAttenuation = KFLOAT_ZERO;//minDist/(minDist + factor*(maxDist-minDist));
			else if(dist>minDist)
				// above minimum distance, sound is attenuated based on distance
				myAttenuation = minDist/(minDist + factor*(dist-minDist));
			else
				// under minimum distance, the sound isn't attenuated
				myAttenuation = KFLOAT_ONE;
			
			/*
			printf("minDist=%.2f|maxDist=%.2f|factor=%.2f|gain=%.2f|distance=%.2f\n",
			        CastToFloat(minDist),CastToFloat(maxDist),CastToFloat(factor),
			        CastToFloat(myAttenuation),CastToFloat(dist));
			*/
		
		}
		
		if(myIsComputedPan)
		{
			Vector3D v= pos;
			v.Normalize();
			
			kfloat pan;
			kfloat ratio= KFLOAT_ONE/KFLOAT_CONST(1.58f);
			if(fabsf(pos.x)<KFLOAT_ONE)      pan= KFLOAT_ZERO;
			else if(fabsf(pos.y)<KFLOAT_ONE) pan= v.x;
			else
			{
				if(v.y>KFLOAT_ZERO) pan= ratio*atanf(v.x/v.y);
				else                pan= ratio*-atanf(v.x/v.y);
			}
			myPan= MIN(MAX(pan,KFLOAT_CONST(-1.0f)),KFLOAT_CONST(1.0f));
		}
		
	}
	
		// update buffer
	if(myBuffer && myBuffer->IsInit()) 
	{
		myBuffer->Stream(this);
	}
	
}

void AndroidAudioSource::NotifyUpdate(const unsigned int  labelid )
{
	// update buffer
	if(myBuffer && myBuffer->IsInit()) 
	{
		myBuffer->Stream(this);
	}
}