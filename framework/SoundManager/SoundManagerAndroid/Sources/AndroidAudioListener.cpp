#include "PrecompiledHeaders.h"
#include "AndroidAudioListener.h"
#include "TravState.h"

IMPLEMENT_CLASS_INFO(AndroidAudioListenerActivator)


AndroidAudioListenerActivator::AndroidAudioListenerActivator(const kstl::string& name,CLASS_NAME_TREE_ARG)
: AudioListenerActivator(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}

bool AndroidAudioListenerActivator::PreDraw(TravState* state)
{
	Matrix3x4 myGlobalToLocal=(myFatherListener->GetGlobalToLocal());
	
	kdouble currentTime= state->GetTime();
	
	if(myLastTime!=currentTime)
	{
		if(myLastTime==KDOUBLE_CONST(-1.f))
		{
			myLastTime= currentTime;
			myLastPos.Set(myGlobalToLocal.e[3][0],myGlobalToLocal.e[3][1],myGlobalToLocal.e[3][2]);  
		}
		else
		{
			myLastPos.Set(myGlobalToLocal.e[3][0],myGlobalToLocal.e[3][1],myGlobalToLocal.e[3][2]);  
			myLastTime=state->GetTime();
		}
	}

	myCurrentPos.Set(myGlobalToLocal.e[3][0],myGlobalToLocal.e[3][1],myGlobalToLocal.e[3][2]);

	if(Drawable::PreDraw(state))
	{	
		myFatherListener->PushOnStack(state);
		return true;
	}
	
	return false;
}



IMPLEMENT_CLASS_INFO(AndroidAudioListener)

AndroidAudioListener::AndroidAudioListener(const kstl::string& name,CLASS_NAME_TREE_ARG)
: AudioListener(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}

AndroidAudioListener::~AndroidAudioListener() {}

