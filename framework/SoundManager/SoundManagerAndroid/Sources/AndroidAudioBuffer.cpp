#include "AndroidAudioBuffer.h"
#include "AndroidAudioSource.h"
#include "SoundManagerAndroid.h"
#include "Core.h"


IMPLEMENT_CLASS_INFO(AndroidAudioBuffer)


AndroidAudioBuffer::AndroidAudioBuffer(const kstl::string& name,CLASS_NAME_TREE_ARG)
: AudioBuffer(name,PASS_CLASS_NAME_TREE_ARG)
{
	mySoundResourceStruct=0;
}


AndroidAudioBuffer::~AndroidAudioBuffer()
{
	if(mySoundResourceStruct)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv();
		g_env->CallStaticBooleanMethod(SoundManagerAndroid::myJavaAudioBufferManager,  SoundManagerAndroid::Unload, mySoundResourceStruct);
		g_env->DeleteGlobalRef(mySoundResourceStruct);
		mySoundResourceStruct=0;
	}
}


bool AndroidAudioBuffer::LoadSound(const kstl::string& fileName) 
{
	bool result=false;
	JNIEnv* g_env = KigsJavaIDManager::getEnv();
	
	jstring fname=g_env->NewStringUTF(fileName.c_str());
	jobject	fdHandle=0;
	fdHandle=g_env->CallStaticObjectMethod(SoundManagerAndroid::myJavaAudioBufferManager,  SoundManagerAndroid::Load, fname,false);
	if(fdHandle)
	{
		mySoundResourceStruct=(jobject)g_env->NewGlobalRef(fdHandle);
		g_env->DeleteLocalRef(fdHandle);
		result=true;
	}
	
	g_env->DeleteLocalRef(fname);	
	return result;
}

/// method used for streaming/updating
void AndroidAudioBuffer::Stream(AudioSource* audioSrc)
{

	AndroidAudioSource* src= (AndroidAudioSource*) audioSrc;
	
	// check "real" playing state 
	bool playing = false;
	JNIEnv* g_env = KigsJavaIDManager::getEnv();
	if(src->mySoundStreamStruct)
	{
		playing=(bool)g_env->CallStaticBooleanMethod(SoundManagerAndroid::myJavaAudioBufferManager,  SoundManagerAndroid::isPlaying, src->mySoundStreamStruct);
	}
	bool channelPlaying = false;
	// check channel playing state
	src->getValue(_S_2_ID("IsPlaying"), channelPlaying);
	
	// get current state
	unsigned int  state = STOP;
	src->getValue(_S_2_ID("State"), state);
	
	
	// check current state
	switch(state)
	{
		case STOP:
		{
			src->setValue(_S_2_ID("IsPlaying"), false);
			src->wasPaused= false;
			if(playing) 
			{
				Empty(audioSrc);
			}
			
		} break;
		
		case PLAY:
		{
			// sound is not playing
			if(!playing) 
			{
				// manage un-pause action
				if(src->wasPaused)
				{
					src->wasPaused= false;
					src->setValue(_S_2_ID("IsPlaying"), true);
					g_env->CallStaticBooleanMethod(SoundManagerAndroid::myJavaAudioBufferManager,  SoundManagerAndroid::Pause, src->mySoundStreamStruct,false);
					
				}
				else if(!channelPlaying) // not in pause and channel is not playing then start to play
				{
					// enter in play mode
					src->setValue(_S_2_ID("IsPlaying"), true);
					// set volume speed and pan
					update(src,true);
					
				}
				else // sound play is finished
				{
					src->setValue(_S_2_ID("IsPlaying"), false);
					src->Stop();
					src->wasPaused= false;
					Empty(audioSrc);
				}
				
			}
			else 
			{
				update(src);
			}
		} break;
		
		case PAUSE:
		{
			if(!src->wasPaused)
			{
				src->wasPaused= true;
				src->setValue(_S_2_ID("IsPlaying"), false);
				g_env->CallStaticBooleanMethod(SoundManagerAndroid::myJavaAudioBufferManager,  SoundManagerAndroid::Pause, src->mySoundStreamStruct,true);
			}
			
		} break;
		
		default:
			break;
	}
	
}
	
/// method used for clearing the stream
void AndroidAudioBuffer::Empty(AudioSource* audioSrc)
{
	AndroidAudioSource* src= (AndroidAudioSource*) audioSrc;
	JNIEnv* g_env = KigsJavaIDManager::getEnv();
	g_env->CallStaticBooleanMethod(SoundManagerAndroid::myJavaAudioBufferManager,  SoundManagerAndroid::Stop, src->mySoundStreamStruct);
	g_env->DeleteGlobalRef(src->mySoundStreamStruct);
	src->mySoundStreamStruct=0;
	src->setValue(_S_2_ID("IsPlaying"), false);
}

void AndroidAudioBuffer::update(AudioSource* audioSrc,bool play)
{
	AndroidAudioSource* src= (AndroidAudioSource*) audioSrc;
	
	JNIEnv* g_env = KigsJavaIDManager::getEnv();

	// get source parameters
	kfloat gain, pitch, attenuation, fpan;
	audioSrc->getValue(_S_2_ID("Gain"),        gain);
	audioSrc->getValue(_S_2_ID("Pitch"),       pitch);
	audioSrc->getValue(_S_2_ID("Attenuation"), attenuation);
	audioSrc->getValue(_S_2_ID("Pan"), fpan);
	
	kfloat volume  = 			gain*attenuation;

	kfloat volumeLeft;
	
	if(fpan>0)
	{
		volumeLeft=0.5f+fpan*0.5f;
	}
	else
	{
		volumeLeft=(KFLOAT_ONE+fpan)*0.5f;
	}

	kfloat volumeRight      = 	(KFLOAT_ONE-volumeLeft);
	
	if(play)
	{
		src->myLastVolume = volume;
		src->myLastLeftVolume=volumeLeft;
			
		kfloat coef=volumeLeft;
		if(volumeRight>coef)
		{
			coef=volumeRight;
		}
	
		coef=volume/coef;
		volumeRight*=coef;
		volumeLeft*=coef;
		src->myLastPitch = pitch;
		jobject	fdHandle=0;
		bool looping=false;
		src->getValue(_S_2_ID("IsLooping"), looping);
		fdHandle=g_env->CallStaticObjectMethod(SoundManagerAndroid::myJavaAudioBufferManager,  SoundManagerAndroid::Play, mySoundResourceStruct,looping,volumeLeft,volumeRight,pitch);
		if(fdHandle)
		{
			src->mySoundStreamStruct=(jobject)g_env->NewGlobalRef(fdHandle);
			g_env->DeleteLocalRef(fdHandle);
		}
	}
	else
	{
		if((src->myLastVolume != volume) ||	(src->myLastLeftVolume!=volumeLeft))
		{
			src->myLastVolume = volume;
			src->myLastLeftVolume=volumeLeft;
			
			kfloat coef=volumeLeft;
			if(volumeRight>coef)
			{
				coef=volumeRight;
			}
		
			coef=volume/coef;
			volumeRight*=coef;
			volumeLeft*=coef;
			
			g_env->CallStaticBooleanMethod(SoundManagerAndroid::myJavaAudioBufferManager,  SoundManagerAndroid::SetVolume, src->mySoundStreamStruct,volumeLeft,volumeRight);
		}
		
		if(src->myLastPitch != pitch)
		{
			src->myLastPitch = pitch;
			g_env->CallStaticBooleanMethod(SoundManagerAndroid::myJavaAudioBufferManager,  SoundManagerAndroid::SetPitch, src->mySoundStreamStruct,pitch);
		}
	}
	
}