#include "PrecompiledHeaders.h"
#include "SoundManagerAndroid.h"
#include "AndroidAudioBuffer.h"
#include "AndroidAudioSource.h"
#include "AndroidAudioListener.h"

#include "Core.h"

SoundManagerAndroid* gInstanceSoundManagerAndroid = 0;

jmethodID 	SoundManagerAndroid::Load=0;
jmethodID 	SoundManagerAndroid::Play=0;
jmethodID 	SoundManagerAndroid::Pause=0;
jmethodID	SoundManagerAndroid::Stop=0;
jmethodID 	SoundManagerAndroid::SetVolume=0;
jmethodID	SoundManagerAndroid::SetPitch=0;
jmethodID	SoundManagerAndroid::Unload=0;
jmethodID	SoundManagerAndroid::isPlaying=0;
	
jclass		SoundManagerAndroid::myJavaAudioBufferManager=0;

IMPLEMENT_CLASS_INFO(SoundManagerAndroid)

SoundManagerAndroid::SoundManagerAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG)
: ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{

}


SoundManagerAndroid::~SoundManagerAndroid() {}


void SoundManagerAndroid::Init(Core* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core,"SoundManagerAndroid", params);
	
	DECLARE_FULL_CLASS_INFO(core,AndroidAudioSource,AudioSource,SoundManager)
	DECLARE_FULL_CLASS_INFO(core,AndroidAudioListener,AudioListener,SoundManager)
	DECLARE_FULL_CLASS_INFO(core,AndroidAudioListenerActivator,AudioListenerActivator,SoundManager)
	DECLARE_FULL_CLASS_INFO(core,AndroidAudioBuffer,AudioBuffer,SoundManager)
	
	JNIEnv* g_env = KigsJavaIDManager::getEnv();
	
	jclass pMaClasse = g_env->FindClass("com/assoria/soundmanager/AudioBuffer");
	myJavaAudioBufferManager = (jclass)g_env->NewGlobalRef(pMaClasse);
	
	
	
	Load=g_env->GetStaticMethodID(myJavaAudioBufferManager, "Load", "(Ljava/lang/String;Z)Lcom/assoria/soundmanager/SoundResourceStruct;");
	
	Play=g_env->GetStaticMethodID(myJavaAudioBufferManager, "Play", "(Lcom/assoria/soundmanager/SoundResourceStruct;ZFFF)Lcom/assoria/soundmanager/SoundStreamStruct;");
	
	Pause=g_env->GetStaticMethodID(myJavaAudioBufferManager, "Pause", "(Lcom/assoria/soundmanager/SoundStreamStruct;Z)Z");
	
	Stop=g_env->GetStaticMethodID(myJavaAudioBufferManager, "Stop", "(Lcom/assoria/soundmanager/SoundStreamStruct;)Z");
	
	SetVolume=g_env->GetStaticMethodID(myJavaAudioBufferManager, "SetVolume", "(Lcom/assoria/soundmanager/SoundStreamStruct;FF)Z");
	
	SetPitch=g_env->GetStaticMethodID(myJavaAudioBufferManager, "SetPitch", "(Lcom/assoria/soundmanager/SoundStreamStruct;F)Z");
	
	Unload=g_env->GetStaticMethodID(myJavaAudioBufferManager, "Unload", "(Lcom/assoria/soundmanager/SoundResourceStruct;)Z");
	
	isPlaying=g_env->GetStaticMethodID(myJavaAudioBufferManager, "isPlaying", "(Lcom/assoria/soundmanager/SoundStreamStruct;)Z");
	
	InitModifiable();
}


void SoundManagerAndroid::InitModifiable()
{


	// init CoreModifiable
	CoreModifiable::InitModifiable();
}


void SoundManagerAndroid::Close()
{
	
	JNIEnv* g_env = KigsJavaIDManager::getEnv();
	
	g_env->DeleteGlobalRef(myJavaAudioBufferManager);
	myJavaAudioBufferManager=0;
	
	BaseClose();
}


void SoundManagerAndroid::Update(const Timer& /*timer*/,void* /*addParam*/)
{
	
}


ModuleBase* MODULEINITFUNC(Core* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	Core::ModuleStaticInit(core);

	ModuleBase* gInstanceSoundManagerAndroid=new SoundManagerAndroid("SoundManagerAndroid");
	gInstanceSoundManagerAndroid->Init(core, params);
	
	return gInstanceSoundManagerAndroid;
}
