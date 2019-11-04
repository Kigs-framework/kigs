#ifndef _SOUNDMANAGERANDROID_H_
#define _SOUNDMANAGERANDROID_H_

#include "ModuleBase.h"
#include <jni.h>

// module defines
class SoundManagerAndroid;
extern SoundManagerAndroid* gInstanceSoundManagerAndroid;

#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformSoundModuleInit
extern "C"
{
	extern ModuleBase* PlatformSoundModuleInit(Core* core, const kstl::vector<CoreModifiableAttribute*>* params);
}
#else
#define MODULEINITFUNC			ModuleInit
#endif


/**
* \class SoundManagerAndroid
* \brief manage sound playing for android
*/
class SoundManagerAndroid : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(SoundManagerAndroid,ModuleBase,SoundManager)
	
	/// Kigs constructor
	SoundManagerAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/// module init method
	void Init(Core* core, const kstl::vector<CoreModifiableAttribute*>* params);
	
	/// module close method
	void Close();
	
	/// module update method
	virtual void Update(const Timer& timer,void* /*addParam*/);   
	
	
	static jmethodID 	Load;
	static jmethodID 	Play;
	static jmethodID 	Pause;
	static jmethodID	Stop;
	static jmethodID 	SetVolume;
	static jmethodID	SetPitch;
	static jmethodID	Unload;
	static jmethodID	isPlaying;
	
	static jclass		myJavaAudioBufferManager;
	
protected:
	/// coreModifiable initialization
	virtual void InitModifiable();
	
	/// destructor
	virtual ~SoundManagerAndroid();
	

	

}; 

#endif //_SOUNDMANAGERANDROID_H_
